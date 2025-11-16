//
// Created by nickberryman on 14/11/25.
//
module;

export module LocalDataStructures:Queue;
import LocalDataStructureConfigs;
import Logger;
namespace LocalDataStructures {
    export template<typename T, localSize_t size> class Queue {
    private:
        localSize_t tail = 0;
        localSize_t head = 0;
        localSize_t len = 0;
        T impl[size];

        inline static localSize_t wrapSubtract(localSize_t a);
    public:
        inline void push(T value);
        inline T pop();
        inline T peek(localSize_t skip = 0);
        inline T& peekRef(localSize_t skip = 0);

        inline T pop_back();

        void clear();

        [[nodiscard]] static inline localSize_t maxLength();
        [[nodiscard]] inline localSize_t length() const;

#ifndef NDEBUG
        Queue() { Logging::assert_except(size > 0); }
#endif
    };
}



// -- IMPLEMENTATION -- //
template<typename T, localSize_t size>
localSize_t LocalDataStructures::Queue<T, size>::wrapSubtract(const localSize_t a) {
    Logging::assert_except(a >= 1 || (1-a) <= size);
    if (a >= 1) return a-1;
    return size-1;
}

template<typename T, localSize_t size>
void LocalDataStructures::Queue<T, size>::push(const T value) {
    Logging::assert_except(len < size);
    impl[tail++] = value;
    tail %= size;
    len++;
}

template<typename T, localSize_t size>
T LocalDataStructures::Queue<T, size>::pop() {
    Logging::assert_except(len > 0);
    len--;
    T out = impl[head++];
    head %= size;
    return out;
}

template<typename T, localSize_t size>
T LocalDataStructures::Queue<T, size>::peek(const localSize_t skip) {
    return peekRef(skip);
}

template<typename T, localSize_t size>
T & LocalDataStructures::Queue<T, size>::peekRef(const localSize_t skip) {
    localSize_t i = (head + skip)%size;
    Logging::assert_except(i >= head || i < tail);
    return impl[i];
}

template<typename T, localSize_t size>
T LocalDataStructures::Queue<T, size>::pop_back() {
    Logging::assert_except(len > 0);
    len--;
    tail = wrapSubtract(tail);
    return impl[tail];
}

template<typename T, localSize_t size>
void LocalDataStructures::Queue<T, size>::clear() {
    head = 0;
    tail = 0;
    len  = 0;
}

template<typename T, localSize_t size>
localSize_t LocalDataStructures::Queue<T, size>::maxLength() {
    return size;
}

template<typename T, localSize_t size>
localSize_t LocalDataStructures::Queue<T, size>::length() const {
    return len;
}
