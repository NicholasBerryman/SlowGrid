//
// Created by nickberryman on 9/12/25.
//
#include <cstdint>
#include "Logger.h"
#define assert Logging::assert_except

import Logger;
import SG_Grid;
import SG_Allocator;

void testOnStack(){
    SG_Allocator::Arena_ULL<50000,3> arena;
    SG_Grid::RuntimeSizeGrid<bool, true> grid(arena,11,11);

    grid.set(SG_Grid::Point(1,1), 1);
    grid.set(SG_Grid::Point(0,1), 0);
    grid.set(SG_Grid::Point(2,0), 1);
    grid.set(SG_Grid::Point(10,10), 1);

    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(1,1)) == 1);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(0,1)) == 0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(2,0)) == 1);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(10,10)) == 1);
    LOGGER_ASSERT_EXCEPT(grid.height() == 11);
    LOGGER_ASSERT_EXCEPT(grid.width() == 11);

    LOGGER_ASSERT_ERROR( grid.get(SG_Grid::Point(11,0)); )
	
	grid.fill(false);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(1,1)) == 0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(0,1)) == 0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(2,0)) == 0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(10,10)) == 0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(0,0)) == 0);


    SG_Grid::RuntimeSizeGrid<char> grid2(arena, 3,3);
    grid2.set(SG_Grid::Point(1,1), 15);
    grid2.set(SG_Grid::Point(0,1), 11);
    grid2.set(SG_Grid::Point(2,0), 17);

    LOGGER_ASSERT_EXCEPT(grid2.get(SG_Grid::Point(1,1)) == 15);
    LOGGER_ASSERT_EXCEPT(grid2.get(SG_Grid::Point(0,1)) == 11);
    LOGGER_ASSERT_EXCEPT(grid2.get(SG_Grid::Point(2,0)) == 17);
    LOGGER_ASSERT_EXCEPT(grid2.height() == 3);
    LOGGER_ASSERT_EXCEPT(grid2.width() == 3);

    LOGGER_ASSERT_ERROR( grid2.get(SG_Grid::Point(0,3)); )

    grid2.fill(0);
    LOGGER_ASSERT_EXCEPT(grid2.get(SG_Grid::Point(1,1)) == 0);
    LOGGER_ASSERT_EXCEPT(grid2.get(SG_Grid::Point(0,1)) == 0);
    LOGGER_ASSERT_EXCEPT(grid2.get(SG_Grid::Point(2,0)) == 0);
    LOGGER_ASSERT_EXCEPT(grid2.get(SG_Grid::Point(2,2)) == 0);
    LOGGER_ASSERT_EXCEPT(grid2.get(SG_Grid::Point(0,0)) == 0);

    grid2.fill(10);
    LOGGER_ASSERT_EXCEPT(grid2.get(SG_Grid::Point(1,1)) == 10);
    LOGGER_ASSERT_EXCEPT(grid2.get(SG_Grid::Point(0,1)) == 10);
    LOGGER_ASSERT_EXCEPT(grid2.get(SG_Grid::Point(2,0)) == 10);
    LOGGER_ASSERT_EXCEPT(grid2.get(SG_Grid::Point(2,2)) == 10);
    LOGGER_ASSERT_EXCEPT(grid2.get(SG_Grid::Point(0,0)) == 10);
}

int main(int, char**) {
    testOnStack();
    return 0;
}

#undef assert

