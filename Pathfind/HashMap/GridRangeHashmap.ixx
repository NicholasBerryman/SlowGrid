//
// Created by nickberryman on 10/12/25.
//

module;
#include <cmath>
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

//TODO fix this to actually work at the edge of the grid?? Calculations here would make the grid smaller in the ok direction as well?? -> Maybe add an centrepoint offset and calculate relating to that???
export namespace SG_Pathfind {
    namespace HashMap {
        template<typename insideArena_t, typename value_t = bool, bool useBitfield=false, uint8_t partitionContains=0, uint8_t partitionGet=0, bool round2Power = false, bool is2Power = false>
        class GridRangeHashMap : private BaseHashMap<SG_Grid::Point, value_t, SG_Grid::Point>{
        public:
            template <typename pathfindGrid_t> requires (std::is_base_of_v<SG_Grid::BaseGrid<typename pathfindGrid_t::value_type>, pathfindGrid_t> && !(std::is_same_v<value_t, bool>))
            inline GridRangeHashMap(insideArena_t& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& distance) :
                origin(max(0, centrePoint.x() - distance), max(0, centrePoint.y() - distance)),
                containsGrid(arena, partitionCount(innerWidth(within, centrePoint, distance)), partitionCount(innerHeight(within, centrePoint, distance)), innerWidth(within, centrePoint, distance) , innerHeight(within, centrePoint, distance)),
                getGrid(arena, partitionCount(innerWidth<pathfindGrid_t, false>(within, centrePoint, distance)), partitionCount(innerHeight<pathfindGrid_t, false>(within, centrePoint, distance)), innerWidth<pathfindGrid_t, false>(within, centrePoint, distance) , innerHeight<pathfindGrid_t, false>(within, centrePoint, distance))
                #ifndef NDEBUG
                ,tr(SG_Grid::Point(min(within.width(), centrePoint.x() + distance),  min(within.height(), centrePoint.y() + distance)))
                #endif
            { clear(); }

            template <typename pathfindGrid_t> requires (std::is_base_of_v<SG_Grid::BaseGrid<typename pathfindGrid_t::value_type>, pathfindGrid_t> && std::is_same_v<value_t, bool>)
            inline GridRangeHashMap(insideArena_t& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& distance) :
                origin(max(0, centrePoint.x() - distance), max(0, centrePoint.y() - distance)),
                containsGrid(arena,partitionCount(innerWidth(within, centrePoint, distance)), innerHeight(within, centrePoint, distance), innerWidth(within, centrePoint, distance) , innerHeight(within, centrePoint, distance)),
                getGrid(containsGrid)
                #ifndef NDEBUG
                ,tr(SG_Grid::Point(min(within.width(), centrePoint.x() + distance),  min(within.height(), centrePoint.y() + distance)))
                #endif
            { clear(); }


            
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
            typedef std::conditional_t< !(std::is_same_v<value_t, bool>),
                SG_Grid::RuntimeSizeGrid<value_t, false>,
                SG_Grid::RuntimeSizeGrid<bool, useBitfield>
             > getGrid_t;
            typedef SG_Grid::RuntimeSizeGrid<bool, useBitfield> containsGrid_t;
        
            SG_Grid::Point origin;
            std::conditional_t<partitionGet == 0, getGrid_t, SG_Grid::SparseRuntimeGrid<insideArena_t,getGrid_t,is2Power>> getGrid;
            std::conditional_t<partitionGet == 0, containsGrid_t, SG_Grid::SparseRuntimeGrid<insideArena_t,containsGrid_t,is2Power>> containsGrid;
            #ifndef NDEBUG
            SG_Grid::Point tr; //Top Right of grid
            #endif

            template <typename pathfindGrid_t, bool contains = true> static SG_Grid::coordinate_t innerWidth(const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& distance) {
                if constexpr ((contains && partitionContains == 0) || (!contains && partitionGet == 0))
                    return min(within.width(), centrePoint.x() + distance) - max(0, centrePoint.x() - distance)+1;
                else if constexpr (round2Power) {
                    auto x = min(within.width(), centrePoint.x() + distance) - max(0, centrePoint.x() - distance)+1;
                    auto y = std::bit_ceil(static_cast<std::make_unsigned_t<decltype(x)>>(x));
                    if constexpr (contains && partitionContains > 0) return y >> (partitionContains*2);
                    if constexpr (!contains && partitionGet > 0)     return y >> (partitionGet*2);
                } else if constexpr (is2Power) {
                    auto x = min(within.width(), centrePoint.x() + distance) - max(0, centrePoint.x() - distance)+1;
                    auto y = static_cast<std::make_unsigned_t<decltype(x)>>(x);
                    if constexpr (contains && partitionContains > 0) return y >> (partitionContains*2);
                    if constexpr (!contains && partitionGet > 0)     return y >> (partitionGet*2);
                }
                return 0;
            }
            template <typename pathfindGrid_t, bool contains = true> static SG_Grid::coordinate_t innerHeight(const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& distance) {
                if constexpr ((contains && partitionContains == 0) || (!contains && partitionGet == 0))
                    return min(within.height(), centrePoint.y() + distance) - max(0, centrePoint.y() - distance)+1;
                else if constexpr (round2Power) {
                    auto x = min(within.width(), centrePoint.x() + distance) - max(0, centrePoint.x() - distance)+1;
                    auto y = std::bit_ceil(static_cast<std::make_unsigned_t<decltype(x)>>(x));
                    if constexpr (contains && partitionContains > 0) return y >> (partitionContains*2);
                    if constexpr (!contains && partitionGet > 0)     return y >> (partitionGet*2);
                } else if constexpr (is2Power) {
                    auto x = min(within.height(), centrePoint.y() + distance) - max(0, centrePoint.y() - distance)+1;
                    auto y = static_cast<std::make_unsigned_t<decltype(x)>>(x);
                    if constexpr (contains && partitionContains > 0) return y >> (partitionContains*2);
                    if constexpr (!contains && partitionGet > 0)     return y >> (partitionGet*2);
                }
                return 0;
            }
            static SG_Grid::coordinate_t partitionCount(const SG_Grid::coordinate_t& depth) { return 1 << (2*static_cast<std::make_unsigned_t<SG_Grid::coordinate_t>>(depth)); }
        };
    }
}
