//
// Created by nickberryman on 10/02/26.
//
module;
#include <type_traits>
#include "Logger.h"

export module SG_Pathfind:HashMapBinaryHeap;
import :BasePriorityQueue;
import Logger;
import SG_Grid;
import SG_Allocator;
import :GridRangeHashMap;
import :BinaryHeap;


export namespace SG_Pathfind::PriorityQueue {
    template<typename InsideArenaType, typename pathfindGrid_t, bool fullDecreaseKey = true, bool fifoOnTie = true, bool uniformCost = false, bool useBitfield = false, bool noHashSet = false>
    class HashMapBinaryHeap {
    public:
        HashMapBinaryHeap(InsideArenaType& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& maxDistanceChebyshev, const SG_Grid::coordinate_t& maxCost, const SG_Grid::coordinate_t& minCost = 0) :
            queue(arena, maxCost, minCost), 
            hashMap(arena, within, centrePoint, maxDistanceChebyshev) {}
    
    inline const SG_Grid::Point& valueAt(const SG_Grid::u_coordinate_t& priority) { return queue.valueAt(queue.encodePriority(priority)); }
    inline SG_Grid::u_coordinate_t findMin() { return queue.decodePriority(queue.findMin()); }
    inline const SG_Grid::u_coordinate_t& length() {  return queue.length(); }

    inline SG_Grid::Point extractMin() {
        SG_Grid::Point out = queue.extractMin();
        //if constexpr (!noHashSet && !uniformCost) hashMap.remove(out); //TODO Not sure if this is necessary in this version -> Have a think about it
        return out;
    }

    inline void insert(const SG_Grid::Point& tile, const SG_Grid::u_coordinate_t& priority_ ) {
        auto& priority = queue.encodePriority(priority_);
        if constexpr (noHashSet) queue.insert(tile, priority);
        else {
            if (!hashMap.contains(tile)) {
                queue.forceInsert(tile, priority);
                hashMap.insert(tile, priority);
                return;
            }
            if constexpr (uniformCost) return;
            SG_Grid::u_coordinate_t& toCheck = hashMap.get(tile);
            if (toCheck <= priority) return;
            queue.insert(tile, priority_); //Automatically handles decreaseKey
            toCheck = priority; //Sets via reference
        }
    }

    inline void clear() {
        queue.clear();
        hashMap.clear();
    }

    private:
        struct empty{};
        BinaryHeap<SG_Grid::Point, SG_Grid::u_coordinate_t, InsideArenaType, fullDecreaseKey, fifoOnTie> queue; //TODO add some outer template parameters for rook/queen, and reserveDivisor
        [[no_unique_address]] std::conditional_t<noHashSet, empty, HashMap::GridRangeHashMap<InsideArenaType, SG_Grid::u_coordinate_t , useBitfield>>  hashMap;
    };
}
