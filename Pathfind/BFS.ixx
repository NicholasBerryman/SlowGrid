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

//TODO make some const grid get functions and use them to make OnGrid a const reference

//TODO optimise by removing the distance matrix unless it's the specific distance matrix function
    // Use a double-buffer frontier -> current frontier and next frontier
    // Swap buffers when one is empty, and increment distance

#define SG_PATHFIND_BFS(Flowfield, Distances) \
    SG_Allocator::LinkedList<WorkingArenaType, SG_Grid::Point, SG_Grid::u_coordinate_t> frontier(arena); \
    frontier.construct_back(startPoint); \
    SG_Grid::Point examine = startPoint; \
    visited.insert(startPoint, startPoint); \
    if constexpr (Distances) Dmat.insert(startPoint, 0); \
    SG_Grid::u_coordinate_t nextDistance = 0; \
    \
    while (frontier.length() > 0){ \
        examine = frontier.get_fromFront(0); \
        frontier.remove_front(); \
        nextDistance = Dmat.get(examine)+1; \
        /*std::cout << examine.x() << "," << examine.y() << " | " << frontier.length() << " | " << nextDistance << std::endl;*/ \
        if (nextDistance > searchDistance) break;\
        for (auto i = 0; i < directions.length(); ++i){ \
            auto next = examine + directions.peekRef(i); \
            if (!(next.on(OnGrid))) continue; /* Don't look past the grid bounds */ \
            if (OnGrid.get(next) && !visited.contains(next)) { /* Not a wall (0 = wall), and not visited */ \
                frontier.construct_back(next); \
                visited.insert(next, directions.peekRef(i)); \
                if constexpr (Distances) Dmat.insert(next, nextDistance); \
            } \
            if constexpr (!Flowfield) { if (next == endPoint_) {examine = next; goto found;} } /* We hit the end point - nice! */ \
        } \
    } \
    found: /* The last remaning use for goto :) */ 

export namespace SG_Pathfind::BFS {
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, typename HashMap_t, bool queensCase = true, const SG_Grid::u_coordinate_t maxOutputNodes = 256>//TODO make a default number for this in a config file
    LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>&  BFS_Point(Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::Point& endPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
        LOGGER_ASSERT_EXCEPT(endPoint.on(OnGrid));
        auto directions = Utils::AvailableMoves<queensCase>();
        auto& out(*(outArena.template allocConstruct<LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>>()));
        const auto& endPoint_(endPoint); //Needs to be defined for the macro, even though we don't actually use it -> Blame how constexpr ifs evaluate
        
        arena.sublifetime_open();
        auto& Dmat(*outArena.template allocConstruct<HashMap::GridRangeHashMap<OutputArenaType, SG_Grid::u_coordinate_t>>(outArena, OnGrid, startPoint, searchDistance));
        HashMap_t visited(arena, OnGrid, startPoint, searchDistance); //Should map a Point to a direction -> Flowfield
        SG_PATHFIND_BFS(false, false);
        Utils::FlowfieldToPath(out, examine, startPoint, endPoint, visited);
        arena.sublifetime_rollback();
        return out;
    }
    
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, typename HashMap_t, bool queensCase = true>//TODO make a default number for this in a config file
    HashMap_t&  BFS_Flowfield(Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
        auto directions = SG_Pathfind::Utils::AvailableMoves<queensCase>();
        const SG_Grid::Point& endPoint_ = {0,0}; //Needs to be defined for the macro, even though we don't actually use it -> Blame how constexpr ifs evaluate
        
        HashMap_t& visited(*outArena.template allocConstruct<HashMap_t>(outArena, OnGrid, startPoint, searchDistance)); //Should map a Point to a Point -> Flowfield
            //Can use the 'contains' method to check if a point is reachable from the startPoint

        arena.sublifetime_open();
        auto& Dmat(*outArena.template allocConstruct<HashMap::GridRangeHashMap<OutputArenaType, SG_Grid::u_coordinate_t>>(outArena, OnGrid, startPoint, searchDistance));
        SG_PATHFIND_BFS(true, false);
        
        arena.sublifetime_rollback();
        return visited;
    }
    
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, typename HashMap_t, bool queensCase = true>//TODO make a default number for this in a config file
    auto&  BFS_Dmatrix(Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
        auto directions = SG_Pathfind::Utils::AvailableMoves<queensCase>();
        const SG_Grid::Point& endPoint_ = {0,0}; //Needs to be defined for the macro, even though we don't actually use it -> Blame how constexpr ifs evaluate
        
        auto& Dmat(*outArena.template allocConstruct<HashMap::GridRangeHashMap<OutputArenaType, SG_Grid::u_coordinate_t>>(outArena, OnGrid, startPoint, searchDistance));
            //Can use the 'contains' method to check if a point is reachable from the startPoint

        arena.sublifetime_open();
        HashMap_t& visited(*outArena.template allocConstruct<HashMap_t>(outArena, OnGrid, startPoint, searchDistance)); //Should map a Point to a Point -> Flowfield
        SG_PATHFIND_BFS(true, true);
        arena.sublifetime_rollback();
        return Dmat;
    }
    
    //TODO add a distance matrix variant -> returns hashmap with distances (templatable to just be a bitfield hashmap of 'inRange/outOfRange')
}
