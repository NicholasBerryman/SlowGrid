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
void test1(){
    SG_Allocator::Arena_ULL<50000,3> arena;
    SG_Grid::FullGrid<T,11,11> grid;
    SG_Pathfind::PriorityQueue::NoPriorityQueue<decltype(arena), decltype(grid), true, true, true, false> q(arena, grid, SG_Grid::Point(5,5), 4, 4);

    assert(q.trySwap());
    assert(q.trySwap());
    assert(q.trySwap());

    q.insert(SG_Grid::Point(6,6));
    q.insert(SG_Grid::Point(6,5));
    q.insert(SG_Grid::Point(7,7));

    assert(q.trySwap());


    assert(q.valueAt(0) == SG_Grid::Point(6,6));
    assert(q.valueAt(1)   == SG_Grid::Point(6,5));
    assert(q.valueAt(2)   == SG_Grid::Point(7,7));

    assert(!q.trySwap());

    assert(q.extractMin() == SG_Grid::Point(6,6));
    assert(!q.trySwap());
    assert(q.extractMin()   == SG_Grid::Point(6,5));
    assert(!q.trySwap());
    assert(q.extractMin()   == SG_Grid::Point(7,7));
    q.insert(SG_Grid::Point(3,6));
    assert(q.trySwap());
    assert(!q.trySwap());

    assert(q.extractMin()   == SG_Grid::Point(3,6));
    assert(q.trySwap());
    assert(q.trySwap());
}

template <typename T>
void test2(){
    SG_Allocator::Arena_ULL<50000,3> arena;
    SG_Grid::FullGrid<T,11,11> grid;
    SG_Pathfind::PriorityQueue::NoPriorityQueue<decltype(arena), decltype(grid), true, false, true, false> q(arena, grid, SG_Grid::Point(5,5), 4, 4);

    assert(!q.trySwap());
    assert(!q.trySwap());
    assert(!q.trySwap());

    q.insert(SG_Grid::Point(6,6));
    q.insert(SG_Grid::Point(6,5));
    q.insert(SG_Grid::Point(7,7));

    assert(!q.trySwap());


    assert(q.valueAt(0) == SG_Grid::Point(6,6));
    assert(q.valueAt(1)   == SG_Grid::Point(6,5));
    assert(q.valueAt(2)   == SG_Grid::Point(7,7));

    assert(!q.trySwap());

    assert(q.extractMin() == SG_Grid::Point(6,6));
    assert(!q.trySwap());
    assert(q.extractMin()   == SG_Grid::Point(6,5));
    assert(!q.trySwap());
    assert(q.extractMin()   == SG_Grid::Point(7,7));
    q.insert(SG_Grid::Point(3,6));
    assert(!q.trySwap());
    assert(!q.trySwap());

    assert(q.extractMin()   == SG_Grid::Point(3,6));
    assert(!q.trySwap());
    assert(!q.trySwap());
}

int main(int, char**) {
    test1<bool>();
    test1<uint64_t>();

    test2<bool>();
    test2<uint64_t>();
    return 0;
}

#undef assert

