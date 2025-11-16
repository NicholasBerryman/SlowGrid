//
// Created by nickberryman on 14/11/25.
//
module;

export module LocalDataStructures:Stack;
import LocalDataStructureConfigs;
import Logger;
namespace LocalDataStructures {
    export template<typename T, localSize_t size> class Stack {
    private:
        localSize_t stackPointer = 0;
        T impl[size];

    public:
        inline void push(T value);
        inline T pop();
        inline T peek(localSize_t back = 0);
        inline T& peekRef(localSize_t back = 0);

        void clear();

        [[nodiscard]] static inline localSize_t maxLength();
        [[nodiscard]] inline localSize_t length() const;

#ifndef NDEBUG
        Stack() { Logging::assert_except(size > 0); }
#endif
    };
}




// -- IMPLEMENTATION -- //
template<typename T, localSize_t size>
void LocalDataStructures::Stack<T, size>::push(T value) {
    Logging::assert_except(stackPointer < size);
    impl[stackPointer++] = value;
}

template<typename T, localSize_t size>
T LocalDataStructures::Stack<T, size>::pop() {
    Logging::assert_except(stackPointer > 0);
    return impl[--stackPointer];
}

template<typename T, localSize_t size>
T LocalDataStructures::Stack<T, size>::peek(const localSize_t back) {
    return peekRef(back);
}

template<typename T, localSize_t size>
T &LocalDataStructures::Stack<T, size>::peekRef(const localSize_t back) {
    Logging::assert_except(stackPointer >= 1+back);
    return impl[stackPointer-1-back];
}

template<typename T, localSize_t size>
void LocalDataStructures::Stack<T, size>::clear() {
    stackPointer = 0;
}

template<typename T, localSize_t size>
localSize_t LocalDataStructures::Stack<T, size>::length() const {
    return stackPointer;
}

template<typename T, localSize_t size>
localSize_t LocalDataStructures::Stack<T, size>::maxLength() {
    return size;
}





