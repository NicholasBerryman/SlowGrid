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

#define SG_PATHFIND_BFS(Flowfield, Distances) \
    SG_Allocator::LinkedList<WorkingArenaType, SG_Grid::Point, SG_Grid::u_coordinate_t> frontier(arena); /* TODO replace with runtime-sized queue */ \
    std::conditional_t<Distances, queuefiller<WorkingArenaType>, decltype(frontier)> frontier2(arena); \
    frontier.construct_back(startPoint); \
    SG_Grid::Point examine = startPoint; \
    visited.insert(startPoint, startPoint); \
    if constexpr (Distances) Dmat.insert(startPoint, 0); \
    SG_Grid::u_coordinate_t nextDistance = !Distances; \
    bool bufferSwap = false; \
    while (frontier.length() > 0 || frontier2.length() > 0){ \
        if constexpr (!Distances){ \
            if (!bufferSwap && frontier.length() == 0) {bufferSwap = true; ++nextDistance;} \
            if (bufferSwap && frontier2.length() == 0) {bufferSwap = false; ++nextDistance;} \
        } \
        if (!bufferSwap) { \
            examine = frontier.get_fromFront(0); \
            frontier.remove_front(); \
        } else { \
            examine = frontier2.get_fromFront(0); \
            frontier2.remove_front(); \
        } \
        if constexpr (Distances){ nextDistance = Dmat.get(examine)+1; } \
        /*std::cout << examine.x() << "," << examine.y() << " | " << frontier.length() << " | " << nextDistance << std::endl;*/ \
        if (nextDistance > searchDistance) break;\
        for (auto i = 0; i < directions.length(); ++i){ \
            auto next = examine + directions.peekRef(i); \
            if (!(next.on(OnGrid))) continue; /* Don't look past the grid bounds */ \
            if (OnGrid.get(next) && !visited.contains(next)) { /* Not a wall (0 = wall), and not visited */ \
                if constexpr (!Distances){ \
                    if (bufferSwap) frontier.construct_back(next); \
                    else frontier2.construct_back(next); \
                } else frontier.construct_back(next); \
                visited.insert(next, directions.peekRef(i)); \
                if constexpr (Distances) Dmat.insert(next, nextDistance); \
            } \
            if constexpr (!Flowfield) { if (next == endPoint_) {examine = next; goto found;} } /* We hit the end point - nice! */ \
        } \
    } \
    found: /* The last remaning use for goto :) */ 

template<typename A>
struct queuefiller{
    queuefiller(const A&){}
    SG_Grid::Point get_fromFront(const SG_Grid::u_coordinate_t& i){return SG_Grid::Point(0,0);}
    void remove_front(){}
    void construct_back(const SG_Grid::Point& p){}
    SG_Grid::u_coordinate_t length(){return 0;}
};

struct dmatfiller{
    void insert(SG_Grid::Point a, SG_Grid::u_coordinate_t b){}
    SG_Grid::u_coordinate_t get(SG_Grid::Point a){return 0;}
};

export namespace SG_Pathfind::BFS {
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, typename HashMap_t, bool queensCase = true, const SG_Grid::u_coordinate_t maxOutputNodes = 256>//TODO make a default number for this in a config file
    LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>&  BFS_Point(Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::Point& endPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        LOGGER_ASSERT_EXCEPT(startPoint.on(OnGrid));
        LOGGER_ASSERT_EXCEPT(endPoint.on(OnGrid));
        auto directions = Utils::AvailableMoves<queensCase>();
        auto& out(*(outArena.template allocConstruct<LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>>()));
        const auto& endPoint_(endPoint); //Needs to be defined for the macro, even though we don't actually use it -> Blame how constexpr ifs evaluate
        dmatfiller Dmat;
        
        arena.sublifetime_open();
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
        dmatfiller Dmat;
        
        HashMap_t& visited(*outArena.template allocConstruct<HashMap_t>(outArena, OnGrid, startPoint, searchDistance)); //Should map a Point to a Point -> Flowfield
            //Can use the 'contains' method to check if a point is reachable from the startPoint

        arena.sublifetime_open();
        SG_PATHFIND_BFS(true, false); //TODO fix to work with distance false so we don't need Dmat anymore;
        
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
}

