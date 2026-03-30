//
// Created by nickberryman on 10/12/25.
//
module;
#include <type_traits>
#include <algorithm>
#include <cstdint>
#include "Logger.h"

export module SG_Pathfind:BFS;
import LocalDataStructures;
import Logger;
import SG_Allocator;
import SG_Grid;
import :Utils;
import :GridRangeHashMap;
import :STDHashMap;
import :NoPriorityQueue;
import :STDNoPriorityQueue;


//TODO keep track of best heuristic tile, and path to that it we don't get the endPoint jishin


#define SG_PATHFIND_BFS(Flowfield, Distances) BFS_Base<useSTD, queensCase, Flowfield, Distances>

namespace SG_Pathfind::BFS {
    template<bool useSTD, bool queensCase, bool Flowfield, bool Distances>
    inline SG_Grid::Point BFS_Base(SG_Allocator::BaseArena_c<char, char> auto& arena, const SG_Grid::ReadableGrid_c auto& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance, auto& visited, auto& directions, const SG_Grid::Point& endPoint_ = {0,0}){
        using arena_t = std::remove_reference_t<decltype(arena)>;
        using grid_t = std::remove_reference_t<decltype(OnGrid)>;
        typedef std::conditional_t<useSTD, PriorityQueue::STDNoPriorityQueue<arena_t, grid_t, queensCase, !Distances, false, false>, PriorityQueue::NoPriorityQueue<arena_t, grid_t, queensCase, !Distances, false, false>> queue_t; \
        queue_t frontier(arena, OnGrid, startPoint, searchDistance);
        frontier.insert(startPoint,0,true);
        SG_Grid::Point examine = startPoint;
        if constexpr (!Distances) visited.insert(startPoint, startPoint);
        else visited.insert(startPoint, 0);
        SG_Grid::u_coordinate_t nextDistance = !Distances;
        frontier.trySwap();
        while (frontier.length() > 0) {
            examine = frontier.extractMin();
            if constexpr (Distances){ nextDistance = visited.get(examine)+1; }
            if (nextDistance > searchDistance) break;
            for (auto i = 0; i < directions.length(); ++i){
                auto next = examine + directions.peekRef(i);
                if (!(next.on(OnGrid))) continue; /* Don't look past the grid bounds */
                if (OnGrid.get(next)) { /* Not a wall (0 = wall)*/
                    if (!visited.contains(next)) {
                        frontier.insert(next,0,true);
                        if constexpr (Distances) visited.insert(next, nextDistance);
                        else visited.insert(next, directions.peekRef(i));
                    }
                }
                if constexpr (!Flowfield) { if (next == endPoint_) { examine = next; goto found; } } /* We hit the end point - nice! */
            }
            if constexpr (!Distances) { if (frontier.trySwap()) ++nextDistance; }
        }
        found: /* The last remaining use for goto :) */
        return examine;
    }
    
    template<std::uint8_t type, bool queensCase = true, bool useSTD = false>
    inline auto&  BFS_Pathfind(auto& out, SG_Allocator::BaseArena_c<char, char> auto& arena, const SG_Grid::ReadableGrid_c auto& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance, const SG_Grid::Point& endPoint = {0,0}) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
         if constexpr (type == 0) {LOGGER_ASSERT_EXCEPT(endPoint.on(OnGrid));}
        auto& directions = Utils::AvailableMoves<queensCase>();

        arena.sublifetime_open();
        if constexpr (type == 0) { // 0 = P2P
            Utils::defaultFlowfield_t<std::remove_reference_t<decltype(arena)>, useSTD> visited(arena, OnGrid, startPoint, searchDistance);
            auto examine = SG_PATHFIND_BFS(false, false)(arena, OnGrid, startPoint, searchDistance, visited, directions, endPoint);
            Utils::FlowfieldToPath(out, examine, startPoint, endPoint, visited);
        }
        if constexpr (type == 1)   // 1 = Flowfield
            SG_PATHFIND_BFS(true, false)(arena, OnGrid, startPoint, searchDistance, out, directions);
        if constexpr (type == 2)   // 2 = Distance Matrix
            SG_PATHFIND_BFS(true, true)(arena, OnGrid, startPoint, searchDistance, out, directions);
        if constexpr (type == 4) { // 4 = Flowfield + Distance
            // (TODO!!!!) using std::pair as out type
        }
        arena.sublifetime_rollback();
        return out;
    }
}


