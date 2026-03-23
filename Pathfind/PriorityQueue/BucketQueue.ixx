//
// Created by nickberryman on 10/12/25.
//
module;
#include <type_traits>
#include "Logger.h"
#include <cstring>

//#include <iostream>

export module SG_Pathfind:BucketQueue;
import :BasePriorityQueue;
import Logger;
import SG_Grid;
import SG_Allocator;


//TODO make const correct, along with the other priority queues
//TODO double-check that A* with an admissible heuristic still satisfies the 'monotonic increasing' assumption that this uses here
export namespace SG_Pathfind::PriorityQueue {
    template<typename T, typename priority_t, typename bucketSize_t, typename InsideArenaType, bool fullDecreaseKey = true, bool fifoOnTie = false>
    class BucketQueue : private BasePriorityQueue<T, priority_t>{
        static_assert(std::is_integral_v<priority_t>, "Priority must be an integral type.");
    public:
        BucketQueue(InsideArenaType& arena, const priority_t& maxPriority_, const priority_t& minPriority_ = 0) :
            buckets(arena)
            ,bucketCount(maxPriority_ - minPriority_ + 1)
            ,bucketStarts(arena, bucketCount+1)
            ,bucketLengths(arena, bucketCount+1)
            ,minIndex(0)
            ,minPriority(minPriority_)
            ,length_(0)
            ,maxIndex(0)
        #ifndef NDEBUG
            ,maxP(maxPriority_-minPriority_)
        #endif
        {
            LOGGER_ASSERT_EXCEPT(maxPriority_ >= minPriority_);
            std::memset(bucketLengths.impl(), 0, bucketCount * sizeof(priority_t)); // Set all buckets to empty
        };

        inline const priority_t& length(){ return length_; }

        inline priority_t encodePriority(const priority_t& priority){ return priority - minPriority; } //YOU NEED TO CALL THIS FIRST (EXTERNAL USE)
        inline priority_t decodePriority(const priority_t& priority){ return priority + minPriority; }

        inline bool insert(const T& value, const priority_t& priority, const SG_Grid::u_coordinate_t& lastPriority = 0 ) {
            if (lastPriority <= priority && lastPriority > 0) return false; // Already visited & visited better -> skip
            if (priority < minIndex) minIndex = priority;

            if (lastPriority == 0) { //Not visited -> insert
                forceInsert(value, priority);
                return true;
            }
            if constexpr (fullDecreaseKey) {
                void* node = nullptr;
                priority_t bucket = minIndex;
                for (;bucket < bucketCount; ++bucket) {
                    node = inBucket(value, bucket);
                    if (node != nullptr) break;
                }
                if (node != nullptr) {
                    decreaseKey(node, bucket, priority); // Already visited & visited was worse + currently in queue -> decreaseKey
                    return true;
                }
            }
            forceInsert(value, priority); // Already visited & visited was worse + NOT currently in queue -> insert
            return true;
        }

        inline const T& valueAt(const priority_t& bucket, const priority_t& indexInBucket = 0) {
            return buckets.get_atNode(nodeAt(bucket, indexInBucket));
        }

        inline const priority_t& findMin() {
            LOGGER_ASSERT_EXCEPT(length_ > 0)
            for (; minIndex < bucketCount; ++minIndex) if (bucketLengths[minIndex] > 0) return minIndex;
            return minIndex;
        }

        inline T extractMin() {
            LOGGER_ASSERT_EXCEPT(length_ > 0)
            void* outNode = nodeAt(findMin());
            bucketStarts[minIndex] = buckets.node_after(outNode);
            removeAt(minIndex, outNode);
            return buckets.get_atNode(outNode);
        }

        inline void decreaseKey(void* const& nodeAdr, const priority_t& oldPriority, const priority_t& newPriority ) {
            forceInsert(buckets.get_atNode(nodeAdr), newPriority);
            if constexpr (fullDecreaseKey) removeAt(oldPriority, nodeAdr);
        }

