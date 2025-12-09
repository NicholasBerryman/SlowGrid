//
// Created by nickberryman on 10/12/25.
//
#include <cstdint>

#include "Logger.h"
#define assert Logging::assert_except

import Logger;
import SG_Grid;

template <typename T>
void testOnStack() {
    SG_Grid::FullGrid<T,3,3> chunk1;
    SG_Grid::FullGrid<T,3,3> chunk2;
    SG_Grid::SparseGrid<SG_Grid::FullGrid<T,3,3>,6,3> grid;

    chunk1.set(SG_Grid::Point(1,1), 15);
    chunk1.set(SG_Grid::Point(0,1), 11);
    chunk1.set(SG_Grid::Point(2,0), 17);

    grid.setChunk(SG_Grid::Point(0,0), &chunk1);
    grid.setChunk(SG_Grid::Point(1,0), &chunk2);

    chunk2.set(SG_Grid::Point(2,2), 19);
    chunk2.set(SG_Grid::Point(0,0), 6);

    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(1,1)) == 15);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(0,1)) == 11);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(2,0)) == 17);
    LOGGER_ASSERT_EXCEPT(grid.height() == 3);
    LOGGER_ASSERT_EXCEPT(grid.width() == 6);
    LOGGER_ASSERT_EXCEPT(grid.chunksHigh() == 1);
    LOGGER_ASSERT_EXCEPT(grid.chunksWide() == 2);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(5,2)) == 19);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(3,0)) == 6);

    grid.set(SG_Grid::Point(5,2), 120);
    LOGGER_ASSERT_EXCEPT(grid.get(SG_Grid::Point(5,2)) == 120);
}

int main(int, char**) {
    testOnStack<char>();
    testOnStack<uint64_t>();
    return 0;
}

#undef assert

