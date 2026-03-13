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
    template<typename InsideArenaType, typename pathfindGrid_t, bool fullDecreaseKey = true, bool fifoOnTie = true, bool uniformCost = false, bool useBitfield = false, bool noHashSet = false>
    class STDPriorityQueue {
    public:
        STDPriorityQueue(InsideArenaType& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& maxDistanceChebyshev, const SG_Grid::coordinate_t& maxCost, const SG_Grid::coordinate_t& minCost = 0) :
            queue(), 
            hashMap(arena, within, centrePoint, maxDistanceChebyshev),
            counter(0) {}
    
    inline SG_Grid::Point valueAt(const SG_Grid::u_coordinate_t& priority) {
        return queue.find(priority);
    }
    inline SG_Grid::u_coordinate_t findMin() { 
        return queue.top().priority;
    }
    inline SG_Grid::u_coordinate_t length() {
        return queue.size();
    }

    inline SG_Grid::Point extractMin() {
        auto out = queue.top().val;
        queue.pop();
        return out;
    }

    inline void insert(const SG_Grid::Point& tile, const SG_Grid::u_coordinate_t& priority ) {
        ++counter;
        if constexpr (noHashSet) queue.smartInsert(tile, priority, counter);
        else {
            if (!hashMap.contains(tile)) {
                queue.smartInsert(tile, priority, counter);
                hashMap.insert(tile, priority);
                return;
            }
            if constexpr (uniformCost) return;
            SG_Grid::u_coordinate_t toCheck = hashMap.get(tile);
            if (toCheck <= priority) return;
            queue.smartInsert(tile, priority, counter); //Automatically handles decreaseKey
        }
    }

    inline void clear() {
        while (!queue.empty()) queue.pop();
    }
    
    private:
        struct empty{};
        struct node{
            SG_Grid::Point val;
            SG_Grid::u_coordinate_t priority;
            SG_Grid::u_coordinate_t tiebreak;
            
            bool operator<(const node& other) const { 
                if constexpr (fifoOnTie)  if (priority == other.priority) return tiebreak > other.tiebreak;
                if constexpr (!fifoOnTie) if (priority == other.priority) return tiebreak < other.tiebreak;
                return priority < other.priority; 
            }
            bool operator>(const node& other) const { 
                if constexpr (fifoOnTie)  if (priority == other.priority) return tiebreak < other.tiebreak;
                if constexpr (!fifoOnTie) if (priority == other.priority) return tiebreak > other.tiebreak;
                return priority > other.priority; 
            }
        };
        
        class q_t : public std::priority_queue<node, std::vector<node>, std::greater<node>>{
            public:
            SG_Grid::Point find(const SG_Grid::u_coordinate_t& val) const {
                auto first = this->c.cbegin();
                auto last = this->c.cend();
                while (first!=last) {
                    if (first->priority == val) return first->val;
                    ++first;
                }
                return last->val;
            }
            bool remove(const SG_Grid::Point& value) {
                auto it = this->c.cbegin();
                auto last = this->c.cend();
                while (it!=last) {
                    if (it->val == value) break;
                    ++it;
                }
                if (it == this->c.end()) {
                    return false;
                }
                if (it == this->c.begin()) {
                    // deque the top element
                    this->pop();
                }    
                else {
                    // remove element and re-heap
                    this->c.erase(it);
                    std::make_heap(this->c.begin(), this->c.end(), this->comp);
                }
                return true;
            }
            
            void smartInsert(const SG_Grid::Point& value, const SG_Grid::u_coordinate_t& priority, const SG_Grid::u_coordinate_t& tiebreak){
                if constexpr (fullDecreaseKey) remove(value);
                this->push({value, priority, tiebreak});
            }
        };

        q_t queue;
        [[no_unique_address]] std::conditional_t<noHashSet, empty, HashMap::STDHashMap<InsideArenaType, SG_Grid::u_coordinate_t>>  hashMap;
        SG_Grid::u_coordinate_t counter;
    };
}
