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
    template<typename InsideArenaType, typename pathfindGrid_t, bool queensCase = true, bool useBitfield = false, bool doubleBuffer = true, bool noHashSet = false>
    class NoPriorityQueue {
    public:
        static SG_Grid::u_coordinate_t maxQueueSize(const SG_Grid::u_coordinate_t& maxDistance) {
            if constexpr (queensCase) return 8*maxDistance+1+1; //extra plus one is a queue requirement
            return 4*maxDistance+1+1; //extra plus one is a queue requirement
        }

        NoPriorityQueue(InsideArenaType& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& maxDistanceChebyshev, const SG_Grid::coordinate_t& maxDistanceSelectedCase, const SG_Grid::coordinate_t& minCost = 0) :
            queue(arena, maxQueueSize(maxDistanceSelectedCase)),
            queue2(arena, maxQueueSize(maxDistanceSelectedCase)),
            hashMap(arena, within, centrePoint, maxDistanceChebyshev),
            swap(false) {}

        inline SG_Grid::Point valueAt(const SG_Grid::u_coordinate_t& priority) {
            if constexpr (doubleBuffer) {
                auto& q = !swap ? queue : queue2;
                return q.peek(priority);
            }
            return queue.peek(priority);
        }

        static inline SG_Grid::u_coordinate_t findMin() {
            return 0;
        }
        inline SG_Grid::u_coordinate_t length() const {
            if constexpr (doubleBuffer) {
                auto& q = !swap ? queue : queue2;
                return q.length();
            }
            return queue.length();
        }

        inline SG_Grid::Point extractMin() {
            if constexpr (doubleBuffer) {
                auto& q = !swap ? queue : queue2;
                return q.pop();
            }
            return queue.pop();
        }

        inline void insert(const SG_Grid::Point& tile, const SG_Grid::u_coordinate_t& priority = 0) {
            if constexpr (noHashSet)  {if (queueContains(tile)) return;}
            else {
                if (hashMap.contains(tile)) return;
                hashMap.insert(tile);
            }

            if constexpr (doubleBuffer) {
                auto& q = !swap ? queue : queue2;
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
            if (!swap && queue.length() == 0) {swap = true; return true;}
            if (swap && queue2.length() == 0) {swap = false; return true;}
            return false;
        }

        private:
            LocalDataStructures::Base::Queue<SG_Allocator::RuntimeArray<SG_Grid::Point, SG_Grid::u_coordinate_t>, SG_Grid::u_coordinate_t> queue;
            struct empty {
                empty(const InsideArenaType&, const SG_Grid::u_coordinate_t&){}
                empty(bool){}
                auto length(){return queue.length();}
                auto clear() {queue.clear();}
                auto push(const auto& a){queue.push(a);}
                auto pop(){return queue.pop();}
                auto peek(){return queue.peek();}
            };

            bool queueContains(const SG_Grid::Point& p){ for (auto i = 0; i < queue.length(); ++i){if (queue.peek(i) == p) return true; }; return false; }
            [[no_unique_address]] std::conditional_t<doubleBuffer, decltype(queue), empty> queue2;

            [[no_unique_address]] std::conditional_t<noHashSet, empty, HashMap::GridRangeHashMap<InsideArenaType, bool, useBitfield>>  hashMap;
            [[no_unique_address]] std::conditional_t<doubleBuffer, bool, empty> swap;
    };
}
