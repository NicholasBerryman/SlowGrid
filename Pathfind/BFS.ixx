//
// Created by nickberryman on 10/12/25.
//
module;
#include <type_traits>
#include <algorithm>
#include "Logger.h"

#include <iostream>

export module SG_Pathfind:BFS;
import LocalDataStructures;
import Logger;
import SG_Allocator;
import SG_Grid;
import :Utils;
import :GridRangeHashMap;
import :NoPriorityQueue;

//TODO convert this into a function that uses references (in Utils.ixx)
#define SG_PATHFIND_BFS(Flowfield, Distances) \
    PriorityQueue::NoPriorityQueue<WorkingArenaType, Grid_t, queensCase, !Distances, false, false> frontier(arena, OnGrid, startPoint, searchDistance); \
    if constexpr (Distances) visited.insert(startPoint, 0); \
    frontier.insert(startPoint); \
    SG_Grid::Point examine = startPoint; \
    if constexpr (!Distances) visited.insert(startPoint, startPoint); \
    else visited.insert(startPoint, 0); \
    SG_Grid::u_coordinate_t nextDistance = !Distances; \
    frontier.trySwap(); \
    while (frontier.length() > 0) { \
        examine = frontier.extractMin(); \
        if constexpr (Distances){ nextDistance = visited.get(examine)+1; } \
        if (nextDistance > searchDistance) break; \
        for (auto i = 0; i < directions.length(); ++i){ \
            auto next = examine + directions.peekRef(i); \
            if (!(next.on(OnGrid))) continue; /* Don't look past the grid bounds */ \
            if (OnGrid.get(next)) { /* Not a wall (0 = wall)*/ \
                bool alreadyChecked;  \
                if (!visited.contains(next)) { \
                    frontier.insert(next); \
                    if constexpr (Distances) visited.insert(next, nextDistance); \
                    else visited.insert(next, directions.peekRef(i)); \
                } \
            } \
            if constexpr (!Flowfield) { if (next == endPoint_) { examine = next; goto found; } } /* We hit the end point - nice! */ \
        } \
        if constexpr (!Distances) { if (frontier.trySwap()) ++nextDistance; } \
    } \
    found: /* The last remaining use for goto :) */

export namespace SG_Pathfind::BFS {
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, typename HashMap_t, bool queensCase = true, const SG_Grid::u_coordinate_t maxOutputNodes = 256>//TODO make a default number for this in a config file
    LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>&  BFS_Point(const Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::Point& endPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
        LOGGER_ASSERT_EXCEPT(endPoint.on(OnGrid));
        auto directions = Utils::AvailableMoves<queensCase>();
        auto& out(*(outArena.template allocConstruct<LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>>()));
        const auto& endPoint_(endPoint); //Needs to be defined for the macro, even though we don't actually use it -> Blame how constexpr ifs evaluate

        arena.sublifetime_open();
        HashMap_t visited(arena, OnGrid, startPoint, searchDistance); //Should map a Point to a direction -> Flowfield
        SG_PATHFIND_BFS(false, false);
        Utils::FlowfieldToPath(out, examine, startPoint, endPoint, visited);
        arena.sublifetime_rollback();
        return out;
    }
    
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, typename HashMap_t, bool queensCase = true>
    HashMap_t&  BFS_Flowfield(Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
        auto directions = SG_Pathfind::Utils::AvailableMoves<queensCase>();
        const SG_Grid::Point& endPoint_ = {0,0}; //Needs to be defined for the macro, even though we don't actually use it -> Blame how constexpr ifs evaluate
        HashMap_t& visited(*outArena.template allocConstruct<HashMap_t>(outArena, OnGrid, startPoint, searchDistance)); //Should map a Point to a Point -> Flowfield

        arena.sublifetime_open();
        SG_PATHFIND_BFS(true, false);
        
        arena.sublifetime_rollback();
        return visited;
    }
    
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, bool queensCase = true>
    auto& BFS_Dmatrix(Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
        auto directions = SG_Pathfind::Utils::AvailableMoves<queensCase>();
        const SG_Grid::Point& endPoint_ = {0,0}; //Needs to be defined for the macro, even though we don't actually use it -> Blame how constexpr ifs evaluate
        auto& visited(*outArena.template allocConstruct<HashMap::GridRangeHashMap<OutputArenaType, SG_Grid::u_coordinate_t>>(outArena, OnGrid, startPoint, searchDistance)); //Should map a Point to a distance -> Distance Matrix

        arena.sublifetime_open();
        SG_PATHFIND_BFS(true, true);
        arena.sublifetime_rollback();
        return visited;
    }
}

