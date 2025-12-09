//
// Created by nickberryman on 1/12/25.
//

module;
#include <type_traits>
#include"Logger.h"

export module SG_Allocator:CachedQueue;
import SG_AllocatorConfigs;
import :BaseArena;
import LocalDataStructures;
import Logger;

export namespace SG_Allocator {
    /**
     * @brief Cache-oblivious  queue implementation. Supports very long queues. Designed for situations where only small numbers of pops occur at a given time, e.g. in an AI waypoint queue
     * @tparam InsideArenaType Arena to allocate to (PseudoArena for raw heap allocation)
     * @tparam T Type to store in queue
     * @tparam cacheSize Number of elements to store in the cache-oblivious part of the queue. NOT ACTUAL CPU CACHE SIZE
     * @tparam backlogCapacity Number of elements in the non-cache-oblivious part of the queue
     */
    template<typename InsideArenaType, typename T, localSize_t cacheSize, arenaSize_t backlogCapacity>
	requires std::is_base_of_v<BaseArena, InsideArenaType>
    class CachedQueue {
        static_assert(cacheSize > 0, "CachedQueue must have cacheSize > 0");
    public:
        explicit CachedQueue(InsideArenaType& arena);
        inline void push(T val);
        inline T pop();
        inline T popUnsafe();
        [[nodiscard]] inline T& peek(localSize_t offset = 0);
        [[nodiscard]] inline const T& peekUnsafe(localSize_t offset = 0);
        inline void refresh();
        inline void clear();
        [[nodiscard]] inline localSize_t length() const;
        [[nodiscard]] inline SG_Allocator::arenaSize_t cacheLength() const;
        [[nodiscard]] inline localSize_t maxLength() const;
        [[nodiscard]] inline SG_Allocator::arenaSize_t cacheMaxLength() const;

    private:
        InsideArenaType _arena;
        LocalDataStructures::Queue<T, cacheSize> cache;
        LocalDataStructures::Queue<T, backlogCapacity>* backlog;
    };
}

/**
 * @brief Constructor for cached queue
 * @param arena Arena object to store backlog in (cache-oblivious part is stored locally to the CachedQueue object)
 */
template<typename InsideArenaType, typename T, localSize_t cacheSize, SG_Allocator::arenaSize_t totalCapacity>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
SG_Allocator::CachedQueue<InsideArenaType, T, cacheSize, totalCapacity>::CachedQueue(InsideArenaType& arena):
_arena(arena){
    backlog = arena.template allocConstruct<LocalDataStructures::Queue<T, totalCapacity>>();
}

/**
 * @brief Adds copy of element to back of queue
 * @param val Value to copy and push
 */
template<typename InsideArenaType, typename T, localSize_t cacheSize, SG_Allocator::arenaSize_t totalCapacity> requires std::
is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
void SG_Allocator::CachedQueue<InsideArenaType, T, cacheSize, totalCapacity>::push(T val) {
    LOGGER_ASSERT_EXCEPT(this->length() < this->maxLength());
    if (cache.length() < cacheSize) cache.push(val);
    else backlog->push(val);
}

/**
 * @brief Removes and returns front element of queue, looking in the cache-oblivious section first (FIFO). Usually cache-oblivious
 * @return Copy of front element of queue
 */
template<typename InsideArenaType, typename T, localSize_t cacheSize, SG_Allocator::arenaSize_t totalCapacity> requires std::
is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
T SG_Allocator::CachedQueue<InsideArenaType, T, cacheSize, totalCapacity>::pop() {
    LOGGER_ASSERT_EXCEPT(cache.length() > 0 || backlog->length() > 0);
    if (cache.length() > 0) return cache.pop();
    return backlog->pop();
}

/**
 * @brief Removes and returns front element from cache-oblivious section of queue (FIFO). Will never look at the backlog. Guaranteed cache-oblivious and slightly faster than pop() if there are elements in this section
 * @return Copy of front element from the cache-oblivious section of the queue
 */
