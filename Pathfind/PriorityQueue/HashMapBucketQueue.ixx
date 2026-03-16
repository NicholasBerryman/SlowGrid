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


export namespace SG_Pathfind::PriorityQueue {
    template<typename InsideArenaType, typename pathfindGrid_t, bool useContains = true, bool fullDecreaseKey = true, bool fifoOnTie = true, bool uniformCost = false, bool useBitfield = false, bool noHashSet = false>
    class HashMapBucketQueue {
    public:
        HashMapBucketQueue(InsideArenaType& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& maxDistanceChebyshev, const SG_Grid::coordinate_t& maxCost, const SG_Grid::coordinate_t& minCost = 0) :
            queue(arena, maxCost, minCost), 
            hashMap(arena, within, centrePoint, maxDistanceChebyshev) {}
    
    inline const SG_Grid::Point& valueAt(const SG_Grid::u_coordinate_t& priority, const SG_Grid::u_coordinate_t& indexInBucket = 0) { return queue.valueAt(queue.encodePriority(priority), indexInBucket); }
    inline SG_Grid::u_coordinate_t findMin() { return queue.decodePriority(queue.findMin()); }
    inline const SG_Grid::u_coordinate_t& length() {  return queue.length(); }

    inline SG_Grid::Point extractMin() {
        SG_Grid::Point out = queue.extractMin();
        if constexpr (!noHashSet && !uniformCost && fullDecreaseKey) hashMap.remove(out); // Prevents decreaseKey running on recycled/invalid nodeAddresses
        return out;
    }

    inline void insert(const SG_Grid::Point& tile, const SG_Grid::u_coordinate_t& priority_, bool force = false ) {
        auto priority = queue.encodePriority(priority_);
        if constexpr (noHashSet) queue.insert(tile, priority);
        else {
            bool toInsert;
            if constexpr (!useContains) {toInsert = force;}
            else toInsert = force || !hashMap.contains(tile);
            if (toInsert) {
                hashMap.insert(tile, {queue.forceInsert(tile, priority), priority});
                return;
            }
            if constexpr (uniformCost) return;
            nodeAddress& toCheck = hashMap.get(tile);
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
            SG_Grid::u_coordinate_t priority;
        };
        struct empty{};
        BucketQueue<SG_Grid::Point, SG_Grid::u_coordinate_t, SG_Grid::u_coordinate_t, InsideArenaType, fullDecreaseKey, fifoOnTie> queue; //TODO make bucketSize_t the same as priority_t in the BucketQueue implementation -> makes it work nicer with the binary heap version too
        [[no_unique_address]] std::conditional_t<noHashSet, empty, HashMap::GridRangeHashMap<InsideArenaType, nodeAddress, useContains, useBitfield>>  hashMap;
    };
}
