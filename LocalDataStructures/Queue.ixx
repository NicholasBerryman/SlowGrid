//
// Created by nickberryman on 14/11/25.
//
module;
#include "Logger.h"
#include <initializer_list>
#include <array>
#include <type_traits>

export module LocalDataStructures:Queue;
import LocalDataStructureConfigs;
import Logger;
namespace LocalDataStructures {


    export namespace Base {
        /**
         * Queue (FIFO) with local (non heap-deferred) storage
         * @tparam Collection_T Data type of internal collection
         * @tparam staticSize If using a statically sized collection, the size of that collection. 0 otherwise.
         */
        template<typename Collection_T, typename Length_T, Length_T staticSize = 0> class Queue {
        private:
            struct empty {};
            Length_T tail;
            Length_T head;
            Collection_T impl;
            [[no_unique_address]] std::conditional_t<staticSize != 0, empty, Length_T> internalSize;

            constexpr static Length_T implSiz() requires (staticSize > 0) {return staticSize+1;}
            const Length_T& implSiz() requires (staticSize == 0) {return internalSize+1;}

            inline static Length_T wrapSubtract(const Length_T& a) {
                if constexpr (staticSize > 0) { LOGGER_ASSERT_EXCEPT(a >= 1 || (1-a) <= staticSize); }
                if (a == 0) return implSiz()-1;
                return a-1;
            };


        public:
            template<typename... Args> explicit Queue(Args&&... args) requires (staticSize > 0): tail(0), head(0), impl(args...) {}
            template<typename... Args> explicit Queue(const Length_T& maxSize, Args&&... args) requires (staticSize == 0): tail(0), head(0), impl(args...), internalSize(maxSize) {}

            /**
             * Add item to end of Queue
             * @param value Item to add (copies)
             */
            void push(const auto& value) {
                if constexpr (staticSize > 0) { LOGGER_ASSERT_EXCEPT(length() < staticSize); }
                impl[tail++] = value;
                if (tail >= implSiz()) tail -= implSiz();
            }


            /**
             * Returns and removes item from queue (FIFO)
             * @return Item at end of queue
             */
            inline const auto& pop() {
                LOGGER_ASSERT_EXCEPT(length() > 0);
                auto out = head;
                ++head;
                if (head >= implSiz()) head -= implSiz();
                return impl[out];
            }

            /**
             * Return *copy of* item from front of queue with offset. Does not remove.
             * @param skip Offset from front of queue. Default is 0 (FIFO)
             * @return Item at offset from front of queue
             */
            inline auto peek(const Length_T& skip = 0) {
                return peekRef(skip);
            }

            /**
             * Return *reference to* item from front of queue with offset. Does not remove.
             * @param skip Offset from front of queue. Default is 0 (FIFO)
             * @return Item at offset from front of queue
             */
            inline auto& peekRef(const Length_T& skip = 0) {
                Length_T i = head + skip;
                if (i >= implSiz()) i %= implSiz();
                LOGGER_ASSERT_EXCEPT(i >= head || i < tail);
                return impl[i];
            }

            /**
             * Returns and removes item from queue (LIFO)
             * @return Item at end of queue
             */
            inline auto pop_back() {
                LOGGER_ASSERT_EXCEPT(length() > 0);
                tail = wrapSubtract(tail);
                return impl[tail];
            }

            /**
             * Sets queue to empty state (does NOT call destructors)
             */
            inline void clear() { head = 0; tail = 0; }

            /**
             *
             * @return Maximum number of elements supported by this queue
             */
            inline static Length_T maxLength() requires (staticSize > 0) { return staticSize; }

            /**
             *
             * @return Number of elements in the queue
             */
            [[nodiscard]] inline Length_T length() const { if (tail >= head) return tail - head; return implSiz()-head+tail; }
        };
    }

    /**
     * Queue (FIFO) with local (non heap-deferred) storage
     * @tparam T Data type of elements
     * @tparam size Maximum number of elements
     */
    export template<typename T, localSize_t size> class Queue : public Base::Queue<std::array<T, size+1>, localSize_t, size> {
        static_assert(size > 0, "Size must be larger than 0");
        typedef Base::Queue<std::array<T, size+1>, localSize_t, size> parent;

        public:
            inline Queue() : parent() {}
            inline Queue(std::initializer_list<T>&& initialValues) : parent() { for (auto elem : initialValues) parent::push(elem); }
    };

}

