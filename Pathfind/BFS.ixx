//
// Created by nickberryman on 10/12/25.
//
module;

export module SG_Pathfind:BFS;
//import LocalDataStructures;
import SG_Allocator;
import SG_Grid;

export namespace SG_Pathfind::BFS {
/*
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, typename HashMap_t, const SG_Grid::u_coordinate_t maxOutputNodes = 256>//TODO make a default number for this in a config file
    LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>&  BFS_Point(const Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& distance) {
        auto& out(*outArena.template allocConstruct<LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>>(outArena, maxOutputNodes));

        arena.sublifetime_open();
        HashMap_t visited(arena, OnGrid, startPoint, distance);
        //TODO algorithm
        arena.sublifetime_rollback();

        //TODO post-process final path to be in-order stack of corner points starting at the end point and ending at the start point -> only need to track changes of direction and distance of current direction, not all points in path

        return out;
    }

    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, typename HashMap_t, const SG_Grid::u_coordinate_t maxOutputNodes = 256>//TODO make a default number for this in a config file
    HashMap_t&  BFS_Flowfield(const Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& distance) {

        HashMap_t& visited(*outArena.template allocConstruct<HashMap_t>(outArena, OnGrid, startPoint, distance));

        arena.sublifetime_open();
        //TODO macro from Point2Point function
        arena.sublifetime_rollback();
        return visited;
    }
*/
}
