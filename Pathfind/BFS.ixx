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


#define SG_PATHFIND_BFS(Flowfield, Distances) BFS_Base<useSTD, queensCase, Flowfield, Distances, WorkingArenaType, Grid_t>

namespace SG_Pathfind::BFS {
    template<bool useSTD, bool queensCase, bool Flowfield, bool Distances, typename WorkingArenaType, typename Grid_t>
    inline SG_Grid::Point BFS_Base(WorkingArenaType& arena, const Grid_t& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance, auto& visited, auto& directions, const SG_Grid::Point& endPoint_ = {0,0}){
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
    
    template<std::uint8_t type, typename WorkingArenaType, typename Grid_t, bool queensCase = true, bool useSTD = false> //TODO make a default number for maxOutputNodes in a config file
    inline auto&  BFS_Pathfind(auto& out, WorkingArenaType& arena, const Grid_t& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance, const SG_Grid::Point& endPoint = {0,0}) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
         if constexpr (type == 0) {LOGGER_ASSERT_EXCEPT(endPoint.on(OnGrid));}
        auto& directions = Utils::AvailableMoves<queensCase>();

        arena.sublifetime_open();
        if constexpr (type == 0) { // 0 = P2P
            Utils::Flowfield_t<WorkingArenaType, useSTD> visited(arena, OnGrid, startPoint, searchDistance);
            auto examine = SG_PATHFIND_BFS(false, false)(arena, OnGrid, startPoint, searchDistance, visited, directions, endPoint);
            Utils::FlowfieldToPath(out, examine, startPoint, endPoint, visited);
        }
        if constexpr (type == 1)   // 1 = Flowfield
            SG_PATHFIND_BFS(true, false)(arena, OnGrid, startPoint, searchDistance, out, directions);
        if constexpr (type == 2)   // 2 = Distance Matrix
            SG_PATHFIND_BFS(true, true)(arena, OnGrid, startPoint, searchDistance, out, directions);
        if constexpr (type == 4) { // 4 = Flowfield + Distance
            // (TODO!!!!)
        }
        arena.sublifetime_rollback();
        return out;
    }
}


export namespace SG_Pathfind::BFS {
    template<typename WorkingArenaType, typename Grid_t, bool queensCase = true, const SG_Grid::u_coordinate_t maxOutputNodes = 256, bool useSTD = false> //TODO make a default number for maxOutputNodes in a config file
    auto&  BFS_Point(Utils::Path_t<maxOutputNodes>& out, WorkingArenaType& arena, const Grid_t& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::Point& endPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        return BFS_Pathfind<0, WorkingArenaType, Grid_t, queensCase, useSTD>(out, arena, OnGrid, startPoint, searchDistance, endPoint);
    }
    
    template<typename OutputArenaType, typename WorkingArenaType, typename Grid_t, bool queensCase = true, bool useSTD = false>
    auto&  BFS_Flowfield(Utils::Flowfield_t<OutputArenaType, useSTD>& out, WorkingArenaType& arena, const Grid_t& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        return BFS_Pathfind<1, WorkingArenaType, Grid_t, queensCase, useSTD>(out, arena, OnGrid, startPoint, searchDistance);
    }
    
    template<typename OutputArenaType, typename WorkingArenaType, typename Grid_t, bool queensCase = true, bool useSTD = false>
    auto&  BFS_Dmatrix(Utils::Dmat_t<OutputArenaType, useSTD>& out, WorkingArenaType& arena, const Grid_t& OnGrid, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        return BFS_Pathfind<2, WorkingArenaType, Grid_t, queensCase, useSTD>(out, arena, OnGrid, startPoint, searchDistance);
    }
    
    
    
    namespace Bench{
        template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, bool queensCase = true, const SG_Grid::u_coordinate_t maxOutputNodes = 256, bool useSTD = false> //TODO make a default number for maxOutputNodes in a config file
        auto&  BFS_Point_(const Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::Point& endPoint, const SG_Grid::u_coordinate_t& searchDistance) {
            auto& out(*(outArena.template allocConstruct<Utils::Path_t<maxOutputNodes>>()));
            return BFS_Point<WorkingArenaType, Grid_t, queensCase, maxOutputNodes, useSTD>(out, arena, OnGrid, startPoint, endPoint, searchDistance);
        }
        
        template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, bool queensCase = true, bool useSTD = false>
        auto&  BFS_Flowfield_(Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
            auto& out(*outArena.template allocConstruct<Utils::Flowfield_t<OutputArenaType, useSTD>>(outArena, OnGrid, startPoint, searchDistance));
            return BFS_Flowfield<OutputArenaType, WorkingArenaType, Grid_t, queensCase, useSTD>(out, arena, OnGrid, startPoint, searchDistance);
        }
        
        template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, bool queensCase = true, bool useSTD = false>
        auto& BFS_Dmatrix_(Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
            auto& out(*outArena.template allocConstruct<Utils::Dmat_t<OutputArenaType, useSTD>>(outArena, OnGrid, startPoint, searchDistance)); //Should map a Point to a distance -> Distance Matrix
            return BFS_Dmatrix<OutputArenaType, WorkingArenaType, Grid_t, queensCase, useSTD>(out, arena, OnGrid, startPoint, searchDistance);
        }
    }

    //TODO make new function that uses an std::pair of u_coordinate_t (distance) AND Point (flow) -> make it so you only need one call instead of 2
    //TODO add 'inRange' variants -> Takes additional grid specifying 'ignorable' tiles for the 'withinRange' check (e.g. walls block shots, but rivers don't) -> Add options for 'ignoreAll', and make default just use all walls of the base map
    //TODO add variants that somehow work the same but use a pre-allocated output matrix/stack -> reduce need to copy and also reduce chance of OutputArena fragmentation
    
    
    //TODO templates are getting too much!!!
        //TODO 'almost always use auto!'
            // Use 'auto' with 'concepts' and clean up all the templates in this project!!
}
