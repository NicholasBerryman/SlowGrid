//
// Created by nickberryman on 10/02/26.
//
module;
#include <type_traits>
#include "Logger.h"

export module SG_Pathfind:HashMapBucketQueue;
import :BasePriorityQueue;
import Logger;
import SG_Grid;
import SG_Allocator;
import :GridRangeHashMap;
import :BucketQueue;


//TODO add template option for FIFO or LIFO within buckets
//TODO Maybe completely refactor BucketQueue to use only a single LinkedList with a ULL of void* for bucket locations, instead of the ULL of LLs
export namespace SG_Pathfind::PriorityQueue {
    template<typename InsideArenaType, typename pathfindGrid_t, bool fullDecreaseKey = true, bool uniformCost = false, bool useBitfield = false, bool noHashSet = false>
    class HashMapBucketQueue {
    public:
        HashMapBucketQueue(InsideArenaType& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& maxDistanceChebyshev, const SG_Grid::coordinate_t& maxCost, const SG_Grid::coordinate_t& minCost = 0) :
            queue(arena, maxCost, minCost), 
            hashMap(arena, within, centrePoint, maxDistanceChebyshev) {}
    
    inline const SG_Grid::Point& valueAt(const SG_Grid::coordinate_t& priority, const SG_Grid::coordinate_t& indexInBucket = 0) { return queue.valueAt(queue.encodePriority(priority), indexInBucket); }
    inline SG_Grid::coordinate_t findMin() { return queue.decodePriority(queue.findMin()); }
    inline const SG_Grid::coordinate_t& length(){ return queue.length(); }
    inline SG_Grid::Point extractMin() requires (noHashSet || uniformCost) { return queue.extractMin(); }
    inline SG_Grid::Point extractMin() requires (!noHashSet && !uniformCost) {
        SG_Grid::Point out = queue.extractMin();
        hashMap.remove(out);
        return out;
    }

    inline void insert(const SG_Grid::Point& tile, const SG_Grid::coordinate_t& priority_ ) {
        auto priority = queue.encodePriority(priority_);
        if constexpr (noHashSet) queue.insert(tile, priority);
        else {
            nodeAddress& toCheck = hashMap.get(tile);
            if (!hashMap.contains(tile)) {
                hashMap.insert(tile);
                toCheck.node = queue.forceInsert(tile, priority);
                toCheck.priority = priority;
                return;
             }
            if constexpr (uniformCost) return;
            if (toCheck.priority <= priority) return;
            toCheck.node = queue.decreaseKeyAndReturn(toCheck.node, toCheck.priority, priority);
            toCheck.priority = priority;
        }
    }

    inline void clear() {
        queue.clear();
        hashMap.clear();
    }

    private:
        struct nodeAddress {
            void* node;
            SG_Grid::coordinate_t priority;
        };
        struct empty{};
        BucketQueue<SG_Grid::Point, SG_Grid::coordinate_t, SG_Grid::coordinate_t, InsideArenaType> queue;
        std::conditional_t<noHashSet, empty, HashMap::GridRangeHashMap<InsideArenaType, nodeAddress, useBitfield>>  hashMap;
    };
}
