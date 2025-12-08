//
// Created by nickberryman on 14/11/25.
//
#include "Logger.h"
#include <iostream>
#define assert Logging::assert_except

using namespace std;

import Logger;
import LocalDataStructures;
void testOnStack(){
    LocalDataStructures::Queue<int, 3> queue;
    queue.push(10);
    queue.push(8);
    queue.push(7);

    // Test Peek
    assert(queue.peek() == 10);
    assert(queue.peek(1) == 8);
    assert(queue.length() == 3);

    // Test PeekRef
    assert(queue.peekRef() == 10);
    assert(queue.peekRef(1) == 8);
    assert(queue.length() == 3);

    // Test Pop
    assert(queue.pop() == 10);
    assert(queue.length() == 2);
    assert(queue.pop() == 8);
    assert(queue.length() == 1);
    assert(queue.pop() == 7);
    assert(queue.length() == 0);

    // Test size
    assert(queue.maxLength() == 3);

    // Test Clear
    for (int i = 0; i < 2; i++) queue.push(1);
    queue.clear();
    assert(queue.length() == 0);
    for (int i = 0; i < 3; i++) queue.push(i);
    assert(queue.peek() == 0);
    assert(queue.length() == 3);

    // Test bounds
    LOGGER_ASSERT_ERROR(queue.push(3);)
    queue.pop();
    queue.pop();
    queue.pop();
    LOGGER_ASSERT_ERROR(queue.pop();)

    // Test circularity
    queue.clear();
    for (int i = 0; i < 3; i++) queue.push(i);
    for (int i = 3; i < 100; i++) {
        assert(queue.peek() == i-3);
        assert(queue.peek(1) == i-2);
        assert(queue.peek(2) == i-1);
        queue.pop();
        queue.push(i);
    }
    assert(queue.pop() == 97);
    assert(queue.pop() == 98);
    assert(queue.pop() == 99);

    // Test FIFO
    queue.clear();
    assert(queue.length() == 0);
    queue.push(10);
    queue.push(8);
    queue.push(7);
    assert(queue.pop_back() == 7);
    assert(queue.pop_back() == 8);
    assert(queue.pop_back() == 10);
    LOGGER_ASSERT_ERROR(queue.pop_back();)
    LOGGER_ASSERT_ERROR(queue.pop();)
}

int main(int, char**) {
    testOnStack();
    return 0;
}

#undef assert

