//
// Created by nickberryman on 10/12/25.
//

module;
#include <type_traits>
#include "Logger.h"
export module SG_Pathfind:GridRangeHashSet;
import :BaseHashSet;
import SG_Grid;
import Logger;

const SG_Grid::coordinate_t& max(const SG_Grid::coordinate_t& a, const SG_Grid::coordinate_t& b) {
    if (a >= b) return a;
    return b;
}

const SG_Grid::coordinate_t& min(const SG_Grid::coordinate_t& a, const SG_Grid::coordinate_t& b) {
    if (a <= b) return a;
    return b;
}

// TODO change to a HashMap -> Can then use as a flow-field grid
export namespace SG_Pathfind {
    namespace HashSet {
        template<bool isBitfield=false>
        class GridRangeHashset : private BaseHashset<SG_Grid::Point, SG_Grid::Point>{
        public:
            template <typename pathfindGrid_t, typename insideArena_t>
            requires std::is_base_of_v<SG_Grid::BaseGrid<typename pathfindGrid_t::value_type>, pathfindGrid_t>
            inline GridRangeHashset(insideArena_t& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& distance) :
                origin(max(0, centrePoint.x() - distance), max(0, centrePoint.y() - distance)),
                hashGrid(arena, min(within.width(), centrePoint.x() + distance) - max(0, centrePoint.x() - distance)+1,  min(within.height(), centrePoint.y() + distance) - max(0, centrePoint.y() - distance)+1)
#ifndef NDEBUG
                ,tr(SG_Grid::Point(min(within.width(), centrePoint.x() + distance),  min(within.height(), centrePoint.y() + distance)))
#endif
            {
                hashGrid.fill(0);
            };
            inline void insert(const SG_Grid::Point& value ) {hashGrid.set(calcHash(value),1);}
            inline const bool& contains(const SG_Grid::Point& value ) {return hashGrid.get(calcHash(value));}
            inline void remove(const SG_Grid::Point& value ) { hashGrid.set(calcHash(value), 0); }
            [[nodiscard]] inline SG_Grid::Point calcHash(const SG_Grid::Point& toHash) const {
                LOGGER_ASSERT_EXCEPT(toHash.x() >= origin.x() && toHash.y() >= origin.y() && toHash.x() <= tr.x() && toHash.y() <= tr.y());
                return toHash-origin;
            }
            inline void clear() {hashGrid.fill(0);}

#ifndef NDEBUG
            SG_Grid::Point tr;
#endif
            SG_Grid::Point origin;
            SG_Grid::RuntimeSizeGrid<bool, isBitfield> hashGrid;
        };
    }
}
