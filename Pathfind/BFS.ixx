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
import :STDHashMap;
import :NoPriorityQueue;
import :STDNoPriorityQueue;


namespace SG_Pathfind::BFS {
    template<bool useSTD, bool queensCase, bool Flowfield, bool Distances, typename WorkingArenaType, typename Grid_t>
    SG_Grid::Point BFS_Base(WorkingArenaType& arena, const Grid_t& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance, auto& visited, auto& directions, const SG_Grid::Point& endPoint_ = {0,0}){
        typedef std::conditional_t<useSTD, PriorityQueue::STDNoPriorityQueue<WorkingArenaType, Grid_t, queensCase, !Distances, false, false>, PriorityQueue::NoPriorityQueue<WorkingArenaType, Grid_t, queensCase, !Distances, false, false>> queue_t; \
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
}

#define SG_PATHFIND_BFS(Flowfield, Distances) BFS_Base<useSTD, queensCase, Flowfield, Distances, WorkingArenaType, Grid_t>


export namespace SG_Pathfind::BFS {
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, bool queensCase = true, const SG_Grid::u_coordinate_t maxOutputNodes = 256, bool useSTD = false> //TODO make a default number for maxOutputNodes in a config file
    LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>&  BFS_Point(const Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::Point& endPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
        LOGGER_ASSERT_EXCEPT(endPoint.on(OnGrid));
        auto& directions = Utils::AvailableMoves<queensCase>();
        typedef std::conditional_t<useSTD, HashMap::STDHashMap<WorkingArenaType,SG_Grid::Point>, HashMap::GridRangeHashMap<WorkingArenaType,SG_Grid::Point>> visited_t;
        auto& out(*(outArena.template allocConstruct<LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>>()));

        arena.sublifetime_open();
        visited_t visited(arena, OnGrid, startPoint, searchDistance); //Should map a Point to a direction -> Flowfield
        auto examine = SG_PATHFIND_BFS(false, false)(arena, OnGrid, startPoint, searchDistance, visited, directions, endPoint);
        Utils::FlowfieldToPath(out, examine, startPoint, endPoint, visited);
        arena.sublifetime_rollback();
        return out;
    }
    
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, bool queensCase = true, bool useSTD = false>
    auto&  BFS_Flowfield(Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
        auto& directions = Utils::AvailableMoves<queensCase>();
        typedef std::conditional_t<useSTD, HashMap::STDHashMap<OutputArenaType,SG_Grid::Point>, HashMap::GridRangeHashMap<OutputArenaType,SG_Grid::Point>> visited_t;
        auto& visited(*outArena.template allocConstruct<visited_t>(outArena, OnGrid, startPoint, searchDistance)); //Should map a Point to a Point (x/y direction) -> Flowfield

        arena.sublifetime_open();
        SG_PATHFIND_BFS(true, false)(arena, OnGrid, startPoint, searchDistance, visited, directions);
        arena.sublifetime_rollback();
        return visited;
    }
    
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, bool queensCase = true, bool useSTD = false>
    auto& BFS_Dmatrix(Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
        auto& directions = Utils::AvailableMoves<queensCase>();
        typedef std::conditional_t<useSTD, HashMap::STDHashMap<OutputArenaType,SG_Grid::u_coordinate_t>, HashMap::GridRangeHashMap<OutputArenaType,SG_Grid::u_coordinate_t>> visited_t;
        auto& visited(*outArena.template allocConstruct<visited_t>(outArena, OnGrid, startPoint, searchDistance)); //Should map a Point to a distance -> Distance Matrix

        arena.sublifetime_open();
        SG_PATHFIND_BFS(true, true)(arena, OnGrid, startPoint, searchDistance, visited, directions);
        arena.sublifetime_rollback();
        return visited;
    }

    //TODO make new function that uses an std::pair of u_coordinate_t (distance) AND Point (flow) -> make it so you only need one call instead of 2

    //TODO add 'inRange' variants -> Takes additional grid specifying 'ignorable' tiles for the 'withinRange' check (e.g. walls block shots, but rivers don't) -> Add options for 'ignoreAll', and make default just use all walls of the base map

    //TODO add variants that somehow work the same but use a pre-allocated output matrix/stack -> reduce need to copy and also reduce chance of OutputArena fragmentation
}
