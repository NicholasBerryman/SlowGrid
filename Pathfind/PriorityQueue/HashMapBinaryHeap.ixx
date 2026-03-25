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
    template<typename InsideArenaType, typename pathfindGrid_t, bool fullDecreaseKey = true, bool fifoOnTie = true, bool noHashSet = false>
    class HashMapBinaryHeap {
    public:
        HashMapBinaryHeap(InsideArenaType& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& maxDistanceChebyshev, const SG_Grid::coordinate_t& maxCost, const SG_Grid::coordinate_t& minCost = 0) :
            queue(arena, maxCost, minCost), 
            hashMap(arena, within, centrePoint, maxDistanceChebyshev) {}
    
    inline const SG_Grid::Point& valueAt(const SG_Grid::u_coordinate_t& priority) { return queue.valueAt(queue.encodePriority(priority)); }
    inline SG_Grid::u_coordinate_t findMin() { return queue.decodePriority(queue.findMin()); }
    inline const SG_Grid::u_coordinate_t& length() {  return queue.length(); }

    inline SG_Grid::Point extractMin() {
        const SG_Grid::Point out = queue.extractMin();
        if constexpr (!noHashSet) hashMap.remove(out);
        return out;
    }

    inline bool insert(const SG_Grid::Point& tile, const SG_Grid::u_coordinate_t& priority_, const SG_Grid::u_coordinate_t& lastPriority = false) {
        if (lastPriority <= priority_ && lastPriority > 0) return false;
        auto& priority = queue.encodePriority(priority_);
        if constexpr (noHashSet) {
            return queue.insert(tile, priority, lastPriority);
        }
        else {
            if (lastPriority == 0 && !hashMap.contains(tile)) {
                queue.forceInsert(tile, priority);
                hashMap.insert(tile);
                return true;
            }
            auto out = queue.insert(tile, priority, queue.encodePriority(lastPriority));
            return out;
        }
    }

    inline void clear() {
        queue.clear();
        hashMap.clear();
    }

    private:
        struct empty{
            inline empty(const auto&, const auto&, const auto&, const auto&) {}
            static inline void insert(const auto&){}
            static inline void remove(const auto&){}
            static inline bool contains(const auto&){return false;}
        };
        BinaryHeap<SG_Grid::Point, SG_Grid::u_coordinate_t, InsideArenaType, fullDecreaseKey, fifoOnTie> queue; //TODO add some outer template parameters for rook/queen, and reserveDivisor
        [[no_unique_address]] std::conditional_t<noHashSet || !fullDecreaseKey, empty, HashMap::GridRangeHashMap<InsideArenaType, bool, true, true>>  hashMap;
    };
}
