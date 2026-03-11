//
// Created by nickberryman on 10/12/25.
//
module;
#include "Logger.h"

export module SG_Pathfind:Utils;
import LocalDataStructures;
import SG_Grid;

//TODO make some const grid get functions and use them to make OnGrid a const reference

export namespace SG_Pathfind::Utils {
    LocalDataStructures::Stack<SG_Grid::Point,8> QueenMoves({
        { 0,-1},
        { 1, 0},
        { 0, 1},
        {-1, 0},
        { 1,-1},
        { 1, 1},
        {-1, 1},
        {-1,-1}
    });
     LocalDataStructures::Stack<SG_Grid::Point,4> RookMoves({
        { 0,-1},
        { 1, 0},
        { 0, 1},
        {-1, 0}
    });
    template <bool QueensCase> auto& AvailableMoves(){ if constexpr (QueensCase) return QueenMoves; else return RookMoves; } //TODO make stack const correct so that this can return a const refrence


    //TODO add const correct methods for HashMap types
    template <typename HashMap_t, typename Out_t>
    inline auto FlowfieldToPath(Out_t& out, const SG_Grid::Point& examine, const SG_Grid::Point& startPoint, const SG_Grid::Point& endPoint, HashMap_t& visited){
        if (examine == endPoint){ // Only return a path if we found one
            auto trace = endPoint;
            auto dir = SG_Grid::Point(0,0);
            while (trace != startPoint){
                auto newDir = visited.get(trace);
                if (dir != newDir){ out.push(trace); dir = newDir; } //Only add to output stack if it's a corner node
                trace = trace - newDir;
            }
        }
    }
}
