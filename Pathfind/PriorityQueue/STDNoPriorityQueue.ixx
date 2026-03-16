//
// Created by nickberryman on 10/02/26.
//
module;
#include <type_traits>
#include "Logger.h"
#include <queue>
#include <vector>
#include <iterator>
#include <functional>
#include <algorithm>

export module SG_Pathfind:STDNoPriorityQueue;
import :BasePriorityQueue;
import Logger;
import SG_Grid;
import SG_Allocator;
import :STDHashMap;


export namespace SG_Pathfind::PriorityQueue {
    template<typename InsideArenaType, typename pathfindGrid_t, bool queensCase = true, bool doubleBuffer = true, bool useContains = true,  bool useBitfield = false, bool noHashSet = false>
    class STDNoPriorityQueue {
    public:
        static SG_Grid::u_coordinate_t maxQueueSize(const SG_Grid::u_coordinate_t& maxDistance) {
            if constexpr (queensCase) return 8*maxDistance+1; //extra plus one is a queue requirement
            return 4*maxDistance+1; //extra plus one is a queue requirement
        }

        STDNoPriorityQueue(InsideArenaType& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& maxDistance, const SG_Grid::coordinate_t& maxCost = 0, const SG_Grid::coordinate_t& minCost = 0) :
            queue(),
            queue2(),
            hashMap(arena, within, centrePoint, maxDistance),
            swap(false) {}

        inline SG_Grid::Point valueAt(const SG_Grid::u_coordinate_t& priority) {
            if constexpr (doubleBuffer) {
                auto& q = !swap ? queue : queue2;
                return q[priority];
            }
            return queue[priority];
        }

        static inline SG_Grid::u_coordinate_t findMin() {
            return 0;
        }
        inline SG_Grid::u_coordinate_t length() const {
            if constexpr (doubleBuffer) {
                auto& q = !swap ? queue : queue2;
                return q.size();
            }
            return queue.size();
        }

        inline SG_Grid::Point extractMin() {
            if constexpr (doubleBuffer) {
                auto& q = !swap ? queue : queue2;
                auto out = q.front();
                q.pop_front();
                return out;
            }
            auto out = queue.front();
            queue.pop_front();
            return out;
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
                q.push_back(tile);
                return;
            }
            queue.push_back(tile);
        }

        inline void clear() {
            if constexpr (doubleBuffer) {
                auto& q = !swap ? queue : queue2;
                while (!q.empty()) q.pop_front();
                return;
            }
            while (!queue.empty()) queue.pop_front();
        }

        inline bool trySwap() {
            if constexpr (!doubleBuffer) { return false; }
            else {
                if (!swap && queue.size() == 0) {swap = true; return true;}
                if (swap && queue2.size() == 0) {swap = false; return true;}
                return false;
            }
        }

        private:
            std::deque<SG_Grid::Point> queue;
            struct empty {
                empty(bool){}
                empty(){}
            };

            [[nodiscard]] bool queueContains(const SG_Grid::Point& p) const { for (auto i = 0; i < queue.size(); ++i){if (queue[i] == p) return true; }; return false; }
            [[no_unique_address]] std::conditional_t<doubleBuffer, decltype(queue), empty> queue2;

            [[no_unique_address]] std::conditional_t<noHashSet, empty, HashMap::STDHashMap<InsideArenaType, bool, useContains, useBitfield>>  hashMap;
            [[no_unique_address]] std::conditional_t<doubleBuffer, bool, empty> swap;
    };
}
