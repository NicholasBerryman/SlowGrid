//
// Created by nickberryman on 10/12/25.
//
module;
#include <type_traits>
#include <algorithm>
#include "Logger.h"

#include <iostream>

export module SG_Pathfind:AStar;
import LocalDataStructures;
import Logger;
import SG_Allocator;
import SG_Grid;
import :Utils;
import :GridRangeHashMap;
import :STDHashMap;
import :HashMapBucketQueue;
import :HashMapBinaryHeap;
import :STDPriorityQueue;

//TODO make it default to heap when non-integer Grid_t::T
namespace SG_Pathfind::AStar {
    template<bool useSTD, bool useHeap, bool tryFifo, bool useHashset, bool queensCase, bool Flowfield, typename WorkingArenaType, typename Grid_t>
    SG_Grid::Point AStar_Base(WorkingArenaType& arena, const Grid_t& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance, auto& visited, auto& flow, auto& directions, const SG_Grid::Point& endPoint_ = {0,0}){
        typedef std::conditional_t<useSTD,
            PriorityQueue::STDPriorityQueue<WorkingArenaType, Grid_t, false, tryFifo, !useHashset>,
            std::conditional_t<useHeap,
                PriorityQueue::HashMapBinaryHeap<WorkingArenaType, Grid_t, false, tryFifo, !useHashset>,
                PriorityQueue::HashMapBucketQueue<WorkingArenaType, Grid_t, true, tryFifo, !useHashset>>> queue_t; \

        SG_Grid::u_coordinate_t pmax;
        SG_Grid::u_coordinate_t pmin;
        if constexpr (!Flowfield) {
            if constexpr (queensCase) {
                pmax = SG_Grid::Distance::Chebyshev(startPoint, endPoint_) + searchDistance + searchDistance;
                pmin = SG_Grid::Distance::Chebyshev(startPoint, endPoint_);
            } else {
                pmax = SG_Grid::Distance::Manhattan(startPoint, endPoint_) + searchDistance + searchDistance;
                pmin = SG_Grid::Distance::Manhattan(startPoint, endPoint_);
            }
        } else {
            pmax = searchDistance;
            pmin = 0;
        }

        queue_t frontier(arena, OnGrid, startPoint, searchDistance, pmax, pmin);
        visited.insert(startPoint, 0);
        frontier.insert(startPoint,pmin);
        SG_Grid::Point examine = startPoint;
        while (frontier.length() > 0) {
            examine = frontier.extractMin();
            for (auto i = 0; i < directions.length(); ++i){
                auto next = examine + directions.peekRef(i);
                if (!(next.on(OnGrid))) continue; /* Don't look past the grid bounds */
                if (OnGrid.get(next)) { /* Not a wall (0 = wall)*/
                    SG_Grid::u_coordinate_t nextPriority;
                    if (visited.get(examine) + OnGrid.get(next) > searchDistance) continue; //TODO try and use this to get 'close enough' paths when we run out of range -> maybe just goto from here??
                    if constexpr (Flowfield) nextPriority = visited.get(examine) + OnGrid.get(next);
                    else if constexpr (queensCase)  nextPriority = visited.get(examine) + OnGrid.get(next) + SG_Grid::Distance::Chebyshev(next, endPoint_);
                    else if constexpr (!queensCase) nextPriority = visited.get(examine) + OnGrid.get(next) + SG_Grid::Distance::Manhattan(next, endPoint_);
                    auto lastP = 0;
                    if (visited.contains(next)) lastP = visited.get(next);
                    if (frontier.insert(next,nextPriority,lastP)) {
                        visited.insert(next, visited.get(examine) + OnGrid.get(next));
                        flow.insert(next, directions.peekRef(i));
                    }
                }
                if constexpr (!Flowfield) { if (next == endPoint_) { examine = next; goto found; } } /* We hit the end point - nice! */
            }
        }
        found: /* The last remaining use for goto :) */
        return examine;
    }
}

#define SG_PATHFIND_AStar(Flowfield) AStar_Base<useSTD, useHeap, tryFifo, useHashset, queensCase, Flowfield, WorkingArenaType, Grid_t>


