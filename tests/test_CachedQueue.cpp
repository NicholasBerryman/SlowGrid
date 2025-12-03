//
// Created by nickberryman on 1/12/25.
//

#include "Logger.h"
#define assert Logging::assert_except

import Logger;
import SG_Allocator;
import LocalDataStructureConfigs;

template <typename a, localSize_t cacheSize, localSize_t backlogSize>
void testCC(){
    a myHeap;
    SG_Allocator::CachedQueue<a, int,cacheSize, backlogSize> queue(myHeap);

    queue.push(10);
    queue.push(8);
    queue.push(7);

    // Test Peek
    assert(queue.peek() == 10);
    assert(queue.peek(1) == 8);
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

    // Test bounds & cache
    assert(queue.cacheLength() == 1);
    assert(queue.cacheMaxLength() == 1);
    LOGGER_ASSERT_ERROR(queue.push(3);)
    queue.popUnsafe();
    assert(queue.cacheLength() == 0);
    assert(queue.cacheMaxLength() == 1);
    LOGGER_ASSERT_ERROR(queue.popUnsafe();)
    queue.refresh();
    assert(queue.cacheLength() == 1);
    assert(queue.cacheMaxLength() == 1);
    queue.popUnsafe();
    assert(queue.cacheLength() == 0);
    assert(queue.cacheMaxLength() == 1);
    queue.pop();
    LOGGER_ASSERT_ERROR(queue.pop();)
    LOGGER_ASSERT_ERROR(queue.popUnsafe();)
}

int main(int, char**) {
    testCC<SG_Allocator::PseudoArena, 1, 2>();
    testCC<SG_Allocator::Arena_ULL<32,3>, 1, 2>();
    return 0;
}

#undef assert

