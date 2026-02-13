//
// Created by nickberryman on 10/12/25.
//
#include <cstdint>

#include "Logger.h"
#define assert Logging::assert_except

import Logger;
import SG_Grid;
import SG_Allocator;

template <typename T>
void testOnStack() {
    SG_Grid::FullGrid<T,3,3> chunk1;
    SG_Grid::FullGrid<T,3,3> chunk2;
    SG_Grid::SparseGrid<SG_Grid::FullGrid<T,3,3>, 6,3> grid;

    chunk1.set(SG_Grid::Point(1,1), 15);
    chunk1.set(SG_Grid::Point(0,1), 11);
    chunk1.set(SG_Grid::Point(2,0), 17);

    grid.setChunk(SG_Grid::Point(0,0), &chunk1);
    grid.setChunk(SG_Grid::Point(1,0), &chunk2);

    chunk2.set(SG_Grid::Point(2,2), 19);
    chunk2.set(SG_Grid::Point(0,0), 6);

    assert(grid.get(SG_Grid::Point(1,1)) == 15);
    assert(grid.get(SG_Grid::Point(0,1)) == 11);
    assert(grid.get(SG_Grid::Point(2,0)) == 17);
    assert(grid.height() == 3);
    assert(grid.width() == 6);
    assert(grid.chunksHigh() == 1);
    assert(grid.chunksWide() == 2);
    assert(grid.get(SG_Grid::Point(5,2)) == 19);
    assert(grid.get(SG_Grid::Point(3,0)) == 6);

    grid.set(SG_Grid::Point(5,2), 120);
    assert(grid.get(SG_Grid::Point(5,2)) == 120);
}

template <typename T>
void testRuntimeSized() {
    SG_Allocator::Arena_ULL<10000,2> arena;
    SG_Grid::SparseRuntimeGrid<decltype(arena), SG_Grid::RuntimeSizeGrid<T, false>> grid(arena, 2,1,3,3) ;

    grid.loadChunk({0,0});
    grid.set(SG_Grid::Point(1,1), 15);
    grid.set(SG_Grid::Point(0,1), 11);
    grid.set(SG_Grid::Point(2,0), 17);
    grid.loadChunk({1,0});
    grid.set(SG_Grid::Point(5,2), 19);
    grid.set(SG_Grid::Point(3,0), 6);

    assert(grid.get(SG_Grid::Point(1,1)) == 15);
    assert(grid.get(SG_Grid::Point(0,1)) == 11);
    assert(grid.get(SG_Grid::Point(2,0)) == 17);
    assert(grid.height() == 3);
    assert(grid.width() == 6);
    assert(grid.chunksHigh() == 1);
    assert(grid.chunksWide() == 2);
    assert(grid.get(SG_Grid::Point(5,2)) == 19);
    assert(grid.get(SG_Grid::Point(3,0)) == 6);

    grid.set(SG_Grid::Point(5,2), 120);
    assert(grid.get(SG_Grid::Point(5,2)) == 120);

}

int main(int, char**) {
    testOnStack<char>();
    testOnStack<uint64_t>();


    testRuntimeSized<char>();
    testRuntimeSized<uint64_t>();
    return 0;
}

#undef assert

