//
// Created by nickberryman on 14/11/25.
//
#include "Logger.h"
#define assert Logging::assert_except

#include <iostream>

import Logger;
import LocalDataStructures;
void testOnStack(){
    LocalDataStructures::Dequeue<int, 3> queue;
    queue.push_back(10);
    queue.push_back(8);
    queue.push_back(7);

    // Test Peek
    queue.peek_back();
    assert(queue.peek_front() == 10);
    assert(queue.peek_front(1) == 8);
    assert(queue.length() == 3);

    // Test PeekRef
    assert(queue.peekRef_front() == 10);
    assert(queue.peekRef_front(1) == 8);
    assert(queue.length() == 3);

    // Test Pop
    assert(queue.pop_front() == 10);
    assert(queue.length() == 2);
    assert(queue.pop_front() == 8);
    assert(queue.length() == 1);
    assert(queue.pop_front() == 7);
    assert(queue.length() == 0);

    // Test size
    assert(queue.maxLength() == 3);

    // Test Clear
    for (int i = 0; i < 2; i++) queue.push_back(1);
    queue.clear();
    assert(queue.length() == 0);
    for (int i = 0; i < 3; i++) queue.push_back(i);
    assert(queue.peek_front() == 0);
    assert(queue.length() == 3);

    // Test bounds
    LOGGER_ASSERT_ERROR(queue.push_back(3);)
    queue.pop_front();
    queue.pop_front();
    queue.pop_front();
    LOGGER_ASSERT_ERROR(queue.pop_front();)

    // Test circularity
    queue.clear();
    for (int i = 0; i < 3; i++) queue.push_back(i);
    for (int i = 3; i < 100; i++) {
        assert(queue.peek_front() == i-3);
        assert(queue.peek_front(1) == i-2);
        assert(queue.peek_front(2) == i-1);
        queue.pop_front();
        queue.push_back(i);
    }
    assert(queue.pop_front() == 97);
    assert(queue.pop_front() == 98);
    assert(queue.pop_front() == 99);

    // Test circularity (reverse)
    queue.clear();
    for (int i = 0; i < 3; i++) queue.push_front(i);
    for (int i = 3; i < 100; i++) {
        assert(queue.peek_back() == i-3);
        assert(queue.peek_back(1) == i-2);
        assert(queue.peek_back(2) == i-1);
        queue.pop_back();
        queue.push_front(i);
    }
    assert(queue.pop_back() == 97);
    assert(queue.pop_back() == 98);
    assert(queue.pop_back() == 99);

    // Test FIFO
    queue.clear();
    assert(queue.length() == 0);
    queue.push_back(10);
    queue.push_back(8);
    queue.push_back(7);
    assert(queue.pop_back() == 7);
    assert(queue.pop_back() == 8);
    assert(queue.pop_back() == 10);
    LOGGER_ASSERT_ERROR(queue.pop_back();)
    LOGGER_ASSERT_ERROR(queue.pop_front();)

    // Test FIFO (reverse)
    queue.clear();
    assert(queue.length() == 0);
    queue.push_front(10);
    queue.push_front(8);
    queue.push_front(7);
    assert(queue.pop_front() == 7);
    assert(queue.pop_front() == 8);
    assert(queue.pop_front() == 10);
    LOGGER_ASSERT_ERROR(queue.pop_back();)
    LOGGER_ASSERT_ERROR(queue.pop_front();)
}

int main(int, char**) {
    testOnStack();
    return 0;
}

#undef assert

