//
// Created by nickberryman on 9/12/25.
//
#include <cstdint>

#include "Logger.h"
#define assert Logging::assert_except

import Logger;
import SG_Grid;

template <typename T>
void testOnStack(){
    SG_Grid::FullGrid<T,3,3> grid;

    grid.set(SG_Grid::Point(1,1), 15);
    grid.set(SG_Grid::Point(0,1), 11);
    grid.set(SG_Grid::Point(2,0), 17);

    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(1,1)) == 15);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(0,1)) == 11);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(2,0)) == 17);
    LOGGER_ASSERT_EXCEPT(grid.height() == 3);
    LOGGER_ASSERT_EXCEPT(grid.width() == 3);

    LOGGER_ASSERT_ERROR( grid.get(SG_Grid::Point(0,3)); )
	
	grid.fill(0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(1,1)) == 0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(0,1)) == 0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(2,0)) == 0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(2,2)) == 0);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(0,0)) == 0);
}

int main(int, char**) {
    testOnStack<char>();
    testOnStack<uint64_t>();
    return 0;
}

#undef assert

