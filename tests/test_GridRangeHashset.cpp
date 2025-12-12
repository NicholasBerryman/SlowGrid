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
    SG_Pathfind::HashSet::GridRangeHashset<> hash(arena, grid, SG_Grid::Point(5,5), 3);

    assert(hash.calcHash(SG_Grid::Point(4,4)) == SG_Grid::Point(2,2));
    assert(hash.calcHash(SG_Grid::Point(8,8)) == SG_Grid::Point(6,6));

    LOGGER_ASSERT_EXCEPT(!hash.contains(SG_Grid::Point(6,6)));
    LOGGER_ASSERT_EXCEPT(!hash.contains(SG_Grid::Point(3,2)));
    LOGGER_ASSERT_EXCEPT(!hash.contains(SG_Grid::Point(2,7)));
    LOGGER_ASSERT_EXCEPT(!hash.contains(SG_Grid::Point(8,8)));
    LOGGER_ASSERT_EXCEPT(!hash.contains(SG_Grid::Point(4,3)));

    hash.insert(SG_Grid::Point(3,7));
    hash.insert(SG_Grid::Point(8,8));
    hash.insert(SG_Grid::Point(2,2));
    LOGGER_ASSERT_EXCEPT(hash.contains(SG_Grid::Point(3,7)));
    LOGGER_ASSERT_EXCEPT(hash.contains(SG_Grid::Point(8,8)));
    LOGGER_ASSERT_EXCEPT(hash.contains(SG_Grid::Point(2,2)));

    hash.remove(SG_Grid::Point(2,2));
    LOGGER_ASSERT_EXCEPT(!hash.contains(SG_Grid::Point(2,2)));

    LOGGER_ASSERT_ERROR( hash.contains(SG_Grid::Point(1,2)); )
    LOGGER_ASSERT_ERROR( auto x = hash.calcHash(SG_Grid::Point(5,9)); )
}

int main(int, char**) {
    testOnStack<char>();
    testOnStack<uint64_t>();
    return 0;
}

#undef assert

