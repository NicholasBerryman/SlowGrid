//
// Created by nickberryman on 10/02/26.
//
module;
#include <type_traits>
#include "Logger.h"

export module SG_Pathfind:NoPriorityQueue;
import :BasePriorityQueue;
import Logger;
import LocalDataStructures;
import SG_Grid;
import SG_Allocator;
import :GridRangeHashMap;

export namespace SG_Pathfind::PriorityQueue {
    template<typename InsideArenaType, typename pathfindGrid_t, bool queensCase = true, bool doubleBuffer = true, bool useContains = true,  bool useBitfield = false, bool noHashSet = false>
    class NoPriorityQueue {
    public:
        static SG_Grid::u_coordinate_t maxQueueSize(const SG_Grid::u_coordinate_t& maxDistance) {
            if constexpr (queensCase) return 8*maxDistance+1; //extra plus one is a queue requirement
            return 4*maxDistance+1; //extra plus one is a queue requirement
        }

        NoPriorityQueue(InsideArenaType& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& maxDistance, const SG_Grid::coordinate_t& maxCost = 0, const SG_Grid::coordinate_t& minCost = 0) :
            queue(maxQueueSize(maxDistance), arena, maxQueueSize(maxDistance)+1),
            queue2(maxQueueSize(maxDistance), arena, maxQueueSize(maxDistance)+1),
            hashMap(arena, within, centrePoint, maxDistance),
            swap(false) {}

        inline const SG_Grid::Point& valueAt(const SG_Grid::u_coordinate_t& priority) {
            if constexpr (doubleBuffer) {
                auto& q = !swap ? queue : queue2;
                return q.peekRef(priority);
            }
            return queue.peekRef(priority);
        }

        static inline SG_Grid::u_coordinate_t findMin() {
            return 0;
        }

        [[nodiscard]] inline SG_Grid::u_coordinate_t length() const {
            if constexpr (doubleBuffer) {
                auto& q = !swap ? queue : queue2;
                return q.length();
            }
            return queue.length();
        }

        inline const SG_Grid::Point& extractMin() {
            if constexpr (doubleBuffer) {
                auto& q = !swap ? queue : queue2;
                return q.pop();
            }
            return queue.pop();
        }

        inline void insert(const SG_Grid::Point& tile, const SG_Grid::u_coordinate_t& priority = 0, bool force = false) {
            if constexpr (noHashSet)  {if (queueContains(tile)) return;}
            else {
                bool toInsert;
                if constexpr (!useContains) {toInsert = force;}
                else toInsert = force || !hashMap.contains(tile);
                if (!toInsert) return;
                hashMap.insert(tile);
            }

            if constexpr (doubleBuffer) {
                auto& q = !swap ? queue2 : queue;
                q.push(tile);
                return;
            }
            queue.push(tile);
        }

        inline void clear() {
            if constexpr (doubleBuffer) {
                auto& q = !swap ? queue : queue2;
                q.clear();
                return;
            }
            queue.clear();
        }

        inline bool trySwap() {
            if constexpr (!doubleBuffer) { return false; }
            else {
                if (!swap && queue.length() == 0) {swap = true; return true;}
                if (swap && queue2.length() == 0) {swap = false; return true;}
                return false;
            }
        }

        private:
            LocalDataStructures::Base::Queue<SG_Allocator::RuntimeArray<SG_Grid::Point, SG_Grid::u_coordinate_t>, SG_Grid::u_coordinate_t> queue;
            struct empty {
                empty(const SG_Grid::u_coordinate_t&, const InsideArenaType&, const SG_Grid::u_coordinate_t&){}
                empty(bool){}
            };

            [[nodiscard]] bool queueContains(const SG_Grid::Point& p) const { for (auto i = 0; i < queue.length(); ++i){if (queue.peek(i) == p) return true; }; return false; }
            [[no_unique_address]] std::conditional_t<doubleBuffer, decltype(queue), empty> queue2;

            [[no_unique_address]] std::conditional_t<noHashSet, empty, HashMap::GridRangeHashMap<InsideArenaType, bool, useContains, useBitfield>>  hashMap;
            [[no_unique_address]] std::conditional_t<doubleBuffer, bool, empty> swap;
    };
}
