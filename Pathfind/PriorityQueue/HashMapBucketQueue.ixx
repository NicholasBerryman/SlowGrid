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
    template<typename InsideArenaType, typename pathfindGrid_t, bool fullDecreaseKey = true, bool fifoOnTie = false, bool noHashSet = false>
    class HashMapBucketQueue {
    public:
        HashMapBucketQueue(InsideArenaType& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& maxDistanceChebyshev, const SG_Grid::coordinate_t& maxCost, const SG_Grid::coordinate_t& minCost = 0) :
            queue(arena, maxCost, minCost),
            hashMap(arena, within, centrePoint, maxDistanceChebyshev) {}

    inline const SG_Grid::Point& valueAt(const SG_Grid::u_coordinate_t& priority, const SG_Grid::u_coordinate_t& indexInBucket = 0) { return queue.valueAt(queue.encodePriority(priority), indexInBucket); }
    inline SG_Grid::u_coordinate_t findMin() { return queue.decodePriority(queue.findMin()); }
    inline const SG_Grid::u_coordinate_t& length() {  return queue.length(); }

    inline SG_Grid::Point extractMin() {
        const SG_Grid::Point out = queue.extractMin();
        if constexpr (!noHashSet) hashMap.remove(out); // Prevents decreaseKey running on recycled/invalid nodeAddresses
        return out;
    }

    inline bool insert(const SG_Grid::Point& tile, const SG_Grid::u_coordinate_t& priority_, const SG_Grid::u_coordinate_t& lastPriority = 0 ) {
        if (lastPriority <= priority_ && lastPriority > 0) return false;
        auto priority = queue.encodePriority(priority_);
        if constexpr (noHashSet) {
            return queue.insert(tile, priority, lastPriority);
        }
        else {
            if constexpr (!fullDecreaseKey) {
                queue.forceInsert(tile, priority);
                return true;
            } else {
                if (lastPriority == 0 || !hashMap.contains(tile)) {
                    hashMap.insert(tile, {queue.forceInsert(tile, priority)});
                    return true;
                }
                nodeAddress& toCheck = hashMap.get(tile);
                auto out = queue.decreaseKeyAndReturn(toCheck.node, queue.encodePriority(lastPriority), priority);
                toCheck.node = out;
            }
            return true;
        }
    }

    inline void clear() {
        queue.clear();
        hashMap.clear();
    }

    private:
        struct nodeAddress {
            void* node;
        };
        struct empty {
            inline empty(const auto&, const auto&, const auto&, const auto&) {}
            static inline void insert(const auto&, const nodeAddress&){}
            static inline void remove(const auto&){}
            static inline bool contains(const auto&){return false;}
            static inline nodeAddress get(const auto&){return {nullptr};}
        };
        BucketQueue<SG_Grid::Point, SG_Grid::u_coordinate_t, SG_Grid::u_coordinate_t, InsideArenaType, fullDecreaseKey, fifoOnTie> queue;
        [[no_unique_address]] std::conditional_t<noHashSet || !fullDecreaseKey, empty, HashMap::GridRangeHashMap<InsideArenaType, nodeAddress, true, true>>  hashMap;
    };
}
