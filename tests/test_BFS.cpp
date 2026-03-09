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

#define BFS1_1(STD) SG_Pathfind::BFS::BFS_Point<decltype(arena),decltype(arena), decltype(grid), true, 256, STD>
#define BFS2_1(STD) SG_Pathfind::BFS::BFS_Flowfield<decltype(arena),decltype(arena), decltype(grid), true, STD>
#define BFS3_1(STD) SG_Pathfind::BFS::BFS_Dmatrix<decltype(arena),decltype(arena), decltype(grid), true, STD>

#define BFS1_2(STD) SG_Pathfind::BFS::BFS_Point<decltype(arena),decltype(arena), decltype(grid), false, 256, STD>
#define BFS2_2(STD) SG_Pathfind::BFS::BFS_Flowfield<decltype(arena),decltype(arena), decltype(grid), false, STD>
#define BFS3_2(STD) SG_Pathfind::BFS::BFS_Dmatrix<decltype(arena),decltype(arena), decltype(grid), false, STD>


template <typename T, bool useSTD>
void testQueen(){
    SG_Allocator::Arena_ULL<50000,3> arena;
    SG_Grid::FullGrid<T,11,11> grid;

    
    grid.fill_memset(0);
    assert(BFS1_1(useSTD)(grid, arena, arena, {3,3}, {0,0}, 4).length() == 0);
    assert(!BFS2_1(useSTD)(grid, arena, arena, {3,3}, 4).contains({0,0}));
    assert(!BFS3_1(useSTD)(grid, arena, arena, {3,3}, 4).contains({0,0}));

    grid.fill(1);
    assert(BFS1_1(useSTD)(grid, arena, arena, {3,3}, {0,0}, 4).length() == 1);
    assert(BFS2_1(useSTD)(grid, arena, arena, {3,3}, 4).get({0,0}) == SG_Grid::Point(-1,-1));
    auto test = BFS3_1(useSTD)(grid, arena, arena, {3,3}, 4).get({0,0});

    assert(BFS3_1(useSTD)(grid, arena, arena, {3,3}, 4).get({0,0}) == 3);
    
    grid.set({2,2},0);
    assert(BFS1_1(useSTD)(grid, arena, arena, {3,3}, {0,0}, 4).length() == 3);
    assert(BFS2_1(useSTD)(grid, arena, arena, {3,3}, 4).get({0,0}) == SG_Grid::Point(0,-1));
    assert(BFS3_1(useSTD)(grid, arena, arena, {3,3}, 4).get({0,0}) == 4);
}

template <typename T, bool useSTD>
void testRook(){
    SG_Allocator::Arena_ULL<50000,3> arena;
    SG_Grid::FullGrid<T,11,11> grid;

    grid.fill_memset(0);
    assert(BFS1_2(useSTD)(grid, arena, arena, {3,3}, {0,0}, 8).length() == 0);
    assert(!BFS2_2(useSTD)(grid, arena, arena, {3,3}, 8).contains({0,0}));
    assert(!BFS3_2(useSTD)(grid, arena, arena, {3,3}, 8).contains({0,0}));
    
    grid.fill(1);
    assert(BFS1_2(useSTD)(grid, arena, arena, {3,3}, {0,0}, 8).length() == 2);
    assert(BFS2_2(useSTD)(grid, arena, arena, {3,3}, 8).get({0,0}) == SG_Grid::Point(0,-1));
    assert(BFS3_2(useSTD)(grid, arena, arena, {3,3}, 8).get({0,0}) == 6);
    
    grid.set({2,3},0);
    assert(BFS1_2(useSTD)(grid, arena, arena, {3,3}, {0,0}, 8).length() == 3);
    assert(BFS2_2(useSTD)(grid, arena, arena, {3,3}, 8).get({0,0}) == SG_Grid::Point(0,-1));
    assert(BFS3_2(useSTD)(grid, arena, arena, {3,3}, 8).get({0,0}) == 6);


    assert(BFS1_2(useSTD)(grid, arena, arena, {1,1}, {0,0}, 2).length() == 2);
    assert(BFS2_2(useSTD)(grid, arena, arena, {1,1}, 2).get({0,0}) == SG_Grid::Point(0,-1));
    assert(BFS3_2(useSTD)(grid, arena, arena, {1,1}, 2).get({0,0}) == 2);
}

int main(int, char**) {
    testQueen<bool, false>(); //TODO make it template on different optimisation parameters etc
    testRook<bool, false>();
    testRook<bool, true>();
    testQueen<bool, true>();

    return 0;
}

#undef assert

#undef BFS1_1
#undef BFS2_1
#undef BFS3_1
#undef BFS1_2
#undef BFS2_2
#undef BFS3_2

