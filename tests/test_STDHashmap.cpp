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
    SG_Grid::FullGrid<int,11,11> grid;
    SG_Pathfind::HashMap::STDHashMap<decltype(arena), T, false> hash(arena, grid, SG_Grid::Point(5,5), 3);

    assert(!hash.contains(SG_Grid::Point(6,6)));
    assert(!hash.contains(SG_Grid::Point(3,2)));
    assert(!hash.contains(SG_Grid::Point(2,7)));
    assert(!hash.contains(SG_Grid::Point(8,8)));
    assert(!hash.contains(SG_Grid::Point(4,3)));

    if constexpr (std::is_same_v<T, bool>) {
        hash.insert(SG_Grid::Point(3,7));
        hash.insert(SG_Grid::Point(8,8));
        hash.insert(SG_Grid::Point(2,2));
        assert(hash.contains(SG_Grid::Point(3,7)));
        assert(hash.contains(SG_Grid::Point(8,8)));
        assert(hash.contains(SG_Grid::Point(2,2)));
    } else {
        hash.insert(SG_Grid::Point(3,7), 5);
        hash.insert(SG_Grid::Point(8,8), 2);
        hash.insert(SG_Grid::Point(2,2), 3);
        assert(hash.get(SG_Grid::Point(3,7)) == 5);
        assert(hash.get(SG_Grid::Point(8,8)) == 2);
        assert(hash.get(SG_Grid::Point(2,2)) == 3);
    }

    assert(hash.contains(SG_Grid::Point(3,7)));
    assert(hash.contains(SG_Grid::Point(8,8)));
    assert(hash.contains(SG_Grid::Point(2,2)));


    hash.remove(SG_Grid::Point(2,2));
    assert(!hash.contains(SG_Grid::Point(2,2)));

    //LOGGER_ASSERT_ERROR( hash.contains(SG_Grid::Point(1,2)); )
}


void testBitfield(){
    SG_Allocator::Arena_ULL<50000,3> arena;
    SG_Grid::FullGrid<int,11,11> grid;
    SG_Pathfind::HashMap::STDHashMap<decltype(arena), bool, true> hash(arena, grid, SG_Grid::Point(5,5), 3);

    assert(!hash.contains(SG_Grid::Point(6,6)));
    assert(!hash.contains(SG_Grid::Point(3,2)));
    assert(!hash.contains(SG_Grid::Point(2,7)));
    assert(!hash.contains(SG_Grid::Point(8,8)));
    assert(!hash.contains(SG_Grid::Point(4,3)));

    hash.insert(SG_Grid::Point(3,7));
    hash.insert(SG_Grid::Point(8,8));
    hash.insert(SG_Grid::Point(2,2));
    assert(hash.contains(SG_Grid::Point(3,7)));
    assert(hash.contains(SG_Grid::Point(8,8)));
    assert(hash.contains(SG_Grid::Point(2,2)));

    hash.remove(SG_Grid::Point(2,2));
    assert(!hash.contains(SG_Grid::Point(2,2)));

    //LOGGER_ASSERT_ERROR( hash.contains(SG_Grid::Point(1,2)); )
}

template <typename T>
void testContainsPartitioned(){
    SG_Allocator::Arena_ULL<50000,3> arena;
    SG_Grid::FullGrid<int,11,11> grid;
    SG_Pathfind::HashMap::STDHashMap<decltype(arena), T, false, 1, 0> hash(arena, grid, SG_Grid::Point(5,5), 3);

    assert(!hash.contains(SG_Grid::Point(6,6)));
    assert(!hash.contains(SG_Grid::Point(3,2)));
    assert(!hash.contains(SG_Grid::Point(2,7)));
    assert(!hash.contains(SG_Grid::Point(8,8)));
    assert(!hash.contains(SG_Grid::Point(4,3)));

    if constexpr (std::is_same_v<T, bool>) {
        hash.insert(SG_Grid::Point(3,7));
        hash.insert(SG_Grid::Point(8,8));
        hash.insert(SG_Grid::Point(2,2));
    } else {
        hash.insert(SG_Grid::Point(3,7), 5);
        hash.insert(SG_Grid::Point(8,8), 2);
        hash.insert(SG_Grid::Point(2,2), 3);
        assert(hash.get(SG_Grid::Point(3,7)) == 5);
        assert(hash.get(SG_Grid::Point(8,8)) == 2);
        assert(hash.get(SG_Grid::Point(2,2)) == 3);
    }

    assert(hash.contains(SG_Grid::Point(3,7)));
    assert(hash.contains(SG_Grid::Point(8,8)));
    assert(hash.contains(SG_Grid::Point(2,2)));


    hash.remove(SG_Grid::Point(2,2));
    assert(!hash.contains(SG_Grid::Point(2,2)));

    //LOGGER_ASSERT_ERROR( hash.contains(SG_Grid::Point(1,2)); )
}

