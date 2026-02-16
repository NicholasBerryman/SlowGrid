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

//TODO add template option for how to handle ties
//TODO fix???
export namespace SG_Pathfind::PriorityQueue {
    template<typename T, typename priority_t, typename InsideArenaType, bool fullDecreaseKey = true, uint8_t sizeMult = 10>
    class BinaryHeap : private BasePriorityQueue<T, priority_t>{
        public:
            BinaryHeap(InsideArenaType& arena, const priority_t& maxPriority_, const priority_t& minPriority_ = 0):
                heap(arena, (SG_Allocator::arenaSize_t)((maxPriority_-minPriority_) * sizeMult)),
                _length(0) {}
            
            inline SG_Allocator::arenaSize_t length() const { return _length; }

            inline T getMin() const {
                LOGGER_ASSERT_EXCEPT(length() > 0);
                return heap.get(0);
            }

            inline void insert(const T& value, const priority_t& priority) {
                heap.push_back({priority, value});
                SG_Allocator::arenaSize_t index = length() - 1;
                heapifyUp(index);
                --_length;
            }

            inline T extractMin() {
                LOGGER_ASSERT_EXCEPT(length() > 0)
                T root = heap.get(0);
                heap.set(0,heap.getFromBack(0));
                heap.pop_back();
                heapifyDown(0);
                return root;
            }

            // Delete a specific node
            inline void deleteNode(T key) {
                int index = -1;
                // Find the index of the node to delete
                for (int i = 0; i < length(); ++i) {
                    if (heap.get(i) == key) {
                        index = i;
                        break;
                    }
                }
                LOGGER_ASSERT_EXCEPT(index != -1) //TODO improve this by using a 'contains' method instead, and correctly typing 'index' as arenaSize_t
                // Replace the node with the last element
                heap.get(index) = heap.getFromBack(0);
                heap.pop_back();
                // Restore heap property
                heapifyUp(index);
                heapifyDown(index);
            }

            // Decrease key function
            inline void decreaseKey(int i, T newValue) {
                LOGGER_ASSERT_EXCEPT(i < 0 || i >= length() || newValue > heap.get(i));
                heap.get(i) = newValue;
                heapifyUp(i);
            }
    
        private:
            struct Node {priority_t priority; T value;};
            SG_Allocator::ULL2<InsideArenaType, int> heap;
            SG_Allocator::arenaSize_t _length;
            
            inline SG_Allocator::arenaSize_t parent(const SG_Allocator::arenaSize_t& index) const { return (index - 1) / 2; }

            // Get the left child index
            inline SG_Allocator::arenaSize_t leftChild(const SG_Allocator::arenaSize_t& index) const { return (2 * index + 1); }

            // Get the right child index
            inline SG_Allocator::arenaSize_t rightChild(const SG_Allocator::arenaSize_t& index) const { return (2 * index + 2); }

            // Heapify up to maintain heap property
            inline void heapifyUp(const SG_Allocator::arenaSize_t& index) {
                while (index && heap.get(index) < heap.get(parent(index))) {
                    std::swap(heap.get(index), heap.get(parent(index)));
                    index = parent(index);
                }
            }

            // Heapify down to maintain heap property
            inline void heapifyDown(int index) {
                SG_Allocator::arenaSize_t left = leftChild(index);
                SG_Allocator::arenaSize_t right = rightChild(index);
                SG_Allocator::arenaSize_t smallest = index;

                if (left < heap.length()
                    && heap.get(left) < heap.get(smallest))
                    smallest = left;

                if (right < heap.length()
                    && heap.get(right) < heap.get(smallest))
                    smallest = right;

                if (smallest != index) {
                    std::swap(heap.get(index), heap.get(smallest));
                    heapifyDown(smallest);
                }
            }
    };
}
