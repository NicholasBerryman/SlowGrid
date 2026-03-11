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
void testQueen(){
    SG_Allocator::Arena_ULL<50000,3> arena;
    SG_Grid::FullGrid<T,11,11> grid;
    auto& BFS = SG_Pathfind::BFS::BFS_Point<decltype(arena),decltype(arena), decltype(grid), SG_Pathfind::HashMap::GridRangeHashMap<decltype(arena),SG_Grid::Point>, true>;
    auto& BFS2 = SG_Pathfind::BFS::BFS_Flowfield<decltype(arena),decltype(arena), decltype(grid), SG_Pathfind::HashMap::GridRangeHashMap<decltype(arena),SG_Grid::Point>, true>;
    auto& BFS3 = SG_Pathfind::BFS::BFS_Dmatrix<decltype(arena),decltype(arena), decltype(grid), SG_Pathfind::HashMap::GridRangeHashMap<decltype(arena),SG_Grid::Point>, true>;
    
    grid.fill_memset(0);
    assert(BFS(grid, arena, arena, {3,3}, {0,0}, 4).length() == 0);
    assert(!BFS2(grid, arena, arena, {3,3}, 4).contains({0,0}));
    assert(!BFS3(grid, arena, arena, {3,3}, 4).contains({0,0}));
    
    grid.fill(1);
    assert(BFS(grid, arena, arena, {3,3}, {0,0}, 4).length() == 1);
    assert(BFS2(grid, arena, arena, {3,3}, 4).get({0,0}) == SG_Grid::Point(-1,-1));
    assert(BFS3(grid, arena, arena, {3,3}, 4).get({0,0}) == 3);
    
    grid.set({2,2},0);
    assert(BFS(grid, arena, arena, {3,3}, {0,0}, 4).length() == 3);
    assert(BFS3(grid, arena, arena, {3,3}, 4).get({0,0}) == 4);
}

template <typename T>
void testRook(){
    SG_Allocator::Arena_ULL<50000,3> arena;
    SG_Grid::FullGrid<T,11,11> grid;
    auto& BFS = SG_Pathfind::BFS::BFS_Point<decltype(arena),decltype(arena), decltype(grid), SG_Pathfind::HashMap::GridRangeHashMap<decltype(arena),SG_Grid::Point>, false>;
    auto& BFS2 = SG_Pathfind::BFS::BFS_Flowfield<decltype(arena),decltype(arena), decltype(grid), SG_Pathfind::HashMap::GridRangeHashMap<decltype(arena),SG_Grid::Point>, false>;
    auto& BFS3 = SG_Pathfind::BFS::BFS_Dmatrix<decltype(arena),decltype(arena), decltype(grid), SG_Pathfind::HashMap::GridRangeHashMap<decltype(arena),SG_Grid::Point>, false>;
    
    grid.fill_memset(0);
    assert(BFS(grid, arena, arena, {3,3}, {0,0}, 8).length() == 0);
    assert(!BFS2(grid, arena, arena, {3,3}, 8).contains({0,0}));
    assert(!BFS3(grid, arena, arena, {3,3}, 8).contains({0,0}));
    
    grid.fill(1);
    assert(BFS(grid, arena, arena, {3,3}, {0,0}, 8).length() == 2);
    assert(BFS2(grid, arena, arena, {3,3}, 8).get({0,0}) == SG_Grid::Point(0,-1));
    assert(BFS3(grid, arena, arena, {3,3}, 8).get({0,0}) == 6);
    
    grid.set({2,3},0);
    assert(BFS(grid, arena, arena, {3,3}, {0,0}, 8).length() == 3);
    assert(BFS3(grid, arena, arena, {3,3}, 8).get({0,0}) == 6);
}

int main(int, char**) {
    testQueen<bool>(); //TODO make it template on different optimisation parameters etc
    testRook<bool>();
    return 0;
}

#undef assert