export namespace SG_Pathfind::AStar {
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, bool queensCase = true, const SG_Grid::u_coordinate_t maxOutputNodes = 256, bool useSTD = false, bool useHeap = false, bool tryFifo = false, bool useHashset = true> //TODO make a default number for maxOutputNodes in a config file
    LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>&  AStar_Point(const Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::Point& endPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
        LOGGER_ASSERT_EXCEPT(endPoint.on(OnGrid));
        auto& directions = Utils::AvailableMoves<queensCase>();
        typedef std::conditional_t<useSTD, HashMap::STDHashMap<WorkingArenaType,SG_Grid::u_coordinate_t>, HashMap::GridRangeHashMap<WorkingArenaType,SG_Grid::u_coordinate_t>> visited_t;
        typedef std::conditional_t<useSTD, HashMap::STDHashMap<WorkingArenaType,SG_Grid::Point>, HashMap::GridRangeHashMap<WorkingArenaType,SG_Grid::Point, false>> flow_t;
        auto& out(*(outArena.template allocConstruct<LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>>()));

        arena.sublifetime_open();
        visited_t visited(arena, OnGrid, startPoint, searchDistance); //Should map a Point to a direction -> Flowfield
        flow_t flow(arena, OnGrid, startPoint, searchDistance); //Should map a Point to a direction -> Flowfield
        auto examine = SG_PATHFIND_AStar(false)(arena, OnGrid, startPoint, searchDistance, visited, flow, directions, endPoint);
        Utils::FlowfieldToPath(out, examine, startPoint, endPoint, flow);
        arena.sublifetime_rollback();
        return out;
    }
    
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, bool queensCase = true, bool useSTD = false, bool useHeap = false, bool tryFifo = false, bool useHashset = true>
    auto&  Dijkstra_Flowfield(Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
        auto& directions = Utils::AvailableMoves<queensCase>();
        typedef std::conditional_t<useSTD, HashMap::STDHashMap<OutputArenaType,SG_Grid::u_coordinate_t>, HashMap::GridRangeHashMap<OutputArenaType,SG_Grid::u_coordinate_t>> visited_t;
        typedef std::conditional_t<useSTD, HashMap::STDHashMap<WorkingArenaType,SG_Grid::Point>, HashMap::GridRangeHashMap<WorkingArenaType,SG_Grid::Point, true>> flow_t;
        auto& flow(*outArena.template allocConstruct<flow_t>(outArena, OnGrid, startPoint, searchDistance)); //Should map a Point to a Point (x/y direction) -> Flowfield

        arena.sublifetime_open();
        visited_t visited(arena, OnGrid, startPoint, searchDistance); //Should map a Point to a direction -> Flowfield
        SG_PATHFIND_AStar(true)(arena, OnGrid, startPoint, searchDistance, visited, flow, directions);
        arena.sublifetime_rollback();
        return flow;
    }
    
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, bool queensCase = true, bool useSTD = false, bool useHeap = false, bool tryFifo = false, bool useHashset = true>
    auto& Dijkstra_Dmatrix(Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
        auto& directions = Utils::AvailableMoves<queensCase>();
        typedef std::conditional_t<useSTD, HashMap::STDHashMap<OutputArenaType,SG_Grid::u_coordinate_t>, HashMap::GridRangeHashMap<OutputArenaType,SG_Grid::u_coordinate_t>> visited_t;
        typedef std::conditional_t<useSTD, HashMap::STDHashMap<WorkingArenaType,SG_Grid::Point>, HashMap::GridRangeHashMap<WorkingArenaType,SG_Grid::Point, false>> flow_t;
        auto& visited(*outArena.template allocConstruct<visited_t>(outArena, OnGrid, startPoint, searchDistance)); //Should map a Point to a distance -> Distance Matrix

        arena.sublifetime_open();
        flow_t flow(arena, OnGrid, startPoint, searchDistance); //Should map a Point to a direction -> Flowfield
        SG_PATHFIND_AStar(true)(arena, OnGrid, startPoint, searchDistance, visited, flow, directions);
        arena.sublifetime_rollback();
        return visited;
    }
}
