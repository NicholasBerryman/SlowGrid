//
// Created by nickberryman on 10/12/25.
//
module;
#include <type_traits>
#include "Logger.h"
#include <cstring>

export module SG_Pathfind:BucketQueue;
import :BasePriorityQueue;
import Logger;
import SG_Grid;
import SG_Allocator;

//TODO add template option for FIFO or LIFO within buckets
//TODO double-check that A* with an admissible heuristic still satisfies the 'monotonic increasing' assumption that this uses here
//TODO add requires on priority_t being integer
export namespace SG_Pathfind::PriorityQueue {
    template<typename T, typename priority_t, typename bucketSize_t, typename InsideArenaType, bool fullDecreaseKey = true>
    class BucketQueue : private BasePriorityQueue<T, priority_t>{
    public:
        BucketQueue(InsideArenaType& arena, const priority_t& maxPriority_, const priority_t& minPriority_ = 0) :
            buckets(arena, maxPriority_-minPriority_+1)
            ,minIndex(0)
            ,minPriority(minPriority_)
            ,length_(0)
        #ifndef NDEBUG
            ,maxP(maxPriority_-minPriority_)
        #endif
        {
            auto* start = buckets.alloc_back(buckets.maxSize());
            std::memset(start, 0, buckets.maxSize()*sizeof(list_t)); //0-byte-init all buckets -> Requires linkedlist be 0-byte initialisable
            for (priority_t i = 1; i < buckets.maxSize(); ++i) buckets.get(i).init(arena);
            //for (auto i = 0; i < buckets.maxSize(); i++) buckets.construct_back(arena);
        };

        inline const priority_t& length(){ return length_; }

        inline priority_t encodePriority(const priority_t& priority){ return priority - minPriority; } //YOU NEED TO CALL THIS FIRST (EXTERNAL USE)
        inline priority_t decodePriority(const priority_t& priority){ return priority + minPriority; }

        inline void insert(const T& value, const priority_t& priority ) {
            #ifndef NDEBUG
                if (length_ > 0) findMin();
                LOGGER_ASSERT_EXCEPT (priority >= minIndex);
            #endif
            void* node = nullptr;
            priority_t bucket = minIndex;
            for (;bucket < buckets.length(); ++bucket) {
                node = inBucket(value, bucket);
                if (node != nullptr) break;
            }
            if (node != nullptr) {
                if (priority >= bucket) return;
                decreaseKey(node, bucket, priority);
                return;
            }
            forceInsert(value, priority);
        }

        inline const T& valueAt(const priority_t& bucket, const priority_t& indexInBucket = 0) {
            LOGGER_ASSERT_EXCEPT(length_ > 0)
            return buckets.get(bucket).get_fromFront(indexInBucket);
        }

        inline const priority_t& findMin() {
            LOGGER_ASSERT_EXCEPT(length_ > 0)
            for (; minIndex < buckets.length(); ++minIndex) if (buckets.get(minIndex).length() > 0) return minIndex;
            return minIndex;
        }

        inline T extractMin() {
            LOGGER_ASSERT_EXCEPT(length_ > 0)
            auto& out = buckets.get(findMin()).get_fromFront(0);
            buckets.get(findMin()).remove_front();
            --length_;
            return out;
        }

        void* inBucket(const T& value, const priority_t& priority) {
            auto& bucket = buckets.get(priority);
            for (bucketSize_t i = 0; i < bucket.length(); ++i) if (void* node = bucket.node_fromFront(i); value == bucket.get_atNode(node)) return node;
            return nullptr;
        }

        inline void removeAt(const priority_t& bucket, void* const& nodeAdr) {
            LOGGER_ASSERT_EXCEPT(length_ > 0)
            buckets.get(bucket).remove_node(nodeAdr);
            --length_;
        }

        inline void decreaseKey(void* const& nodeAdr, const priority_t& oldPriority, const priority_t& newPriority ) {
            forceInsert(buckets.get(oldPriority).get_atNode(nodeAdr), newPriority);
            if constexpr (fullDecreaseKey) removeAt(oldPriority, nodeAdr);
        }

        inline void* decreaseKeyAndReturn(void* const& nodeAdr, const priority_t& oldPriority, const priority_t& newPriority ) {
            auto out = forceInsert(buckets.get(oldPriority).get_atNode(nodeAdr), newPriority);
            if constexpr (fullDecreaseKey) removeAt(oldPriority, nodeAdr);
            return out;
        }

        inline void* forceInsert(const T& value, const priority_t& priority ) {
            LOGGER_ASSERT_EXCEPT(priority <= maxP)
            LOGGER_ASSERT_EXCEPT(priority >= minIndex)
            ++length_;
            return buckets.get(priority).construct_front(value);
        }

        inline void softClear() {
            for (priority_t i = 0; i < buckets.length(); ++i) while (buckets.get(i).length() > 0) buckets.get(i).remove_front();
        }

    private:
        typedef SG_Allocator::LinkedList<InsideArenaType, T, priority_t, true, true, true, true> list_t;
        SG_Allocator::ULL2<InsideArenaType, list_t> buckets; //TODO try to make work with just a singly-linked list?

        priority_t minIndex;
        priority_t minPriority;
        priority_t length_;

        #ifndef NDEBUG
        priority_t maxP;
        #endif
    };
}

