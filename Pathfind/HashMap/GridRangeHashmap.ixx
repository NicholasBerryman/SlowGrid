//
// Created by nickberryman on 10/12/25.
//

module;
#include <type_traits>
#include "Logger.h"
#include <cstdint>

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


//TODO use sparsegrid for partitions!!!
export namespace SG_Pathfind {
    namespace HashMap {
        template<typename value_t = bool, bool useBitfield=false, uint8_t partitionContains=0, uint8_t partitionGet=0>
        class GridRangeHashMap : private BaseHashMap<SG_Grid::Point, value_t, SG_Grid::Point>{
        public:
            template <typename pathfindGrid_t, typename insideArena_t> requires (std::is_base_of_v<SG_Grid::BaseGrid<typename pathfindGrid_t::value_type>, pathfindGrid_t> && !(std::is_same_v<value_t, bool>))
            inline GridRangeHashMap(insideArena_t& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& distance) :
                origin(max(0, centrePoint.x() - distance), max(0, centrePoint.y() - distance)),
                containsGrid(arena, min(within.width(), centrePoint.x() + distance) - max(0, centrePoint.x() - distance)+1,  min(within.height(), centrePoint.y() + distance) - max(0, centrePoint.y() - distance)+1),
                getGrid(arena, min(within.width(), centrePoint.x() + distance) - max(0, centrePoint.x() - distance)+1,  min(within.height(), centrePoint.y() + distance) - max(0, centrePoint.y() - distance)+1)
                #ifndef NDEBUG
                ,tr(SG_Grid::Point(min(within.width(), centrePoint.x() + distance),  min(within.height(), centrePoint.y() + distance)))
                #endif
            { clear(); };
            
            template <typename pathfindGrid_t, typename insideArena_t> requires (std::is_base_of_v<SG_Grid::BaseGrid<typename pathfindGrid_t::value_type>, pathfindGrid_t> && std::is_same_v<value_t, bool>)
            inline GridRangeHashMap(insideArena_t& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& distance) :
                origin(max(0, centrePoint.x() - distance), max(0, centrePoint.y() - distance)),
                containsGrid(arena, min(within.width(), centrePoint.x() + distance) - max(0, centrePoint.x() - distance)+1,  min(within.height(), centrePoint.y() + distance) - max(0, centrePoint.y() - distance)+1),
                getGrid(containsGrid)
                #ifndef NDEBUG
                ,tr(SG_Grid::Point(min(within.width(), centrePoint.x() + distance),  min(within.height(), centrePoint.y() + distance)))
                #endif
            { clear(); };
            
            inline void insert(const SG_Grid::Point& key) { containsGrid.set(calcHash(key),1); }
            inline void insert(const SG_Grid::Point& key, const value_t val) {insert(calcHash(key)); getGrid.set(calcHash(key),val); }
            inline bool contains(const SG_Grid::Point& key ) {return containsGrid.get(calcHash(key));}
            inline value_t& get(const SG_Grid::Point& key ) requires (!(useBitfield && std::is_same_v<value_t, bool>)) {return getGrid.get(calcHash(key));}
            inline value_t  get(const SG_Grid::Point& key ) requires (  useBitfield && std::is_same_v<value_t, bool>)  {return getGrid.get(calcHash(key));}
            inline void remove(const SG_Grid::Point& key ) { containsGrid.set(calcHash(key), 0); }
            [[nodiscard]] inline SG_Grid::Point calcHash(const SG_Grid::Point& toHash) const {
                LOGGER_ASSERT_EXCEPT(toHash.x() >= origin.x() && toHash.y() >= origin.y() && toHash.x() <= tr.x() && toHash.y() <= tr.y());
                return toHash-origin;
            }
            inline void clear() { containsGrid.fill_memset(0); }

        private:
            struct empty {};
        
            SG_Grid::Point origin;
            std::conditional_t< !(std::is_same_v<value_t, bool>),
                SG_Grid::RuntimeSizeGrid<value_t, false>,
                SG_Grid::RuntimeSizeGrid<bool, useBitfield>&
             > getGrid;
            SG_Grid::RuntimeSizeGrid<bool, useBitfield> containsGrid;
            
            std::conditional_t<partitionGet == 0, empty, SG_Grid::RuntimeSizeGrid<uint8_t, false> > getPartitionGrid;
            std::conditional_t<partitionContains == 0, empty, SG_Grid::RuntimeSizeGrid<uint8_t, false> > containsPartitionGrid;
            
            //SG_Grid::coordinate_t partition
            
            #ifndef NDEBUG
            SG_Grid::Point tr; //Top Right of grid
            #endif
            
        };
    }
}
