//
// Created by nickberryman on 14/11/25.
//
module;
#include "Logger.h"

export module LocalDataStructures:Dequeue;
import LocalDataStructureConfigs;
import Logger;
namespace LocalDataStructures {
    /**
     * Double ended queue with local (non-heap deferred) storage
     * @tparam T Data type of elements
     * @tparam size Maximum number of elements
     */
    export template<typename T, localSize_t size> class Dequeue {
        static_assert(size > 0, "Size must be larger than 0");
    private:
        localSize_t tail = 0;
        localSize_t head = 0;
        localSize_t len = 0;
        T impl[size];

        inline static localSize_t wrapSubtract(localSize_t a, localSize_t b);

    public:
        Dequeue() {}
        inline void push_front(const T& value);
        inline const T& pop_front();
        inline T peek_front(localSize_t skip = 0);
        inline T& peekRef_front(localSize_t skip = 0);

        inline void push_back(const T& value);
        inline const T& pop_back();
        inline T peek_back(localSize_t skip = 0);
        inline T& peekRef_back(localSize_t skip = 0);

        void clear();

        [[nodiscard]] static inline localSize_t maxLength();
        [[nodiscard]] inline localSize_t length() const;

    };
}



// -- IMPLEMENTATION -- //
template<typename T, localSize_t size>
localSize_t LocalDataStructures::Dequeue<T, size>::wrapSubtract(const localSize_t a, const localSize_t b) {
    LOGGER_ASSERT_EXCEPT(a >= b || (b-a) <= size);
    if (a >= b) return a-b;
    return size-(b - a);
}


/**
 * Add item to end of Queue
 * @param value Item to add (copies)
 */
template<typename T, localSize_t size>
void LocalDataStructures::Dequeue<T, size>::push_back(const T& value) {
    LOGGER_ASSERT_EXCEPT(len < size);
    if (len == 0) head = (wrapSubtract(head,1));
    impl[tail++] = value;
    if (tail >= size) tail %= size;
    len++;
}

/**
 * Add item to front of Queue
 * @param value Item to add (copies)
 */
template<typename T, localSize_t size>
void LocalDataStructures::Dequeue<T, size>::push_front(const T& value) {
    LOGGER_ASSERT_EXCEPT(len < size);
    if (len == 0) tail = (tail+1)%size;
    impl[head] = value;
    head = wrapSubtract(head,1);
    len++;
}


/**
 * Returns and removes item from queue (FIFO)
 * @return Item at end of queue
 */
template<typename T, localSize_t size>
const T& LocalDataStructures::Dequeue<T, size>::pop_front() {
    LOGGER_ASSERT_EXCEPT(len > 0);
    len--;
    head++;
    if (head >= size) head = head%size;
    return impl[head];
}

/**
 * Returns and removes item from queue (LIFO)
 * @return Item at end of queue
 */
template<typename T, localSize_t size>
const T& LocalDataStructures::Dequeue<T, size>::pop_back() {
    LOGGER_ASSERT_EXCEPT(len > 0);
    len--;
    tail = wrapSubtract(tail,1);
    return impl[tail];
}

/**
 * Return *copy of* item from front of queue with offset. Does not remove.
 * @param skip Offset from front of queue. Default is 0 (FIFO)
 * @return Item at offset from front of queue
 */
template<typename T, localSize_t size>
T LocalDataStructures::Dequeue<T, size>::peek_front(const localSize_t skip) {
    return peekRef_front(skip);
}

/**
 * Return *copy of* item from back of queue with offset. Does not remove.
 * @param skip Offset from back of queue. Default is 0 (LIFO)
 * @return Item at offset from back of queue
 */
template<typename T, localSize_t size>
T LocalDataStructures::Dequeue<T, size>::peek_back(const localSize_t skip) {
    return peekRef_back(skip);
}

/**
 * Return *reference to* item from front of queue with offset. Does not remove.
 * @param skip Offset from front of queue. Default is 0 (FIFO)
 * @return Item at offset from front of queue
 */
template<typename T, localSize_t size>
T & LocalDataStructures::Dequeue<T, size>::peekRef_front(const localSize_t skip) {
    localSize_t i = (head + 1 + skip)%size;
    LOGGER_ASSERT_EXCEPT(i >= (head+1)%size || i <= wrapSubtract(tail,1));
    return impl[i];
}

/**
 * Return *reference to* item from back of queue with offset. Does not remove.
 * @param skip Offset from back of queue. Default is 0 (LIFO)
 * @return Item at offset from back of queue
 */
template<typename T, localSize_t size>
T & LocalDataStructures::Dequeue<T, size>::peekRef_back(const localSize_t skip) {
    localSize_t i = (wrapSubtract(tail,1+skip));
    LOGGER_ASSERT_EXCEPT(i >= (head+1)%size || i <= wrapSubtract(tail,1));
    return impl[i];
}

/**
 * Sets queue to empty
 */
template<typename T, localSize_t size>
void LocalDataStructures::Dequeue<T, size>::clear() {
    head = 0;
    tail = 0;
    len  = 0;
}

/**
 *
 * @return Maximum number of elements supported by this queue
 */
template<typename T, localSize_t size>
localSize_t LocalDataStructures::Dequeue<T, size>::maxLength() {
    return size;
}

/**
 *
 * @return Number of elements in the queue
 */
template<typename T, localSize_t size>
localSize_t LocalDataStructures::Dequeue<T, size>::length() const {
    return len;
}