template<typename InsideArenaType, typename T, localSize_t cacheSize, SG_Allocator::arenaSize_t totalCapacity> requires std::
is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
T SG_Allocator::CachedQueue<InsideArenaType, T, cacheSize, totalCapacity>::popUnsafe() {
    LOGGER_ASSERT_EXCEPT(cache.length() > 0);
    return cache.pop();
}

/**
 *
 * @param offset Number of elements from front to skip over. Looks in cache-oblivious section first
 * @return Reference to the element of the queue with the specified offset from the front (FIFO)
 */
template<typename InsideArenaType, typename T, localSize_t cacheSize, SG_Allocator::arenaSize_t totalCapacity> requires std::
is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
T & SG_Allocator::CachedQueue<InsideArenaType, T, cacheSize, totalCapacity>::peek(localSize_t offset) {
    LOGGER_ASSERT_EXCEPT(cache.length() + backlog->length() > offset);
    if (cache.length() > offset) return cache.peekRef(offset);
    return backlog->peekRef(offset-cache.length());
}

/**
 *
 * @param offset Number of elements from front to skip over. Only looks inside cache-oblivious section (marginally faster than peek)
 * @return Reference to the element of the cache-oblivious section of the queue with the specified offset from the front (FIFO)
 */
template<typename InsideArenaType, typename T, localSize_t cacheSize, SG_Allocator::arenaSize_t totalCapacity> requires std::
is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
const T & SG_Allocator::CachedQueue<InsideArenaType, T, cacheSize, totalCapacity>::peekUnsafe(localSize_t offset) {
    LOGGER_ASSERT_EXCEPT(cache.length() > offset);
    return cache.peekRef(offset);
}

/**
 * Refill the cache-oblivious section from the backlog. Guarantees cache-oblivious behaviour from the next cacheSize pops, but is not itself cache-oblivious
 */
template<typename InsideArenaType, typename T, localSize_t cacheSize, SG_Allocator::arenaSize_t totalCapacity> requires std::
is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
void SG_Allocator::CachedQueue<InsideArenaType, T, cacheSize, totalCapacity>::refresh() {
    while (cache.length() < cacheSize) cache.push(backlog->pop());
}

/**
 * Sets the queue length to zero and removes all elements. DOES NOT CALL DESTRUCTORS
 */
template<typename InsideArenaType, typename T, localSize_t cacheSize, SG_Allocator::arenaSize_t totalCapacity> requires std::
is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
void SG_Allocator::CachedQueue<InsideArenaType, T, cacheSize, totalCapacity>::clear() {
    cache.clear();
    backlog->clear();
}

/**
 *
 * @return Number of elements available to be popped from the queue
 */
template<typename InsideArenaType, typename T, localSize_t cacheSize, SG_Allocator::arenaSize_t totalCapacity> requires std::
is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
localSize_t SG_Allocator::CachedQueue<InsideArenaType, T, cacheSize, totalCapacity>::length() const {
    return cache.length() + backlog->length();
}

/**
 *
 * @return Number of elements available to be popped from the cache-oblivious section of the queue
 */
template<typename InsideArenaType, typename T, localSize_t cacheSize, SG_Allocator::arenaSize_t totalCapacity> requires std::
is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
SG_Allocator::arenaSize_t SG_Allocator::CachedQueue<InsideArenaType, T, cacheSize, totalCapacity>::cacheLength() const {
    return cache.length();
}

/**
 *
 * @return Max number of elements that can fit in the queue
 */
template<typename InsideArenaType, typename T, localSize_t cacheSize, SG_Allocator::arenaSize_t totalCapacity> requires
std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
localSize_t SG_Allocator::CachedQueue<InsideArenaType, T, cacheSize, totalCapacity>::maxLength() const {
    return cache.maxLength() + backlog->maxLength();
}

/**
 *
 * @return Max number of elements that can be stored in the cache-oblivious section before overflowing into the backlog
 */
template<typename InsideArenaType, typename T, localSize_t cacheSize, SG_Allocator::arenaSize_t totalCapacity> requires
std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
SG_Allocator::arenaSize_t SG_Allocator::CachedQueue<InsideArenaType, T, cacheSize, totalCapacity>::cacheMaxLength() const {
    return cache.maxLength();
}








