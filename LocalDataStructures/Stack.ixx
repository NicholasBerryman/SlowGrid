//
// Created by nickberryman on 14/11/25.
//
module;
#include "Logger.h"
#include <initializer_list>
#include <array>

export module LocalDataStructures:Stack;
import LocalDataStructureConfigs;
import Logger;
namespace LocalDataStructures {

    export namespace Base {

        /**
         * Stack (LIFO) with selectable storage (requires [] operators)
         * @tparam Collection_T Data type of internal collection
         * @tparam staticSize If using a statically sized collection, the size of that collection. 0 otherwise.
         */
        template<typename Collection_T, typename Length_T, Length_T staticSize = 0> class Stack {
        private:
            Length_T stackPointer;
            Collection_T impl;

        public:
            template<typename... Args> explicit Stack(Args&&... args) requires (staticSize > 0) : stackPointer(0), impl(args...) {}

            /**
             * Add item to end of stack
             * @param value Item to add (copies)
             */
            inline void push(const auto& value) { if constexpr (staticSize > 0) {LOGGER_ASSERT_EXCEPT(stackPointer < staticSize);} impl[stackPointer++] = value; }

            /**
             * Returns and removes item from stack (LIFO)
             * @return Item at end of stack
             */
            inline const auto& pop() {LOGGER_ASSERT_EXCEPT(stackPointer > 0); return impl[--stackPointer];}

            /**
             * Return *reference to* item from end of stack with offset. Does not remove.
             * @param back Offset from end of stack. Default is 0 (LIFO)
             * @return Item at offset from end of stack
             */
            inline auto& peekRef(const Length_T& back = 0) {LOGGER_ASSERT_EXCEPT(stackPointer >= 1+back); return impl[stackPointer-1-back];}

            /**
             * Return *copy of* item from end of stack with offset. Does not remove.
             * @param back Offset from end of stack. Default is 0 (LIFO)
             * @return Item at offset from end of stack
             */
            inline auto peek(const Length_T& back = 0) {return peekRef(back); }//TODO make a const correct version as well

            /**
             * Sets stack to empty state (does NOT call destructors)
             */
            void clear() { stackPointer = 0; }

            [[nodiscard]] static inline Length_T maxLength() requires (staticSize > 0) {return staticSize; }
            [[nodiscard]] inline const Length_T& length() const {return stackPointer; };
        };
    }

    /**
     * Stack (LIFO) with local (non heap-deferred) storage
     * @tparam T Data type of elements
     * @tparam size Maximum number of elements
     */
    export template<typename T, localSize_t size> class Stack: public Base::Stack<std::array<T, size>, localSize_t, size> {
        static_assert(size > 0, "Size must be larger than 0");
        typedef Base::Stack<std::array<T, size>, localSize_t, size> parent;

        public:
            inline Stack() : parent() {}
            inline Stack(std::initializer_list<T>&& initialValues) : parent() { for (auto elem : initialValues) parent::push(elem); }

    };
}





