//
// Created by nickberryman on 1/12/25.
//

#include "Logger.h"
#include <iostream>
#define assert Logging::assert_except

import Logger;
import SG_Allocator;

template <typename a, typename align>
void testULL(){
    SG_Allocator::PseudoArena myHeap;
    SG_Allocator::ULL<a, align> ull(myHeap, 3);

    // Test alloc
    (*ull.template alloc<char>()) = 'A';
    assert(*ull.template get<char>(0) == 'A');
    assert(ull.maxSize() == 3);
    assert(ull.length() == 1);
    (*ull.template alloc<char>()) = 'B';
    assert(*ull.template get<char>(0) == 'A');
    assert(*ull.template get<char>(1) == 'B');
    assert(ull.length() == 2);
    (*ull.template alloc<char>()) = 'C';
    
    // Test alloc beyond initialSize
    (*ull.template alloc<char>()) = 'D';
    void* holdNode = SG_Allocator::NodeExaminer<a, align>::currentTail(ull);
    SG_Allocator::arenaSize_t holdSize = ull.length();
    (*ull.template alloc<char>()) = 'E';
    assert(*ull.template get<char>(0) == 'A');
    assert(*ull.template get<char>(1) == 'B');
    assert(*ull.template get<char>(2) == 'C');
    assert(*ull.template get<char>(3) == 'D');
    assert(*ull.template get<char>(4) == 'E');
    assert(ull.length() == 5);

    // Test array alloc
    ull.template allocArray<char>(1)[0] = 'F';
    LOGGER_ASSERT_ERROR( ull.template allocArray<char>(50); )
    char* testArr = ull.template allocArray<char>(3);
    testArr[0] = '0';
    testArr[1] = '1';
    testArr[2] = '2';
    assert(*ull.template get<char>(5) == 'F');
    assert(ull.template get<char>(6)[0] == '0');
    assert(ull.template get<char>(6)[1] == '1');
    assert(ull.template get<char>(6)[2] == '2');

    // Test node rollback
    SG_Allocator::NodeExaminer<a, align>::setTail(ull, holdNode, holdSize);
    assert(ull.length() == 4);
    assert(ull.maxSize() == 9);
    (*ull.template alloc<char>()) = 'G';
    assert(ull.length() == 5);
    assert(ull.maxSize() == 9);
    ull.template alloc<char>(); //Leave unset -> Lets us check that we rediscover the existing value
    ull.template alloc<char>(); //Leave unset -> Lets us check that we rediscover the existing node
    assert(*ull.template get<char>(0) == 'A');
    assert(*ull.template get<char>(1) == 'B');
    assert(*ull.template get<char>(2) == 'C');
    assert(*ull.template get<char>(3) == 'D');
    assert(*ull.template get<char>(4) == 'G');
    assert(*ull.template get<char>(5) == 'F');
    assert(ull.template get<char>(6)[0] == '0');
    assert(ull.template get<char>(6)[1] == '1');
    assert(ull.template get<char>(6)[2] == '2');

    // Test soft clear
    ull.softClear();
    assert(ull.length() == 0);
    (*ull.template alloc<char>()) = 'a';
    assert(*ull.template get<char>(0) == 'a');
    assert(ull.maxSize() == 9);
    assert(ull.length() == 1);
    (*ull.template alloc<char>()) = 'b';
    (*ull.template alloc<char>()) = 'c';
    (*ull.template alloc<char>()) = 'd';
    LOGGER_ASSERT_ERROR( ull.template get<char>(4); )
    assert(ull.template get<char>(3)[sizeof(align)] == 'G');

    // Test shrink
    SG_Allocator::NodeExaminer<a, align>::shrink(ull);
    assert(ull.length() == 4);
    assert(ull.maxSize() == 6);
    (*ull.template alloc<char>()) = 'm';
    assert(*ull.template get<char>(4) == 'm');
    
    // Test hard clear
    ull.clear();
    assert(ull.length() == 0);
    (*ull.template alloc<char>()) = 'z';
    assert(*ull.template get<char>(0) == 'z');
    assert(ull.maxSize() == 3);
    assert(ull.length() == 1);

    // Test dealloc
    (*ull.template alloc<char>()) = 'x';
    (*ull.template alloc<char>()) = 'y';
    (*ull.template alloc<char>()) = 'w';
    (*ull.template alloc<char>()) = 'v';
    (*ull.template alloc<char>()) = 'u';
    assert(ull.length() == 6);
    assert(ull.maxSize() == 6);
    ull.dealloc(2);
    assert(ull.length() == 4);
    assert(ull.maxSize() == 6);
    ull.dealloc(1);
    assert(ull.length() == 3);
    assert(ull.maxSize() == 3);
    LOGGER_ASSERT_ERROR( ull.dealloc(4); )
}

int main(int, char**) {
    testULL<SG_Allocator::PseudoArena, char>();
    testULL<SG_Allocator::PseudoArena, uint64_t>();
    return 0;
}

#undef assert

