//
// Created by nickberryman on 10/12/25.
//
module;
#include <type_traits>


export module SG_Pathfind:Utils;
import LocalDataStructures;
import SG_Grid;
import :GridRangeHashMap;
import :STDHashMap;
import :BaseHashMap;


export namespace SG_Pathfind::Utils {
    template<typename T> concept Path_c  = requires(T t, SG_Grid::Point p) { {t.push(p)}; };
    template <typename T> concept Flowfield_c = HashMap::GridHashmap_c<T,SG_Grid::Point>;
    template <typename T> concept Dmat_c = HashMap::GridHashmap_c<T,SG_Grid::u_coordinate_t>;

    template <const SG_Grid::u_coordinate_t maxOutputNodes = 256> using defaultPath_t = LocalDataStructures::Stack<SG_Grid::Point, maxOutputNodes>;
    template <typename ArenaType, bool useSTD = false> using defaultFlowfield_t = std::conditional_t<useSTD, HashMap::STDHashMap<ArenaType,SG_Grid::Point>,          HashMap::GridRangeHashMap<ArenaType,SG_Grid::Point>>;
    template <typename ArenaType, bool useSTD = false> using defaultDmatrix_t   = std::conditional_t<useSTD, HashMap::STDHashMap<ArenaType,SG_Grid::u_coordinate_t>, HashMap::GridRangeHashMap<ArenaType,SG_Grid::u_coordinate_t>>;

    
    //TODO probably prioritise horizontal movement first (might reduce weird movement for queen's case) -> might need to update unit tests to account
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
    template <bool QueensCase> const auto& AvailableMoves(){ if constexpr (QueensCase) return QueenMoves; else return RookMoves; }

    //TODO add some string-pulling functions, so we can get nicer paths esp in QueensCase
        // Add a grid-bound string pull function
        // Add an any-direction string pull function
    template <typename HashMap_t, typename Out_t, const SG_Grid::u_coordinate_t maxOutputNodes = 256>
    auto FlowfieldToPath(Out_t& out, const SG_Grid::Point& examine, const SG_Grid::Point& startPoint, const SG_Grid::Point& endPoint, const HashMap_t& visited){
        if (examine == endPoint){ // Only return a path if we found one
            auto trace = endPoint;
            auto dir = SG_Grid::Point(0,0);
            while (trace != startPoint && out.length() < maxOutputNodes){
                auto newDir = visited.get(trace);
                if (dir != newDir){ out.push(trace); dir = newDir; } //Only add to output stack if it's a corner node
                if (out.length() == maxOutputNodes) return;
                trace = trace - newDir;
            }
        }
    }

    //TODO add a 'threatMatrix' function based on a min/max distance, a distance matrix and a 'blocker' matrix -> return a new grid showing distance to put tiles within that range without hitting a 'blocker' -> e.g. showing Fire Emblem archer attackable squares
}