export namespace SG_Pathfind::BFS {
    template<bool queensCase = true, bool useSTD = false>
    auto& BFS_Point(Utils::Path_c auto& out, SG_Allocator::BaseArena_c<char, char> auto& arena, const SG_Grid::ReadableGrid_c auto& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::Point& endPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        return BFS_Pathfind<0, queensCase, useSTD>(out, arena, OnGrid, startPoint, searchDistance, endPoint);
    }
    
    template<bool queensCase = true, bool useSTD = false>
    auto& BFS_Flowfield(Utils::Flowfield_c auto& out, SG_Allocator::BaseArena_c<char, char> auto& arena, const SG_Grid::ReadableGrid_c auto& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        return BFS_Pathfind<1, queensCase, useSTD>(out, arena, OnGrid, startPoint, searchDistance);
    }
    
    template<bool queensCase = true, bool useSTD = false>
    auto& BFS_Dmatrix(Utils::Dmat_c auto& out, SG_Allocator::BaseArena_c<char, char> auto& arena, const SG_Grid::ReadableGrid_c auto& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        return BFS_Pathfind<2, queensCase, useSTD>(out, arena, OnGrid, startPoint, searchDistance);
    }
    

    namespace Bench{
        template<bool queensCase = true, const SG_Grid::u_coordinate_t maxOutputNodes = 256, bool useSTD = false>
        auto& BFS_Point_(const SG_Grid::ReadableGrid_c auto& OnGrid, SG_Allocator::BaseArena_c<char, char> auto& arena, SG_Allocator::BaseArena_c<char, char> auto& outArena, const SG_Grid::Point& startPoint, const SG_Grid::Point& endPoint, const SG_Grid::u_coordinate_t& searchDistance) {
            auto& out(*(outArena.template allocConstruct<Utils::defaultPath_t<maxOutputNodes>>()));
            return BFS_Point<queensCase, useSTD>(out, arena, OnGrid, startPoint, endPoint, searchDistance);
        }
        
        template<bool queensCase = true, bool useSTD = false>
        auto& BFS_Flowfield_(const SG_Grid::ReadableGrid_c auto& OnGrid, SG_Allocator::BaseArena_c<char, char> auto& arena, SG_Allocator::BaseArena_c<char, char> auto& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
            auto& out(*outArena.template allocConstruct<Utils::defaultFlowfield_t<std::remove_reference_t<decltype(outArena)>, useSTD>>(outArena, OnGrid, startPoint, searchDistance));
            return BFS_Flowfield<queensCase, useSTD>(out, arena, OnGrid, startPoint, searchDistance);
        }
        
        template<bool queensCase = true, bool useSTD = false>
        auto& BFS_Dmatrix_(const SG_Grid::ReadableGrid_c auto& OnGrid, SG_Allocator::BaseArena_c<char, char> auto& arena, SG_Allocator::BaseArena_c<char, char> auto& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
            auto& out(*outArena.template allocConstruct<Utils::defaultDmatrix_t<std::remove_reference_t<decltype(outArena)>, useSTD>>(outArena, OnGrid, startPoint, searchDistance)); //Should map a Point to a distance -> Distance Matrix
            return BFS_Dmatrix<queensCase, useSTD>(out, arena, OnGrid, startPoint, searchDistance);
        }
    }

    //TODO make new function that uses an std::pair of u_coordinate_t (distance) AND Point (flow) -> make it so you only need one call instead of 2
    //TODO add 'inRange' variants -> Takes additional grid specifying 'ignorable' tiles for the 'withinRange' check (e.g. walls block shots, but rivers don't) -> Add options for 'ignoreAll', and make default just use all walls of the base map
}
