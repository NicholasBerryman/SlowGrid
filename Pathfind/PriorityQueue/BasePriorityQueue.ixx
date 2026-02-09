//
// Created by nickberryman on 10/12/25.
//
export module SG_Pathfind:BasePriorityQueue;
import Logger;

export namespace SG_Pathfind {
    namespace PriorityQueue {
        /**
         * @brief Interface for priority queues. Should only be inherited from, never used directly
         *
         */
        template<typename T, typename priority_t>
        class BasePriorityQueue {
        public:

            inline priority_t encodePriority(const priority_t& priority) {Logging::assert_except(0); return 0;}
            inline void insert(const T& value, const priority_t& priority ) {Logging::assert_except(0); }
            inline bool contains(const T& value ) {Logging::assert_except(0); return 0;}

            inline priority_t findMin() {Logging::assert_except(0); return 0;}
            inline T extractMin() {Logging::assert_except(0); return nullptr;}
            inline void decreaseKey(const T& value, const priority_t& newPriority ) {Logging::assert_except(0); }

        protected:
            BasePriorityQueue() = default;
        };
    }
}
