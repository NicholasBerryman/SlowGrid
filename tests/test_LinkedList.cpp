//
// Created by nickberryman on 1/12/25.
//

#include "Logger.h"
#include <iostream>
#define assert Logging::assert_except

import Logger;
import SG_Allocator;

template <typename a, typename align>
void testLL(){
    a myHeap;
    SG_Allocator::LinkedList<a, align> ll(myHeap);

    assert(ll.length() == 0);
    ll.push_back(16);
    assert(ll.length() == 1);
    assert(ll.get(0) == 16);
    assert(ll.getFromBack(0) == 16);


    ll.push_back(11);
    assert(ll.length() == 2);
    assert(ll.get(0) == 16);
    assert(ll.get(1) == 11);
    assert(ll.getFromBack(0) == 11);
    assert(ll.getFromBack(1) == 16);

    ll.push_front(0);
    assert(ll.length() == 3);
    assert(ll.get(0) == 0);
    assert(ll.get(1) == 16);
    assert(ll.get(2) == 11);
    assert(ll.getFromBack(0) == 11);
    assert(ll.getFromBack(1) == 16);
    assert(ll.getFromBack(2) == 0);

    ll.push_at(1, 8);
    //TODO figure out double-free after this call + implicit constructor

    //TODO test removes

    //TODO convert ULL to use LL backend (with some additional access methods maybe, to allow the ULL attorney class to function)
}

int main(int, char**) {
    testLL<SG_Allocator::PseudoArena, char>();
    testLL<SG_Allocator::PseudoArena, uint64_t>();

    testLL<SG_Allocator::Arena_ULL<1000,3>, char>();
    testLL<SG_Allocator::Arena_ULL<32,3>, uint64_t>();

    return 0;
}

#undef assert

