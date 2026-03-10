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
    
    SG_Pathfind::BFS::BFS_Point<decltype(arena),decltype(arena), decltype(grid), SG_Pathfind::HashMap::GridRangeHashMap<decltype(arena),SG_Grid::Point>> (grid, arena, arena, {3,3}, {0,0}, 5);
}

int main(int, char**) {
    test1<bool>(); //TODO make it template on different optimisation parameters etc
    return 0;
}

#undef assert

