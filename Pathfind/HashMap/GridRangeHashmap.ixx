//
// Created by nickberryman on 10/12/25.
//

module;
#include <type_traits>
#include "Logger.h"
export module SG_Pathfind:GridRangeHashMap;
import :BaseHashMap;
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

export namespace SG_Pathfind {
    namespace HashMap {
        template<typename value_t = bool, bool isBitfield=false>
        class GridRangeHashMap : private BaseHashMap<SG_Grid::Point, value_t, SG_Grid::Point>{
        public:
            template <typename pathfindGrid_t, typename insideArena_t>
            requires std::is_base_of_v<SG_Grid::BaseGrid<typename pathfindGrid_t::value_type>, pathfindGrid_t>
            inline GridRangeHashMap(insideArena_t& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& distance, const value_t defaultValue = 0) :
                origin(max(0, centrePoint.x() - distance), max(0, centrePoint.y() - distance)),
                hashGrid(arena, min(within.width(), centrePoint.x() + distance) - max(0, centrePoint.x() - distance)+1,  min(within.height(), centrePoint.y() + distance) - max(0, centrePoint.y() - distance)+1),
                _defaultValue(defaultValue)
#ifndef NDEBUG
                ,tr(SG_Grid::Point(min(within.width(), centrePoint.x() + distance),  min(within.height(), centrePoint.y() + distance)))
#endif
            { hashGrid.fill(defaultValue); };
            
            inline void insert(const SG_Grid::Point& key, const value_t val = 1) {hashGrid.set(calcHash(key),val);}
            inline bool contains(const SG_Grid::Point& key ) {return hashGrid.get(calcHash(key)) != _defaultValue;}
            inline value_t& get(const SG_Grid::Point& key ) requires (!isBitfield) {return hashGrid.get(calcHash(key));}
            inline value_t get(const SG_Grid::Point& key ) requires (isBitfield) {return hashGrid.get(calcHash(key));}
            inline void remove(const SG_Grid::Point& key ) { hashGrid.set(calcHash(key), _defaultValue); }
            [[nodiscard]] inline SG_Grid::Point calcHash(const SG_Grid::Point& toHash) const {
                LOGGER_ASSERT_EXCEPT(toHash.x() >= origin.x() && toHash.y() >= origin.y() && toHash.x() <= tr.x() && toHash.y() <= tr.y());
                return toHash-origin;
            }
            inline void clear() {hashGrid.fill(_defaultValue);}

#ifndef NDEBUG
            SG_Grid::Point tr; //Top Right of grid
#endif
            SG_Grid::Point origin;
            SG_Grid::RuntimeSizeGrid<value_t, isBitfield> hashGrid;
            value_t _defaultValue;
        };
    }
}
