//
// Created by nickberryman on 14/11/25.
//
module;
#include "Logger.h"

export module LocalDataStructures:Stack;
import LocalDataStructureConfigs;
import Logger;
namespace LocalDataStructures {
    /**
     * Stack (LIFO) with local (non heap-deferred) storage
     * @tparam T Data type of elements
     * @tparam size Maximum number of elements
     */
    export template<typename T, localSize_t size> class Stack {
        static_assert(size > 0, "Size must be larger than 0");
    private:
        localSize_t stackPointer = 0;
        T impl[size];

    public:
        Stack() {} // Leaving this as a default constructor seems to cause the impl array to be 0-initialised (slow) [Tested on g++15 for Linux)
            // Stack() = default -> BAD!!!! Causes an asm 'rep' loop to 0 initialise the array
            // No constructor -> BAD!!!! Same as above
            // Stack(){} -> GOOD! No array initialisation
        inline void push(const T& value);
        inline const T& pop();
        inline T peek(localSize_t back = 0);
        inline T& peekRef(localSize_t back = 0);

        void clear();

        [[nodiscard]] static inline localSize_t maxLength();
        [[nodiscard]] inline localSize_t length() const;

    };
}

/**
 * Add item to end of stack
 * @param value Item to add (copies)
 */
template<typename T, localSize_t size>
void LocalDataStructures::Stack<T, size>::push(const T& value) {
    LOGGER_ASSERT_EXCEPT(stackPointer < size);
    impl[stackPointer++] = value;
}


/**
 * Returns and removes item from stack (LIFO)
 * @return Item at end of stack
 */
template<typename T, localSize_t size>
const T& LocalDataStructures::Stack<T, size>::pop() {
    LOGGER_ASSERT_EXCEPT(stackPointer > 0);
    return impl[--stackPointer];
}

/**
 * Return *copy of* item from end of stack with offset. Does not remove.
 * @param back Offset from end of stack. Default is 0 (LIFO)
 * @return Item at offset from end of stack
 */
template<typename T, localSize_t size>
T LocalDataStructures::Stack<T, size>::peek(const localSize_t back) {
    return peekRef(back);
}

/**
 * Return *reference to* item from end of stack with offset. Does not remove.
 * @param back Offset from end of stack. Default is 0 (LIFO)
 * @return Item at offset from end of stack
 */
template<typename T, localSize_t size>
T &LocalDataStructures::Stack<T, size>::peekRef(const localSize_t back) {
    LOGGER_ASSERT_EXCEPT(stackPointer >= 1+back);
    return impl[stackPointer-1-back];
}

/**
 * Sets stack to empty
 */
template<typename T, localSize_t size>
void LocalDataStructures::Stack<T, size>::clear() {
    stackPointer = 0;
}

/**
 *
 * @return Number of elements in the stack
 */
template<typename T, localSize_t size>
localSize_t LocalDataStructures::Stack<T, size>::length() const {
    return stackPointer;
}

/**
 *
 * @return Maximum number of elements supported by this stack
 */
template<typename T, localSize_t size>
localSize_t LocalDataStructures::Stack<T, size>::maxLength() {
    return size;
}





