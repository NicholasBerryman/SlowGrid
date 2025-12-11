//
// Created by nickberryman on 10/12/25.
//
export module SG_Pathfind:BucketQueue;
import :BasePriorityQueue;
import Logger;
import SG_Allocator;

export namespace SG_Pathfind {
    namespace PriorityQueue {

        template<typename T, typename priority_t>
        class BucketQueue : private BasePriorityQueue<T, priority_t>{
        public:
            BucketQueue(){};
            inline void insert(const T& value, priority_t priority ) {Logging::assert_except(0); }
            inline bool contains(const T& value ) {Logging::assert_except(0); return 0;}

            inline const T& findMin() {Logging::assert_except(0); return nullptr;}
            inline T extractMin() {Logging::assert_except(0); return nullptr;}
            inline void decreaseKey(const T& value, priority_t newPriority ) {Logging::assert_except(0); }

            inline const T& findMax() {Logging::assert_except(0); return nullptr;}
            inline T extractMax() {Logging::assert_except(0); return nullptr;}
            inline void increaseKey(const T& value, priority_t newPriority ) {Logging::assert_except(0); }

        private:
            //SG_Allocator::ULL<>

        };
    }
}
