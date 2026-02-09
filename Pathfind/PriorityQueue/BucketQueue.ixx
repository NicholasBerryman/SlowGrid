//
// Created by nickberryman on 10/12/25.
//
module;
#include <type_traits>
#include "Logger.h"

export module SG_Pathfind:BucketQueue;
import :BasePriorityQueue;
import Logger;
import SG_Grid;
import SG_Allocator;


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
        { for (priority_t i = 0; i < buckets.maxSize(); ++i) buckets.construct_back(arena); };

        inline priority_t encodePriority(const priority_t& priority){ return priority - minPriority; } //YOU NEED TO CALL THIS FIRST (EXTERNAL USE)

        inline void insert(const T& value, const priority_t& priority ) {
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

        inline const priority_t& findMin() {
            LOGGER_ASSERT_EXCEPT(length_ > 0)
            for (; minIndex < buckets.length(); ++minIndex) if (buckets.get(minIndex).length() > 0) return minIndex;
            return minIndex;
        }
        inline const T& valueAt(const priority_t& bucket, const priority_t& indexInBucket = 0) { return buckets.get(bucket).get_fromFront(indexInBucket); }
        inline void removeAt(const priority_t& bucket, void* const& nodeAdr) { buckets.get(bucket).remove_node(nodeAdr); }

        void* inBucket(const T& value, const priority_t& priority) {
            auto& bucket = buckets.get(priority);
            for (bucketSize_t i = 0; i < bucket.length(); ++i) if (void* node = bucket.node_fromFront(i); value == bucket.get_atNode(node)) return node;
            return nullptr;
        }
        inline void decreaseKey(void* const& nodeAdr, const priority_t& oldPriority, const priority_t& newPriority ) {
            forceInsert(buckets.get(oldPriority).get_atNode(nodeAdr), newPriority);
            if constexpr (fullDecreaseKey) removeAt(oldPriority, nodeAdr);
        }

        inline void forceInsert(const T& value, const priority_t& priority ) {
            LOGGER_ASSERT_EXCEPT(priority <= maxP)
            LOGGER_ASSERT_EXCEPT(priority >= minIndex)
            buckets.get(priority).construct_front(value);
            ++length_;
        }

    private:
        SG_Allocator::ULL2<InsideArenaType, SG_Allocator::LinkedList<InsideArenaType, T, priority_t, true, true, true>> buckets; //TODO try to make work with just a singly-linked list?

        priority_t minIndex;
        priority_t minPriority;
        priority_t length_;

        #ifndef NDEBUG
        priority_t maxP;
        #endif
    };
}

