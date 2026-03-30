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

//TODO keep track of best heuristic tile, and path to that if we don't get the endPoint jishin

//TODO make it default to heap when non-integer Grid_t::T
namespace SG_Pathfind::AStar {
    template<bool useSTD, bool useHeap, bool tryFifo, bool useHashset, bool queensCase, bool Flowfield>
    SG_Grid::Point AStar_Base(SG_Allocator::BaseArena_c<char, char> auto& arena, const SG_Grid::ReadableGrid_c auto& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance, auto& visited, auto& flow, auto& directions, const SG_Grid::Point& endPoint_ = {0,0}){
        using arena_t = std::remove_reference_t<decltype(arena)>;
        using grid_t = std::remove_reference_t<decltype(OnGrid)>;
        typedef std::conditional_t<useSTD,
            PriorityQueue::STDPriorityQueue<arena_t, grid_t, false, tryFifo, !useHashset>,
            std::conditional_t<useHeap,
                PriorityQueue::HashMapBinaryHeap<arena_t, grid_t, false, tryFifo, !useHashset>,
                PriorityQueue::HashMapBucketQueue<arena_t, grid_t, false, tryFifo, !useHashset>>> queue_t; \

        auto weightRange = Utils::HeuristicRange<Flowfield, queensCase>(startPoint, endPoint_, searchDistance);
        queue_t frontier(arena, OnGrid, startPoint, searchDistance, weightRange.x(), weightRange.y());
        visited.insert(startPoint, 0);
        frontier.insert(startPoint,weightRange.y());
        SG_Grid::Point examine = startPoint;
        while (frontier.length() > 0) {
            examine = frontier.extractMin();
            for (auto i = 0; i < directions.length(); ++i){
                auto next = examine + directions.peekRef(i);
                if (!(next.on(OnGrid))) continue; /* Don't look past the grid bounds */
                if (OnGrid.get(next)) { /* Not a wall (0 = wall)*/
                    SG_Grid::u_coordinate_t nextPriority;
                    if (visited.get(examine) + OnGrid.get(next) > searchDistance) continue;
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

#define SG_PATHFIND_AStar(Flowfield) AStar_Base<useSTD, useHeap, tryFifo, useHashset, queensCase, Flowfield>


namespace SG_Pathfind::AStar {
    template<std::uint8_t type, bool queensCase = true, bool useSTD = false, bool useHeap = false, bool tryFifo = false, bool useHashset = true>
    inline auto&  AStar_Pathfind(auto& out, SG_Allocator::BaseArena_c<char, char> auto& arena, const SG_Grid::ReadableGrid_c auto& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance, const SG_Grid::Point& endPoint = {0,0}) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
        using arena_t = std::remove_reference_t<decltype(arena)>;
        if constexpr (type == 0) {LOGGER_ASSERT_EXCEPT(endPoint.on(OnGrid));}
        auto& directions = Utils::AvailableMoves<queensCase>();

        arena.sublifetime_open();
        if constexpr (type == 0) { // 0 = P2P
            Utils::defaultFlowfield_t<arena_t, useSTD> flow(arena, OnGrid, startPoint, searchDistance);
            Utils::defaultDmatrix_t  <arena_t, useSTD> visited(arena, OnGrid, startPoint, searchDistance);
            auto examine = SG_PATHFIND_AStar(false)(arena, OnGrid, startPoint, searchDistance, visited, flow, directions, endPoint);
            Utils::FlowfieldToPath(out, examine, startPoint, endPoint, flow);
        }
        if constexpr (type == 1) {
            // 1 = Flowfield
            Utils::defaultDmatrix_t<arena_t, useSTD> visited(arena, OnGrid, startPoint, searchDistance);
            auto examine = SG_PATHFIND_AStar(false)(arena, OnGrid, startPoint, searchDistance, visited, out, directions);
        }
        if constexpr (type == 2) {
            // 2 = Distance Matrix
            Utils::defaultFlowfield_t <arena_t, useSTD> flow(arena, OnGrid, startPoint, searchDistance);
            auto examine = SG_PATHFIND_AStar(false)(arena, OnGrid, startPoint, searchDistance, out, flow, directions);
        }
        if constexpr (type == 4) { // 4 = Flowfield + Distance
            // (TODO!!!!) using std::pair as out type
        }
        arena.sublifetime_rollback();
        return out;
    }
}


export namespace SG_Pathfind::AStar {
    template<bool queensCase = true, bool useSTD = false, bool useHeap = false, bool tryFifo = false, bool useHashset = true>
    auto& AStar_Point(Utils::Path_c auto& out, SG_Allocator::BaseArena_c<char, char> auto& arena, const SG_Grid::ReadableGrid_c auto& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::Point& endPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        return AStar_Pathfind<0, queensCase, useSTD, useHeap, tryFifo, useHashset>(out, arena, OnGrid, startPoint, searchDistance, endPoint);
    }

    template<bool queensCase = true, bool useSTD = false, bool useHeap = false, bool tryFifo = false, bool useHashset = true>
    auto& Dijkstra_Flowfield(Utils::Flowfield_c auto& out, SG_Allocator::BaseArena_c<char, char> auto& arena, const SG_Grid::ReadableGrid_c auto& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        return AStar_Pathfind<1, queensCase, useSTD, useHeap, tryFifo, useHashset>(out, arena, OnGrid, startPoint, searchDistance);
    }

    template<bool queensCase = true, bool useSTD = false, bool useHeap = false, bool tryFifo = false, bool useHashset = true>
    auto& Dijkstra_Dmatrix(Utils::Dmat_c auto& out, SG_Allocator::BaseArena_c<char, char> auto& arena, const SG_Grid::ReadableGrid_c auto& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        return AStar_Pathfind<2, queensCase, useSTD, useHeap, tryFifo, useHashset>(out, arena, OnGrid, startPoint, searchDistance);
    }


    namespace Bench{
        template<const SG_Grid::u_coordinate_t maxOutputNodes = 256, bool queensCase = true, bool useSTD = false, bool useHeap = false, bool tryFifo = false, bool useHashset = true>
        auto& AStar_Point_(const SG_Grid::ReadableGrid_c auto& OnGrid, SG_Allocator::BaseArena_c<char, char> auto& arena, SG_Allocator::BaseArena_c<char, char> auto& outArena, const SG_Grid::Point& startPoint, const SG_Grid::Point& endPoint, const SG_Grid::u_coordinate_t& searchDistance) {
            auto& out(*(outArena.template allocConstruct<Utils::defaultPath_t<maxOutputNodes>>()));
            return AStar_Point<queensCase, useSTD, useHeap, tryFifo, useHashset>(out, arena, OnGrid, startPoint, endPoint, searchDistance);
        }

        template<bool queensCase = true, bool useSTD = false, bool useHeap = false, bool tryFifo = false, bool useHashset = true>
        auto& Dijkstra_Flowfield_(const SG_Grid::ReadableGrid_c auto& OnGrid, SG_Allocator::BaseArena_c<char, char> auto& arena, SG_Allocator::BaseArena_c<char, char> auto& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
            auto& out(*outArena.template allocConstruct<Utils::defaultFlowfield_t<std::remove_reference_t<decltype(outArena)>, useSTD>>(outArena, OnGrid, startPoint, searchDistance));
            return Dijkstra_Flowfield<queensCase, useSTD, useHeap, tryFifo, useHashset>(out, arena, OnGrid, startPoint, searchDistance);
        }

        template<bool queensCase = true, bool useSTD = false, bool useHeap = false, bool tryFifo = false, bool useHashset = true>
        auto& Dijkstra_Dmatrix_(const SG_Grid::ReadableGrid_c auto& OnGrid, SG_Allocator::BaseArena_c<char, char> auto& arena, SG_Allocator::BaseArena_c<char, char> auto& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
            auto& out(*outArena.template allocConstruct<Utils::defaultDmatrix_t<std::remove_reference_t<decltype(outArena)>, useSTD>>(outArena, OnGrid, startPoint, searchDistance)); //Should map a Point to a distance -> Distance Matrix
            return Dijkstra_Dmatrix<queensCase, useSTD, useHeap, tryFifo, useHashset>(out, arena, OnGrid, startPoint, searchDistance);
        }
    }
}
