//
// Created by nickberryman on 14/11/25.
//
#include "Logger.h"
#define assert Logging::assert_except

import Logger;
import LocalDataStructures;
void testOnStack(){
    LocalDataStructures::Stack<int, 3> stack;
    stack.push(10);
    stack.push(8);
    stack.push(7);

    // Test Peek
    assert(stack.peek() == 7);
    assert(stack.peek(1) == 8);
    assert(stack.length() == 3);

    // Test PeekRef
    assert(stack.peekRef() == 7);
    assert(stack.peekRef(1) == 8);
    assert(stack.length() == 3);

    // Test Pop
    assert(stack.pop() == 7);
    assert(stack.length() == 2);
    assert(stack.pop() == 8);
    assert(stack.length() == 1);
    assert(stack.pop() == 10);
    assert(stack.length() == 0);

    // Test size
    assert(stack.maxLength() == 3);

    // Test Clear
    for (int i = 0; i < 2; i++) stack.push(1);
    stack.clear();
    assert(stack.length() == 0);

    // Test bounds
    LOGGER_ASSERT_ERROR(for (int i = 0; i < 4; i++) stack.push(1);) //Should error on adding a 4th element
    stack.clear();
    LOGGER_ASSERT_ERROR(stack.pop();)
}

int main(int, char**) {
    testOnStack();
    return 0;
}

#undef assert

