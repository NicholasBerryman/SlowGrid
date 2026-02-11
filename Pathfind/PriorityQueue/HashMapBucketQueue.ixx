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
//TODO add template to swap between bucket-indices being in structs or in hashmap. Maybe even completely refactor BucketQueue to use only a single LinkedList with a ULL of void* for bucket locations, instead of the ULL of LLs
//TODO remove double-up distance parameters, and add a bool template to swap between manhattan and chebyshev for priority queue -> automaticall convert to chebyshev for hashset always
export namespace SG_Pathfind::PriorityQueue {
    template<typename InsideArenaType, typename pathfindGrid_t, bool fullDecreaseKey = true, bool uniformCost = false, bool noHashSet = false>
    class HashMapBucketQueue {
    public:
        HashMapBucketQueue(InsideArenaType& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& maxDistanceChebyshev, const SG_Grid::coordinate_t& maxDistanceSelectedMetric, const SG_Grid::coordinate_t& minDistanceSelectedMetric = 0) :
            queue(arena, maxDistanceSelectedMetric, minDistanceSelectedMetric), 
            hashMap(arena, within, centrePoint, maxDistanceChebyshev, {nullptr, 0})
            {}
    
    inline const SG_Grid::Point& valueAt(const SG_Grid::coordinate_t& priority, const SG_Grid::coordinate_t& indexInBucket = 0) { return queue.valueAt(queue.encodePriority(priority), indexInBucket); }
    inline SG_Grid::coordinate_t findMin() { return queue.decodePriority(queue.findMin()); }
    inline const SG_Grid::coordinate_t& length(){ return queue.length(); }
    inline SG_Grid::Point extractMin() requires (noHashSet || uniformCost) { return queue.extractMin(); }
    inline SG_Grid::Point extractMin() requires (!noHashSet && !uniformCost) { 
        auto out = queue.extractMin();
        hashMap.get(out).node = nullptr; // Not sure if this is necessary, but probably safest to keep it in just in case
        return out;
    }
    
    inline void insert(const SG_Grid::Point& value, const SG_Grid::coordinate_t& priority_ ) {
        // Note that 'value' is actually the key in the hashmap, but is still a value in the queue
        auto priority = queue.encodePriority(priority_);
        if constexpr (noHashSet) queue.insert(value, priority);
        else {
            nodeAddress& toCheck = hashMap.get(value);
            if (toCheck.node == nullptr) { toCheck.node = queue.forceInsert(value, priority); toCheck.priority = priority; return; }
            if constexpr (uniformCost) return;
            if (toCheck.priority <= priority) return;
            toCheck.node = queue.decreaseKeyAndReturn(toCheck.node, toCheck.priority, priority);
            toCheck.priority = priority;
        }
    }
    
    private:
        struct nodeAddress {
            void* node = nullptr;
            SG_Grid::coordinate_t priority = 0;
        };
        struct empty{};
        BucketQueue<SG_Grid::Point, SG_Grid::coordinate_t, SG_Grid::coordinate_t, InsideArenaType> queue;
        std::conditional_t<noHashSet, empty, HashMap::GridRangeHashMap<nodeAddress>>  hashMap;
    };
}
