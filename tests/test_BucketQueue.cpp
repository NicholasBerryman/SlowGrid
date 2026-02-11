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
    assert(bq.valueAt(bq.encodePriority(2)) == 10);
    assert(bq.findMin() == bq.encodePriority(2));
    assert(bq.extractMin() == 10);
    LOGGER_ASSERT_ERROR( bq.valueAt(bq.encodePriority(2)) == 10; )

    bq.insert(11,bq.encodePriority(2));
    bq.insert(12,bq.encodePriority(5));
    bq.insert(13,bq.encodePriority(5));
    assert(bq.findMin() == bq.encodePriority(2));
    assert(bq.valueAt(bq.encodePriority(2)) == 11);
    assert(bq.valueAt(bq.encodePriority(5)) == 13);
    assert(bq.valueAt(bq.encodePriority(5),1) == 12);
    assert(bq.extractMin() == 11);
    assert(bq.findMin() == bq.encodePriority(5));
    assert(bq.extractMin() == 13);
    assert(bq.valueAt(bq.encodePriority(5)) == 12);

    bq.insert(2,bq.encodePriority(10));
    assert(bq.findMin() == bq.encodePriority(5));
    assert(bq.valueAt(bq.encodePriority(10)) == 2);
    LOGGER_ASSERT_ERROR(bq.valueAt(bq.encodePriority(10),1);)

    bq.insert(2,bq.encodePriority(9));
    assert(bq.findMin() == bq.encodePriority(5));
    assert(bq.valueAt(bq.encodePriority(9)) == 2);
    LOGGER_ASSERT_ERROR(bq.valueAt(bq.encodePriority(10));)

    assert(bq.extractMin() == 12);
    assert(bq.findMin() == bq.encodePriority(9));
    assert(bq.extractMin() == 2);
}


int main(int, char**) {
    testBucketQueue<SG_Allocator::Arena_ULL<2000,3>, char>();
    testBucketQueue<SG_Allocator::Arena_ULL<1040,3>, uint64_t>(); //TODO figure out if we can shrink this???

    return 0;
}

#undef assert

