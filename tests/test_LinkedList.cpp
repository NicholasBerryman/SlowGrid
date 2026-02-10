//
// Created by nickberryman on 1/12/25.
//

#include "Logger.h"
#include <iostream>
#define assert Logging::assert_except

import Logger;
import SG_Allocator;

template <typename a, typename align, bool recycle = true>
void testLLDoubleLinked(){
    a myHeap;
    SG_Allocator::LinkedList2<a, align, SG_Allocator::arenaSize_t, true, true, recycle> ll(myHeap);

    assert(ll.length() == 0);
    ll.construct_back(16);
    assert(ll.length() == 1);
    assert(ll.get_fromFront(0) == 16);
    assert(ll.get_fromBack(0) == 16);

    ll.construct_back(11);
    assert(ll.length() == 2);
    assert(ll.get_fromFront(0) == 16);
    assert(ll.get_fromFront(1) == 11);
    assert(ll.get_fromBack(0) == 11);
    assert(ll.get_fromBack(1) == 16);

    ll.construct_front(0);
    assert(ll.length() == 3);
    assert(ll.get_fromFront(0) == 0);
    assert(ll.get_fromFront(1) == 16);
    assert(ll.get_fromFront(2) == 11);
    assert(ll.get_fromBack(0) == 11);
    assert(ll.get_fromBack(1) == 16);
    assert(ll.get_fromBack(2) == 0);

    ll.construct_fromFront(1, 8);
    assert(ll.length() == 4);
    assert(ll.get_fromFront(0) == 0);
    assert(ll.get_fromFront(1) == 8);
    assert(ll.get_fromFront(2) == 16);
    assert(ll.get_fromFront(3) == 11);
    assert(ll.get_fromBack(0) == 11);
    assert(ll.get_fromBack(1) == 16);
    assert(ll.get_fromBack(2) == 8);
    assert(ll.get_fromBack(3) == 0);

	ll.remove_back();
    assert(ll.length() == 3);
    assert(ll.get_fromFront(0) == 0);
    assert(ll.get_fromFront(1) == 8);
    assert(ll.get_fromFront(2) == 16);
    assert(ll.get_fromBack(0) == 16);
    assert(ll.get_fromBack(1) == 8);
    assert(ll.get_fromBack(2) == 0);
	
	ll.remove_front();
    assert(ll.length() == 2);
    assert(ll.get_fromFront(0) == 8);
    assert(ll.get_fromFront(1) == 16);
    assert(ll.get_fromBack(0) == 16);
    assert(ll.get_fromBack(1) == 8);
	
	ll.construct_fromFront(1, 3);
    assert(ll.length() == 3);
    assert(ll.get_fromFront(0) == 8);
    assert(ll.get_fromFront(1) == 3);
    assert(ll.get_fromFront(2) == 16);
    assert(ll.get_fromBack(0) == 16);
    assert(ll.get_fromBack(1) == 3);
    assert(ll.get_fromBack(2) == 8);

	ll.remove_fromFront(1);
    assert(ll.length() == 2);
    assert(ll.get_fromFront(0) == 8);
    assert(ll.get_fromFront(1) == 16);
    assert(ll.get_fromBack(0) == 16);
    assert(ll.get_fromBack(1) == 8);
	
	ll.remove_back();
    assert(ll.length() == 1);
    assert(ll.get_fromFront(0) == 8);
    assert(ll.get_fromBack(0) == 8);
	
	ll.remove_front();
	assert(ll.length() == 0);

	// TODO maybe add faster linked-list template option with less runtime checking (trusting in compile-time correct use)???
}

template <typename a, typename align, bool recycle = true>
void testLLForwardLink(){
    a myHeap;
    SG_Allocator::LinkedList2<a, align, SG_Allocator::arenaSize_t, true, false, recycle> llf(myHeap);

	assert(llf.length() == 0);
	llf.construct_front(16);
	assert(llf.length() == 1);
	assert(llf.get_fromFront(0) == 16);

	llf.construct_front(17);
	assert(llf.length() == 2);
	assert(llf.get_fromFront(0) == 17);
	assert(llf.get_fromFront(1) == 16);

	llf.construct_afterNode(llf.node_fromFront(0), 21);
	assert(llf.length() == 3);
	assert(llf.get_fromFront(0) == 17);
	assert(llf.get_fromFront(1) == 21);
	assert(llf.get_fromFront(2) == 16);

	llf.remove_front();
	assert(llf.length() == 2);
	assert(llf.get_fromFront(0) == 21);
	assert(llf.get_fromFront(1) == 16);

	llf.remove_fromFront(1);
	assert(llf.length() == 1);
	assert(llf.get_fromFront(0) == 21);
}



int main(int, char**) {
    testLLDoubleLinked<SG_Allocator::PseudoArena, char>();
    testLLDoubleLinked<SG_Allocator::PseudoArena, uint64_t>();
    testLLDoubleLinked<SG_Allocator::Arena_ULL<1000,3>, char>();
    testLLDoubleLinked<SG_Allocator::Arena_ULL<32,3>, uint64_t>();

	testLLForwardLink<SG_Allocator::PseudoArena, char>();
	testLLForwardLink<SG_Allocator::PseudoArena, uint64_t>();
	testLLForwardLink<SG_Allocator::Arena_ULL<1000,3>, char>();
	testLLForwardLink<SG_Allocator::Arena_ULL<32,3>, uint64_t>();


	testLLDoubleLinked<SG_Allocator::PseudoArena, char, false>();
	testLLDoubleLinked<SG_Allocator::PseudoArena, uint64_t, false>();
	testLLDoubleLinked<SG_Allocator::Arena_ULL<1000,3>, char, false>();
	testLLDoubleLinked<SG_Allocator::Arena_ULL<32,3>, uint64_t, false>();

	testLLForwardLink<SG_Allocator::PseudoArena, char, false>();
	testLLForwardLink<SG_Allocator::PseudoArena, uint64_t, false>();
	testLLForwardLink<SG_Allocator::Arena_ULL<1000,3>, char, false>();
	testLLForwardLink<SG_Allocator::Arena_ULL<32,3>, uint64_t, false>();

    return 0;
}

#undef assert

