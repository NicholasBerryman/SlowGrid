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

const SG_Grid::u_coordinate_t& max(const SG_Grid::u_coordinate_t& a, const SG_Grid::u_coordinate_t& b) {
    if (a >= b) return a;
    return b;
}

const SG_Grid::u_coordinate_t& min(const SG_Grid::u_coordinate_t& a, const SG_Grid::u_coordinate_t& b) {
    if (a <= b) return a;
    return b;
}

//TODO allow this HashMap to use a refernce to another HashMap's 'containsGrid'
    // Or maybe make it possible to create without a 'containtsGrid', but still without zero-initialising the 'get Grid' -> Can then manually use the other set as the containsGrid

export namespace SG_Pathfind::HashMap {
    template<typename insideArena_t, typename value_t = bool, bool useContains=true, bool useBitfield=true, uint8_t partitionContains=0, uint8_t partitionGet=0, bool is2Power = false>
    class GridRangeHashMap : private BaseHashMap<SG_Grid::Point, value_t, SG_Grid::Point>{
    private:    
        static constexpr uint8_t partitionGet_ = []{if constexpr (std::is_same_v<value_t, bool>) return partitionContains; else return partitionGet;}();
        static constexpr bool useContains_  = useContains || std::is_same_v<value_t, bool>;
    
    public:  
        template <typename pathfindGrid_t> requires (std::is_base_of_v<SG_Grid::BaseGrid<typename pathfindGrid_t::value_type>, pathfindGrid_t> && !(std::is_same_v<value_t, bool>))
        inline GridRangeHashMap(insideArena_t& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::u_coordinate_t& distance) :
            origin(max(0, centrePoint.x() - distance), max(0, centrePoint.y() - distance)),
            containsGrid(arena, partitionCount(partitionContains), partitionCount(partitionContains), innerWidth(within, centrePoint, distance) , innerHeight(within, centrePoint, distance)),
            getGrid(arena, partitionCount(partitionGet_), partitionCount(partitionGet_), innerWidth<pathfindGrid_t, false>(within, centrePoint, distance) , innerHeight<pathfindGrid_t, false>(within, centrePoint, distance)),
            containsChunkLoaded(arena, partitionCount(partitionContains), partitionCount(partitionContains)),
            getChunkLoaded(arena, partitionCount(partitionGet_), partitionCount(partitionGet_))
            #ifndef NDEBUG
            ,tr(SG_Grid::Point(min(within.width(), centrePoint.x() + distance),  min(within.height(), centrePoint.y() + distance)))
            #endif
        { clear(); }

        template <typename pathfindGrid_t> requires (std::is_base_of_v<SG_Grid::BaseGrid<typename pathfindGrid_t::value_type>, pathfindGrid_t> && std::is_same_v<value_t, bool>)
        inline GridRangeHashMap(insideArena_t& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::u_coordinate_t& distance) :
            origin(max(0, centrePoint.x() - distance), max(0, centrePoint.y() - distance)),
            containsGrid(arena, partitionCount(partitionContains), partitionCount(partitionContains), innerWidth(within, centrePoint, distance) , innerHeight(within, centrePoint, distance)),
            getGrid(containsGrid),
            containsChunkLoaded(arena, partitionCount(partitionContains), partitionCount(partitionContains)),
            getChunkLoaded(containsChunkLoaded)
            #ifndef NDEBUG
            ,tr(SG_Grid::Point(min(within.width(), centrePoint.x() + distance),  min(within.height(), centrePoint.y() + distance)))
            #endif
        { clear(); }


        inline void insert(const SG_Grid::Point& key) requires (std::is_same_v<value_t, bool>) { insert_(key); }
        inline void insert(const SG_Grid::Point& key, const value_t& val) requires (!std::is_same_v<value_t, bool>){
            if constexpr (partitionGet_ > 0) {
                const auto chunk = getGrid.chunkLocation(calcHash(key));
                if (!getChunkLoaded.get(chunk)) {
                    getChunkLoaded.set(chunk, 1);
                    getGrid.template loadChunk<true>(chunk);
                }
            }
            insert_(key);
            getGrid.set(calcHash(key),val);
        }

        inline bool contains(const SG_Grid::Point& key ) requires (useContains_) {
            if constexpr (useContains_ && partitionContains > 0) if (!containsChunkLoaded.get(containsGrid.chunkLocation(calcHash(key)))) return false;
            return containsGrid.get(calcHash(key));
        }

        inline value_t& get(const SG_Grid::Point& key ) requires (!(std::is_same_v<value_t, bool>)) {
            if constexpr (partitionGet_ > 0) LOGGER_ASSERT_EXCEPT(getChunkLoaded.get(getGrid.chunkLocation(calcHash(key))));
            return getGrid.get(calcHash(key));
        }

        inline void remove(const SG_Grid::Point& key ) { if constexpr (useContains_) containsGrid.set(calcHash(key), 0); }
        
        [[nodiscard]] inline SG_Grid::Point calcHash(const SG_Grid::Point& toHash) const {
            LOGGER_ASSERT_EXCEPT(toHash.x() >= origin.x() && toHash.y() >= origin.y() && toHash.x() <= tr.x() && toHash.y() <= tr.y());
            return toHash-origin;
        }
        
