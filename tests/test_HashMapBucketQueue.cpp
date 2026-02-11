//
// Created by nickberryman on 9/12/25.
//
#include <cstdint>
#include <iostream>
#include "Logger.h"
#define assert Logging::assert_except

import Logger;
import SG_Allocator;
import SG_Grid;
import SG_Pathfind;


template <typename T>
void testOnStack(){
    SG_Allocator::Arena_ULL<50000,3> arena;
    SG_Grid::FullGrid<T,11,11> grid;
    SG_Pathfind::PriorityQueue::HashMapBucketQueue<decltype(arena), decltype(grid)> q(arena, grid, SG_Grid::Point(5,5), 4, 4);

    q.insert(SG_Grid::Point(6,6),1);
    q.insert(SG_Grid::Point(6,5),1);
    q.insert(SG_Grid::Point(7,7),3);
    
    assert(q.valueAt(1,1) == SG_Grid::Point(6,6));
    assert(q.valueAt(1)   == SG_Grid::Point(6,5));
    assert(q.valueAt(3)   == SG_Grid::Point(7,7));
    assert(q.length() == 3);
    LOGGER_ASSERT_ERROR( q.valueAt(3,1); )
    LOGGER_ASSERT_ERROR( q.valueAt(2); )
    assert(q.findMin() == 1);
    
    q.insert({7,7}, 4); //Wow this makes Points a lot more readable. I think it's a c++11 feature?
    assert(q.valueAt(1,1) == SG_Grid::Point(6,6));
    assert(q.valueAt(1)   == SG_Grid::Point(6,5));
    assert(q.valueAt(3)   == SG_Grid::Point(7,7));
    assert(q.length() == 3);
    LOGGER_ASSERT_ERROR( q.valueAt(3,1); )
    LOGGER_ASSERT_ERROR( q.valueAt(2); )
    
    q.insert({7,7}, 3);
    assert(q.valueAt(1,1) == SG_Grid::Point(6,6));
    assert(q.valueAt(1)   == SG_Grid::Point(6,5));
    assert(q.valueAt(3)   == SG_Grid::Point(7,7));
    assert(q.length() == 3);
    LOGGER_ASSERT_ERROR( q.valueAt(3,1); )
    LOGGER_ASSERT_ERROR( q.valueAt(2); )
    
    q.insert({7,7}, 2);
    assert(q.valueAt(1,1) == SG_Grid::Point(6,6));
    assert(q.valueAt(1)   == SG_Grid::Point(6,5));
    assert(q.valueAt(2)   == SG_Grid::Point(7,7));
    assert(q.length() == 3);
    LOGGER_ASSERT_ERROR( q.valueAt(2,1); )
    LOGGER_ASSERT_ERROR( q.valueAt(3); )
    
    assert(q.extractMin() == SG_Grid::Point(6,5));
    assert(q.extractMin() == SG_Grid::Point(6,6));
    assert(q.extractMin() == SG_Grid::Point(7,7));
    assert(q.length() == 0);
    
    LOGGER_ASSERT_ERROR( q.extractMin(); )
}

int main(int, char**) {
    testOnStack<bool>();
    testOnStack<uint64_t>();
    return 0;
}

#undef assert