        inline void* decreaseKeyAndReturn(void* const& nodeAdr, const priority_t& oldPriority, const priority_t& newPriority ) {
            auto out = forceInsert(buckets.get_atNode(nodeAdr), newPriority);
            if constexpr (fullDecreaseKey) removeAt(oldPriority, nodeAdr);
            return out;
        }

        inline void* forceInsert(const T& value, const priority_t& priority ) {
            LOGGER_ASSERT_EXCEPT(priority <= maxP)
            LOGGER_ASSERT_EXCEPT(priority >= minIndex)

            if (priority > maxIndex) maxIndex = priority;
            if (priority < minIndex) minIndex = priority;
            ++length_;
            ++bucketLengths[priority];

            if (bucketLengths[priority] == 1) {
                for (auto i = priority+1; i <= maxIndex; ++i) if (bucketLengths[i] != 0) return bucketStarts[priority] = buckets.construct_beforeNode(bucketStarts[i], value);
                return bucketStarts[priority] = buckets.construct_back(value);
            }
            if constexpr (!fifoOnTie) return bucketStarts[priority] = buckets.construct_beforeNode(bucketStarts[priority], value);
            else {
                for (auto i = priority+1; i <= maxIndex; ++i) if (bucketLengths[i] != 0) return buckets.construct_beforeNode(bucketStarts[i], value);
                return buckets.construct_back(value);
            }
        }

    private:
        SG_Allocator::LinkedList<InsideArenaType, T, priority_t, true, true, true, false> buckets;
        priority_t bucketCount;
        SG_Allocator::RuntimeArray<void*, priority_t> bucketStarts;
        SG_Allocator::RuntimeArray<bucketSize_t, priority_t> bucketLengths;

        priority_t minIndex;
        priority_t minPriority;
        priority_t length_;

        priority_t maxIndex;

        #ifndef NDEBUG
        priority_t maxP;
        #endif


        inline void* nodeAt(const priority_t& bucket, const priority_t& indexInBucket = 0) {
            LOGGER_ASSERT_EXCEPT(bucketLengths[bucket] > indexInBucket)
            void* node = bucketStarts[bucket];
            if (indexInBucket == 0) return node;
            for (bucketSize_t i = 1; i < indexInBucket; ++i) node = buckets.node_after(node);
            return buckets.node_after(node);
        }

        inline void removeAt(const priority_t& bucket, void* const& nodeAdr) {
            LOGGER_ASSERT_EXCEPT(bucketLengths[bucket] > 0)
            buckets.remove_node(nodeAdr);
            --length_;
            --bucketLengths[bucket];
        }

        inline void* inBucket(const T& value, const priority_t& priority) {
            if (bucketLengths[priority] == 0) return nullptr;
            auto node = bucketStarts[priority];
            if (value == buckets.get_atNode(node)) return node;
            for (bucketSize_t i = 1; i < bucketLengths[priority]; ++i) {
                if (node = buckets.node_after(node); value == buckets.get_atNode(node)) return node;
            }
            return nullptr;
        }


        /*inline void debugPrint(const char* message = "") {
            if constexpr (std::is_same_v<T, SG_Grid::Point>) {
                std::cout << message << std::endl;
                for (auto i = 0; i < buckets.length(); ++i) {
                    std::cout << buckets.get_fromFront(i).x() << "," << buckets.get_fromFront(i).y() << "[" << buckets.node_fromFront(i) << "]" << std::endl;
                }
                std::cout << "################" << std::endl;
                for (auto i = 0; i < bucketCount; ++i) {
                    auto b = bucketStarts[i];
                    SG_Grid::Point bv;
                    if (bucketLengths[i] > 0) {
                        bv = buckets.get_atNode(b);
                        std::cout << bv.x() << "," << bv.y() << "[" << b << "]" << " | " << i << std::endl;
                    }
                    for (auto j = 1; j < bucketLengths[i]; ++j) {
                        b = buckets.node_after(b);
                        bv = buckets.get_atNode(b);
                        std::cout << bv.x() << "," << bv.y() << "[" << b << "]" << " | " << i << "." << std::endl;
                    }
                }
                std::cout << "------------------" << std::endl;
            }
        }*/
    };
}

