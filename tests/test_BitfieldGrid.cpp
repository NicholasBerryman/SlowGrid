//
// Created by nickberryman on 9/12/25.
//
#include <cstdint>

#include "Logger.h"
#define assert Logging::assert_except

import Logger;
import SG_Grid;

void testOnStack(){
    SG_Grid::FullGrid<bool, 11,11, true> grid;

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
	

	grid.fill(0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(1,1)) == 0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(0,1)) == 0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(2,0)) == 0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(10,10)) == 0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(0,0)) == 0);


    grid.fill(1);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(1,1)) == 1);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(0,1)) == 1);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(2,0)) == 1);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(10,10)) == 1);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(0,0)) == 1);
}

int main(int, char**) {
    testOnStack();
    return 0;
}

#undef assert

