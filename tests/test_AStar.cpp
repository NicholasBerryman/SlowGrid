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

#define AStar1_1(STD, heap, fifo, useHs) SG_Pathfind::AStar::Bench::AStar_Point_<256, true, STD, heap, fifo, useHs>
#define AStar2_1(STD, heap, fifo, useHs) SG_Pathfind::AStar::Bench::Dijkstra_Flowfield_<true, STD, heap, fifo, useHs>
#define AStar3_1(STD, heap, fifo, useHs) SG_Pathfind::AStar::Bench::Dijkstra_Dmatrix_<true, STD, heap, fifo, useHs>

#define AStar1_2(STD,heap, fifo, useHs) SG_Pathfind::AStar::Bench::AStar_Point_<256, false, STD, heap, fifo, useHs>
#define AStar2_2(STD,heap, fifo, useHs) SG_Pathfind::AStar::Bench::Dijkstra_Flowfield_<false, STD, heap, fifo, useHs>
#define AStar3_2(STD,heap, fifo, useHs) SG_Pathfind::AStar::Bench::Dijkstra_Dmatrix_<false, STD, heap, fifo, useHs>


#define p2plen(command, len, start, dist) { \
    auto testPoint2 = command; \
    testPoint2.push(start); \
    auto testLen2 = 0; for (auto i = 1; i < testPoint2.length(); i++) testLen2 += SG_Grid::Distance::dist(testPoint2.peekRef(i-1), testPoint2.peekRef(i)); \
    assert(len == testLen2); }


template <typename T, bool useSTD, bool heap = false, bool fifo = false, bool useHs = true>
void testQueen(){
    SG_Allocator::Arena_ULL<50000,3> arena;
    SG_Grid::FullGrid<T,11,11> grid;

    grid.fill_memset(0);
    assert(AStar1_1(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, {0,0}, 4).length() == 0);
    assert(!AStar2_1(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, 4).contains({0,0}));
    assert(!AStar3_1(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, 4).contains({0,0}));

    grid.fill(1);
    p2plen(AStar1_1(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, {0,0}, 4), 3, SG_Grid::Point(3,3), Chebyshev);
    assert(AStar2_1(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, 4).get({0,0}) == SG_Grid::Point(-1,-1));
    assert(AStar3_1(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, 4).get({0,0}) == 3);
    
    grid.set({2,2},0);
    auto test = AStar1_1(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, {0,0}, 4);
    p2plen(test, 4, SG_Grid::Point(3,3), Chebyshev);
    assert(AStar2_1(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, 5).get({0,0}) != SG_Grid::Point(0,0));
    assert(AStar3_1(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, 5).get({0,0}) == 4);
}

template <typename T, bool useSTD, bool heap = false, bool fifo = false, bool useHs = true>
void testRook(){
    SG_Allocator::Arena_ULL<50000,3> arena;
    SG_Grid::FullGrid<T,11,11> grid;

    grid.fill_memset(0);
    assert(AStar1_2(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, {0,0}, 8).length() == 0);
    assert(!AStar2_2(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, 8).contains({0,0}));
    assert(!AStar3_2(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, 8).contains({0,0}));
    
    grid.fill(1);
    p2plen(AStar1_2(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, {0,0}, 8), 6, SG_Grid::Point(3,3), Manhattan);
    assert(AStar2_2(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, 8).get({0,0}) != SG_Grid::Point(0,0));
    assert(AStar3_2(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, 8).get({0,0}) == 6);
    
    grid.set({2,3},0);
    p2plen(AStar1_2(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, {0,0}, 8), 6, SG_Grid::Point(3,3), Manhattan);
    assert(AStar2_2(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, 8).get({0,0}) != SG_Grid::Point(0,0));
    assert(AStar3_2(useSTD, heap, fifo, useHs)(grid, arena, arena, {3,3}, 8).get({0,0}) == 6);


    p2plen(AStar1_2(useSTD, heap, fifo, useHs)(grid, arena, arena, {1,1}, {0,0}, 2), 2, SG_Grid::Point(1,1), Manhattan);
    assert(AStar2_2(useSTD, heap, fifo, useHs)(grid, arena, arena, {1,1}, 2).get({0,0}) != SG_Grid::Point(0,0));
    assert(AStar3_2(useSTD, heap, fifo, useHs)(grid, arena, arena, {1,1}, 2).get({0,0}) == 2);
}

int main(int, char**) {
    testQueen<bool, false, false, false>();
    testRook<bool, false, false, false>();
    testRook<bool, true, false, false>();
    testQueen<bool, true, false, false>();
    testQueen<bool, false, true, false>();
    testRook<bool, false, true, false>();

    testQueen<bool, false, false, true>();
    testRook<bool, false, false, true>();
    testRook<bool, true, false, true>();
    testQueen<bool, true, false, true>();
    testQueen<bool, false, true, true>();
    testRook<bool, false, true, true>();



    testQueen<bool, false, false, false, false>();
    testRook<bool, false, false, false, false>();
    testRook<bool, true, false, false, false>();
    testQueen<bool, true, false, false, false>();
    testQueen<bool, false, true, false, false>();
    testRook<bool, false, true, false, false>();

    testQueen<bool, false, false, true, false>();
    testRook<bool, false, false, true, false>();
    testRook<bool, true, false, true, false>();
    testQueen<bool, true, false, true, false>();
    testQueen<bool, false, true, true, false>();
    testRook<bool, false, true, true, false>();
    return 0;
}



#undef assert

#undef AStar1_1
#undef AStar2_1
#undef AStar3_1
#undef AStar1_2
#undef AStar2_2
#undef AStar3_2