template <typename T>
void testGetPartitioned(){
    SG_Allocator::Arena_ULL<50000,3> arena;
    SG_Grid::FullGrid<int,11,11> grid;
    SG_Pathfind::HashMap::STDHashMap<decltype(arena), T, false, 0, 1> hash(arena, grid, SG_Grid::Point(5,5), 3);

    assert(!hash.contains(SG_Grid::Point(6,6)));
    assert(!hash.contains(SG_Grid::Point(3,2)));
    assert(!hash.contains(SG_Grid::Point(2,7)));
    assert(!hash.contains(SG_Grid::Point(8,8)));
    assert(!hash.contains(SG_Grid::Point(4,3)));

    if constexpr (std::is_same_v<T, bool>) {
        hash.insert(SG_Grid::Point(3,7));
        hash.insert(SG_Grid::Point(8,8));
        hash.insert(SG_Grid::Point(2,2));
    } else {
        hash.insert(SG_Grid::Point(3,7), 5);
        hash.insert(SG_Grid::Point(8,8), 2);
        hash.insert(SG_Grid::Point(2,2), 3);
        assert(hash.get(SG_Grid::Point(3,7)) == 5);
        assert(hash.get(SG_Grid::Point(8,8)) == 2);
        assert(hash.get(SG_Grid::Point(2,2)) == 3);
    }

    assert(hash.contains(SG_Grid::Point(3,7)));
    assert(hash.contains(SG_Grid::Point(8,8)));
    assert(hash.contains(SG_Grid::Point(2,2)));


    hash.remove(SG_Grid::Point(2,2));
    assert(!hash.contains(SG_Grid::Point(2,2)));

    //LOGGER_ASSERT_ERROR( hash.contains(SG_Grid::Point(1,2)); )
}

template <typename T>
void testGridEdge(){
    SG_Allocator::Arena_ULL<50000,3> arena;
    SG_Grid::FullGrid<int,11,11> grid;

    SG_Pathfind::HashMap::STDHashMap<decltype(arena), T, false> hash(arena, grid, SG_Grid::Point(11,11), 3);
    hash.insert({10,10}, 3);
    hash.insert({11,11}, 9);
    hash.insert({8,8},10);
    assert(hash.get({10,10}) == 3);
    assert(hash.get({11,11}) == 9);
    assert(hash.get({8,8}) == 10);
    //LOGGER_ASSERT_ERROR( hash.insert({12,12}, 18); )
    //LOGGER_ASSERT_ERROR( hash.insert({7,7}, 19); )

    SG_Pathfind::HashMap::STDHashMap<decltype(arena), T, false> hash2(arena, grid, SG_Grid::Point(1,1), 3);
    hash2.insert({3,3}, 3);
    hash2.insert({4,4}, 9);
    hash2.insert({0,0},10);
    assert(hash2.get({3,3}) == 3);
    assert(hash2.get({4,4}) == 9);
    assert(hash2.get({0,0}) == 10);
    //LOGGER_ASSERT_ERROR( hash2.insert({5,5}, 18); )

    SG_Pathfind::HashMap::STDHashMap<decltype(arena), T, false> hash3(arena, grid, SG_Grid::Point(11,5), 3);
    hash3.insert({10,5}, 3);
    hash3.insert({11,5}, 9);
    hash3.insert({8,5},10);
    assert(hash3.get({10,5}) == 3);
    assert(hash3.get({11,5}) == 9);
    assert(hash3.get({8,5}) == 10);
    //LOGGER_ASSERT_ERROR( hash3.insert({12,5}, 18); )
    //LOGGER_ASSERT_ERROR( hash3.insert({7,5}, 19); )
}



int main(int, char**) {
    testOnStack<bool>();
    testOnStack<char>();
    testOnStack<uint64_t>();
    testBitfield();

    testContainsPartitioned<bool>();
    testContainsPartitioned<char>();

    testGetPartitioned<bool>();
    testGetPartitioned<char>();

    testGridEdge<uint64_t>();
    testGridEdge<char>();

    return 0;
}

#undef assert