        inline void clear() {
            if constexpr (useContains_){
                if constexpr (partitionContains == 0) containsGrid.fill_memset(0);
                else containsChunkLoaded.fill_memset(0);
            }
            if constexpr (partitionGet_ > 0 && !useBitfield) getChunkLoaded.fill_memset(0);
        }

    private:
        typedef SG_Grid::RuntimeSizeGrid<bool, useBitfield, (partitionContains > 0)> containsGrid_t; //Only enable 2Power optimisation if it's rounded because we want to use sparse storage
        struct empty: public SG_Grid::BaseGrid<value_t> {
            empty(const auto&, const auto&, const auto&, const auto&, const auto&){};
        };

        typedef std::conditional_t< !(std::is_same_v<value_t, bool>),
            SG_Grid::RuntimeSizeGrid<value_t, false, (partitionGet_ > 0)>,
            std::add_lvalue_reference_t<containsGrid_t>
        > getGrid_t;
        struct emptyChunkLoad {emptyChunkLoad(insideArena_t& arena, const SG_Grid::u_coordinate_t& width, const SG_Grid::u_coordinate_t& height){}};

        SG_Grid::Point origin;

        std::conditional_t<useContains_,
            std::conditional_t<partitionContains == 0, containsGrid_t, SG_Grid::SparseRuntimeGrid<insideArena_t,containsGrid_t>>,
            empty
        > containsGrid;

        std::conditional_t<partitionGet_ == 0, getGrid_t,
            std::conditional_t<!(std::is_same_v<value_t, bool>), 
                SG_Grid::SparseRuntimeGrid<insideArena_t,std::remove_reference_t<getGrid_t>>, 
                std::add_lvalue_reference_t<decltype(containsGrid)> >
        > getGrid;

        [[no_unique_address]] std::conditional_t<partitionContains == 0, emptyChunkLoad, SG_Grid::RuntimeSizeGrid<bool, useBitfield>> containsChunkLoaded;
        [[no_unique_address]] std::conditional_t<std::is_same_v<value_t, bool>, std::add_lvalue_reference_t<decltype(containsChunkLoaded)>,
            std::conditional_t<partitionGet_ == 0,
                emptyChunkLoad, 
                SG_Grid::RuntimeSizeGrid<bool, useBitfield>
        >> getChunkLoaded;
        #ifndef NDEBUG
        SG_Grid::Point tr; //Top Right of grid
        #endif

        template <typename pathfindGrid_t, bool contains = true> static SG_Grid::u_coordinate_t innerWidth(const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::u_coordinate_t& distance) {
            if constexpr ((contains && partitionContains == 0) || (!contains && partitionGet_ == 0))
                return min(within.width(), centrePoint.x() + distance) - max(0, centrePoint.x() - distance)+1;
            else if constexpr (!is2Power) {
                auto x = min(within.width(), centrePoint.x() + distance) - max(0, centrePoint.x() - distance)+1;
                auto y = std::bit_ceil(static_cast<std::make_unsigned_t<decltype(x)>>(x));
                if constexpr (contains && partitionContains > 0) return y >> (partitionContains);
                if constexpr (!contains && partitionGet_ > 0)     return y >> (partitionGet_);
            } else if constexpr (is2Power) {
                auto x = min(within.width(), centrePoint.x() + distance) - max(0, centrePoint.x() - distance)+1;
                auto y = static_cast<std::make_unsigned_t<decltype(x)>>(x);
                if constexpr (contains && partitionContains > 0) return y >> (partitionContains);
                if constexpr (!contains && partitionGet_ > 0)     return y >> (partitionGet_);
            }
            return 0;
        }
        template <typename pathfindGrid_t, bool contains = true> static SG_Grid::u_coordinate_t innerHeight(const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::u_coordinate_t& distance) {
            if constexpr ((contains && partitionContains == 0) || (!contains && partitionGet_ == 0))
                return min(within.height(), centrePoint.y() + distance) - max(0, centrePoint.y() - distance)+1;
            else if constexpr (!is2Power) {
                auto x = min(within.width(), centrePoint.x() + distance) - max(0, centrePoint.x() - distance)+1;
                auto y = std::bit_ceil(static_cast<std::make_unsigned_t<decltype(x)>>(x));
                if constexpr (contains && partitionContains > 0) return y >> (partitionContains);
                if constexpr (!contains && partitionGet_ > 0)     return y >> (partitionGet_);
            } else if constexpr (is2Power) {
                auto x = min(within.height(), centrePoint.y() + distance) - max(0, centrePoint.y() - distance)+1;
                auto y = static_cast<std::make_unsigned_t<decltype(x)>>(x);
                if constexpr (contains && partitionContains > 0) return y >> (partitionContains);
                if constexpr (!contains && partitionGet_ > 0)     return y >> (partitionGet_);
            }
            return 0;
        }
        static SG_Grid::u_coordinate_t partitionCount(const SG_Grid::u_coordinate_t& depth) { return 1 << (static_cast<SG_Grid::u_coordinate_t>(depth)); }
        
        inline void insert_(const SG_Grid::Point& key) {
            if constexpr ( useContains_) {
                if constexpr (partitionContains > 0) {
                    const auto chunk = containsGrid.chunkLocation(calcHash(key));
                    if (!containsChunkLoaded.get(chunk)) {
                        containsChunkLoaded.set(chunk, 1);
                        containsGrid.template loadChunk<true>(chunk);
                    }
                }
                containsGrid.set(calcHash(key),1);
            }
        }
        
    };
}

