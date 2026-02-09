//
// Created by nickberryman on 1/12/25.
//

#include <cstdint>
#include "Logger.h"
#include <iostream>
#define assert Logging::assert_except

import Logger;
import SG_Pathfind;
import SG_Allocator;

template <typename arena, typename pqueueValue>
void testBucketQueue(){
    arena myHeap;
    SG_Pathfind::PriorityQueue::BucketQueue<pqueueValue, SG_Allocator::arenaSize_t, SG_Allocator::arenaSize_t, arena> bq(myHeap, 10,1);
    assert(bq.encodePriority(2) == 1);
    bq.insert(10,bq.encodePriority(2));
    bq.findMin();
}


int main(int, char**) {
    testBucketQueue<SG_Allocator::Arena_ULL<1000,3>, char>();
    //testBucketQueue<SG_Allocator::Arena_ULL<640,3>, uint64_t>(); // A linked list is minimum 64 bytes with current settings -> 640 allows for a BucketQueue with length 10 to be initialised

    return 0;
}

#undef assert

