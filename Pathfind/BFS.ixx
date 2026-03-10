//
// Created by nickberryman on 10/12/25.
//
module;
#include <type_traits>
#include <algorithm>

export module SG_Pathfind:BFS;
import LocalDataStructures;
import SG_Allocator;
import SG_Grid;

//TODO make some cont grid get functions and use them to make OnGrid a const reference

export namespace SG_Pathfind::BFS {
    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, typename HashMap_t, bool queensCase = true, const SG_Grid::u_coordinate_t maxOutputNodes = 256>//TODO make a default number for this in a config file
    LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>&  BFS_Point(Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::Point& endPoint, const SG_Grid::u_coordinate_t& searchDistance) {
        //TODO assert that endPoint is on the grid
        
        //TODO make this stack an input parameter, with an external function used to create it with specified orderings -> Also, I don't want these being recreated every pathfinding call
        std::conditional_t<queensCase, LocalDataStructures::Stack<SG_Grid::Point,8>, LocalDataStructures::Stack<SG_Grid::Point,4>> directions;
        SG_Grid::u_coordinate_t crowDist;
        if constexpr (queensCase) { 
            directions.push({-1,-1});
            directions.push({-1, 1});
            directions.push({ 1, 1});
            directions.push({ 1,-1});
            crowDist = SG_Grid::Distance::Chebyshev(startPoint, endPoint);
        }
        else{
            LocalDataStructures::Stack<SG_Grid::Point,4> directions;
            crowDist = SG_Grid::Distance::Manhattan(startPoint, endPoint);
        }
        directions.push({ 0, 1});
        directions.push({ 0,-1});
        directions.push({ 1, 0});
        directions.push({-1, 0});
        // -- END DIRECTION STACK --
        
        auto& out(*(outArena.template allocConstruct<LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>>()));
        arena.sublifetime_open();
        HashMap_t visited(arena, OnGrid, startPoint, searchDistance); //Should map a Point to a Point -> Flowfield
        // -- algorithm --
        SG_Allocator::ULL2<WorkingArenaType, SG_Grid::Point> frontier(arena, directions.length() * std::min(crowDist * 2, searchDistance)); // ULL Blocksize based on double the crow distance -> Maybe tune this??
        frontier.push_back(startPoint);
        SG_Grid::Point examine = startPoint;
        SG_Grid::u_coordinate_t distance = 0;
        visited.insert(startPoint, startPoint);
        
        while (frontier.length() > 0 && distance <= searchDistance){
            examine = frontier.pop_back();
            for (auto i = 0; i < directions.length(); ++i){
                auto next = examine + directions.peekRef(i);
                if (next.x() < 0 || next.x() >= OnGrid.width() || next.y() < 0 || next.y() >= OnGrid.height()) continue; //Don't look past the grid bounds
                if (!OnGrid.get(examine) && !visited.contains(examine)) { //Not a wall, and not visited
                    frontier.push_back(next);
                    visited.insert(next, directions.peekRef(i));
                }
                if (next == endPoint) break; //We hit the end point - nice!
            }
        }
        // -- END algorithm -> TODO macro-ify it --
        arena.sublifetime_rollback();
        
        if (examine == endPoint){
            auto trace = endPoint;
            auto dir = SG_Grid::Point(0,0);
            while (trace != startPoint){
                auto newDir = visited.get(trace);
                if (dir != newDir) out.push(trace); //Only add to output stack if it's a corner node
                trace = trace + newDir;
            }
        }

        return out;
    }

    template<typename WorkingArenaType, typename OutputArenaType, typename Grid_t, typename HashMap_t, bool queensCase = true>//TODO make a default number for this in a config file
    HashMap_t&  BFS_Flowfield(const Grid_t& OnGrid, WorkingArenaType& arena, OutputArenaType& outArena, const SG_Grid::Point& startPoint, const SG_Grid::u_coordinate_t& searchDistance) {

        HashMap_t& visited(*outArena.template allocConstruct<HashMap_t>(outArena, OnGrid, startPoint, searchDistance)); //Should map a Point to a Point -> Flowfield

        arena.sublifetime_open();
        //TODO macro from Point2Point function
        arena.sublifetime_rollback();
        return visited;
    }
}
