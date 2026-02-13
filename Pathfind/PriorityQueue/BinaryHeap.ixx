//
// Created by nickberryman on 10/12/25.
//

module;
#include <type_traits>
#include "Logger.h"
#include <algorithm>
#include <cstdint>

export module SG_Pathfind:BinaryHeap;
import :BasePriorityQueue;
import Logger;
import SG_Grid;
import SG_Allocator;

export namespace SG_Pathfind::PriorityQueue {
    template<typename T, typename priority_t, typename InsideArenaType, bool fullDecreaseKey = true, bool fifoOnTie = true , bool queensCase = true, uint8_t reserveDivisor = 4>
    class BinaryHeap : private BasePriorityQueue<T, priority_t>{
        public:
            BinaryHeap(InsideArenaType& arena, const priority_t& maxPriority_, const priority_t& minPriority_ = 0) requires (!queensCase):
                heap(arena, static_cast<SG_Allocator::arenaSize_t>(
                    2 * (maxPriority_ - minPriority_) * (maxPriority_ - minPriority_ - 1) + 1) / reserveDivisor // 2x(x-1)+1
                ){if constexpr (fifoOnTie) counter = 0;}

            BinaryHeap(InsideArenaType& arena, const priority_t& maxPriority_, const priority_t& minPriority_ = 0) requires (queensCase):
                heap(arena, static_cast<SG_Allocator::arenaSize_t>(
                    (2 * (maxPriority_ - minPriority_ - 1)) * (2 * (maxPriority_ - minPriority_ - 1)) / reserveDivisor // (2x-1)^2
                )){if constexpr (fifoOnTie) counter = 0;}

            static inline const priority_t& encodePriority(const priority_t& priority){ return priority; }
            static inline const priority_t& decodePriority(const priority_t& priority){ return priority; }

            [[nodiscard]] inline const SG_Allocator::arenaSize_t& length() const { return heap.length(); }


            inline void forceInsert(const T& value, const priority_t& priority) {
                if constexpr (!fifoOnTie) heap.construct_back(priority, value);
                else heap.construct_back(priority, value, counter);
                heapifyUp(length()-1);
                if constexpr (fifoOnTie) ++counter;
            }

            inline void insert(const T& value, const priority_t& priority) {
                for (auto i = 0; i < heap.length(); ++i) {
                    if (heap.get(i).value == value) {
                        if constexpr (fullDecreaseKey) {
                            if (priority <= heap.get(i).priority) decreaseKey(i, priority);
                            return;
                        }
                        if (heap.get(i).priority <= priority)
                            return;
                    }
                }
                forceInsert(value, priority);
            }

            inline const priority_t& findMin() const {
                LOGGER_ASSERT_EXCEPT(length() > 0);
                return heap.get(0).priority;
            }

            inline T extractMin() {
                LOGGER_ASSERT_EXCEPT(length() > 0)
                T root = heap.get(0).value;
                heap.set(0,heap.getFromBack(0));
                heap.pop_back();
                heapifyDown(0);
                return root;
            }

            inline void decreaseKey(priority_t i, priority_t newPriority) {
                LOGGER_ASSERT_EXCEPT(i > 0 && i < length() || newPriority < heap.get(i).priority);
                heap.get(i).priority = newPriority;
                heapifyUp(i);
            }


            // Should only really be used for debugging -> O(n) operation
            inline const T& valueAt(const priority_t& priority) {
                LOGGER_ASSERT_EXCEPT(length() > 0)
                priority_t count = 0;
                for (priority_t i = 0; i < length(); ++i) {
                    if (heap.get(i).priority == priority) ++count;
                    if (count > 0) return heap.get(i).value;
                }
                LOGGER_ASSERT_EXCEPT (count > 0); // Assert to ensure that the selected priority actually exists in the heap
                return heap.get(0).value; //Should never actually get here
            }

            inline const T& valueAtIndex(const priority_t& index) {
                return heap.get(index).value;
            }

    
        private:
            struct empty{};
            struct Node {
                priority_t priority;
                T value;
                [[no_unique_address]] std::conditional_t<fifoOnTie, priority_t, empty> tiebreak;
                Node(priority_t priority, T value) requires (!fifoOnTie): priority(priority), value(value){}
                Node(priority_t priority, T value, priority_t tiebreak) requires  (fifoOnTie): priority(priority), value(value), tiebreak(tiebreak){}
                bool operator<(const Node& other) const {
                    if constexpr (fifoOnTie) if (priority == other.priority) return tiebreak > other.tiebreak;
                    return priority < other.priority;
                }
            };
            SG_Allocator::ULL2<InsideArenaType, Node> heap;
            [[no_unique_address]] std::conditional_t<fifoOnTie, priority_t, empty> counter;


            static inline SG_Allocator::arenaSize_t parent(const SG_Allocator::arenaSize_t& index) { return (index - 1) / 2; }
            static inline SG_Allocator::arenaSize_t leftChild(const SG_Allocator::arenaSize_t& index) { return (2 * index + 1); }
            static inline SG_Allocator::arenaSize_t rightChild(const SG_Allocator::arenaSize_t& index) { return (2 * index + 2); }

            // Heapify up to maintain heap property
            inline void heapifyUp(const SG_Allocator::arenaSize_t& index_) {
                auto index = index_;
                while (index && heap.get(index) < heap.get(parent(index))) {
                    std::swap(heap.get(index), heap.get(parent(index)));
                    index = parent(index);
                }
            }

            // Heapify down to maintain heap property
            inline void heapifyDown(priority_t index) {
                SG_Allocator::arenaSize_t left = leftChild(index);
                SG_Allocator::arenaSize_t right = rightChild(index);
                SG_Allocator::arenaSize_t smallest = index;

                if (left  < heap.length() && heap.get(left)  < heap.get(smallest))
                    smallest = left;

                if (right < heap.length() && heap.get(right) < heap.get(smallest))
                    smallest = right;

                if (smallest != index) {
                    std::swap(heap.get(index), heap.get(smallest));
                    heapifyDown(smallest);
                }
            }
    };
}
