//
// Created by nickberryman on 14/11/25.
//
module;
#include "Logger.h"

export module LocalDataStructures:Queue;
import LocalDataStructureConfigs;
import Logger;
namespace LocalDataStructures {
    /**
     * Queue (FIFO) with local (non heap-deferred) storage
     * @tparam T Data type of elements
     * @tparam size Maximum number of elements
     */
    export template<typename T, localSize_t size> class Queue {
        static_assert(size > 0, "Size must be larger than 0");
    private:
        constexpr static localSize_t implSize = size+1;
        localSize_t tail = 0;
        localSize_t head = 0;
        T impl[implSize];

        inline static localSize_t wrapSubtract(localSize_t a);
    public:
        Queue() {}
        inline void push(const T& value);
        inline const T& pop();
        inline T peek(localSize_t skip = 0);
        inline T& peekRef(localSize_t skip = 0);

        inline T pop_back();

        void clear();

        [[nodiscard]] static inline localSize_t maxLength();
        [[nodiscard]] inline localSize_t length() const;

    };
}



// -- IMPLEMENTATION -- //
template<typename T, localSize_t size>
localSize_t LocalDataStructures::Queue<T, size>::wrapSubtract(const localSize_t a) {
    LOGGER_ASSERT_EXCEPT(a >= 1 || (1-a) <= size);
    if (a >= 1) return a-1;
    return implSize-1;
}


/**
 * Add item to end of Queue
 * @param value Item to add (copies)
 */
template<typename T, localSize_t size>
void LocalDataStructures::Queue<T, size>::push(const T& value) {
    LOGGER_ASSERT_EXCEPT(length() < size);
    impl[tail++] = value;
    if (tail >= implSize) tail %= implSize;
}


/**
 * Returns and removes item from queue (FIFO)
 * @return Item at end of queue
 */
template<typename T, localSize_t size>
const T& LocalDataStructures::Queue<T, size>::pop() {
    LOGGER_ASSERT_EXCEPT(length() > 0);
    auto out = head;
    head++;
    if (head >= implSize) head %= implSize;
    return impl[out];
}

/**
 * Return *copy of* item from front of queue with offset. Does not remove.
 * @param skip Offset from front of queue. Default is 0 (FIFO)
 * @return Item at offset from front of queue
 */
template<typename T, localSize_t size>
T LocalDataStructures::Queue<T, size>::peek(const localSize_t skip) {
    return peekRef(skip);
}

/**
 * Return *reference to* item from front of queue with offset. Does not remove.
 * @param skip Offset from front of queue. Default is 0 (FIFO)
 * @return Item at offset from front of queue
 */
template<typename T, localSize_t size>
T & LocalDataStructures::Queue<T, size>::peekRef(const localSize_t skip) {
    localSize_t i = (head + skip)%implSize;
    LOGGER_ASSERT_EXCEPT(i >= head || i < tail);
    return impl[i];
}

/**
 * Returns and removes item from queue (LIFO)
 * @return Item at end of queue
 */
template<typename T, localSize_t size>
T LocalDataStructures::Queue<T, size>::pop_back() {
    LOGGER_ASSERT_EXCEPT(length() > 0);
    tail = wrapSubtract(tail);
    return impl[tail];
}

/**
 * Sets queue to empty
 */
template<typename T, localSize_t size>
void LocalDataStructures::Queue<T, size>::clear() {
    head = 0;
    tail = 0;
}

/**
 *
 * @return Maximum number of elements supported by this queue
 */
template<typename T, localSize_t size>
localSize_t LocalDataStructures::Queue<T, size>::maxLength() {
    return size;
}

/**
 *
 * @return Number of elements in the queue
 */
template<typename T, localSize_t size>
localSize_t LocalDataStructures::Queue<T, size>::length() const {
    if (tail >= head) return tail - head;
    else return implSize-head+tail;
}
