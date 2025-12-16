//
// Created by nickberryman on 10/12/25.
//
export module SG_Pathfind:BucketQueue;
import :BasePriorityQueue;
import Logger;
import SG_Grid;
import SG_Allocator;

export namespace SG_Pathfind {
    namespace PriorityQueue {
        template<typename T, typename priority_t, typename bucketSize_t, typename InsideArenaType>
        class BucketQueue : private BasePriorityQueue<T, priority_t>{
        public:
            BucketQueue(InsideArenaType& arena, const priority_t& maxPriority, bucketSize_t* bucketSizes(const priority_t& bucketIndex) ,const priority_t& minPriority = 0) :
                buckets(arena, maxPriority - minPriority)
            {
                for (priority_t i = 0; i <= maxPriority - minPriority; i++) buckets.construct_back(arena, bucketSizes(i));
            };
            inline void insert(const T& value, const priority_t& priority ) {
                //TODO expand (and if debug log) if priority above max
                buckets.get(priority).push_back(value);
            }
            inline bool contains(const T& value ) {
                for (priority_t i = 0; i < buckets.length(); i++) {
                    if (inBucket(value, i)) return true;
                }
                return false;
            }

            // TODO improve efficiency by adding 'min/max' variables that are tracked on insert (templatable to decide if min or max queue??)
            inline const T& findMin() {
                for (priority_t i = 0; i < buckets.length(); i++)
                    if (buckets.get(i).length() > 0) return i;
                T out; return out;
            }
            inline const T& extractMin() {
                for (priority_t i = 0; i < buckets.length(); i++)
                    if (buckets.get(i).length() > 0) return buckets.get(i).pop_back();
                T out; return out;
            }

            inline const T& findMax() {Logging::assert_except(0); return nullptr;}
            inline T extractMax() {Logging::assert_except(0); return nullptr;}

            //TODO look at my notes in 'Pathfinding.txt' for this
            inline void decreaseKey(const T& value, const priority_t& newPriority ) {Logging::assert_except(0); }
            inline void increaseKey(const T& value, const priority_t& newPriority ) {Logging::assert_except(0); }

        private:
            SG_Allocator::ULL2<InsideArenaType, SG_Allocator::ULL2<InsideArenaType, T>> buckets;

            bool inBucket(const T& value, const priority_t& priority) {
                auto& bucket = buckets.get(priority);
                for (bucketSize_t i = 0; i < bucket.length(); i++)
                    if (value == bucket.get(i)) return true;
                return false;
            }
        };
    }
}
