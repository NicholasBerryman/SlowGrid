//
// Created by nickberryman on 1/12/25.
//

module;
#include <type_traits>
#include "Logger.h"

export module SG_Allocator:UnrolledLinkedList;
import :BaseArena;
import :PseudoArena;
import :LinkedList;
import SG_AllocatorConfigs;
import Logger;

export namespace SG_Allocator {
    template<typename InsideArenaType, typename alignment = char>
    requires std::is_base_of_v<BaseArena, InsideArenaType>
    class NodeExaminer;

    /**
     * @brief Heterogeneous unrolled doubly-linked list, with arena allocation support
     *
     * @tparam InsideArenaType Arena type to allocate blocks into (Use a PseudoArena if heap allocation preferred)
     * @tparam alignment Data type to use for element size. e.g. char gives 1 byte per element, uint64_t give 4 bytes per element.
     */
    template<typename InsideArenaType, typename alignment = char>
    requires std::is_base_of_v<BaseArena, InsideArenaType>
    class ULL {
        friend class NodeExaminer<InsideArenaType, alignment>;
    public:
        ULL(InsideArenaType& arena_, const arenaSize_t& initialSize);
        inline ~ULL();
        template<typename T> inline T* alloc();
        template<typename T> inline T* allocArray(const arenaSize_t& arrayLength);

        inline void dealloc(const arenaSize_t& deallocAmount);
        inline void clear();
        inline void softClear();
        inline void expand(const arenaSize_t& newSize);

		//TODO look at increasing 'length' on fragmentation -> makes these functions' assertions work even when fragmentation occurs
        template<typename T> inline T* get(const arenaSize_t& index);
        template<typename T> inline T* getFromBack(const arenaSize_t& indexFromBack);

        [[nodiscard]] inline arenaSize_t maxSize() const;
        [[nodiscard]] inline const arenaSize_t& length() const;

    private:
        struct Block{
            arenaSize_t remainingSpace; //Maybe benchmark using this vs modulo operations??
            alignment* arr;
            Block(const arenaSize_t& a, alignment* const& b): remainingSpace(a), arr(b){}
        };

        InsideArenaType& arena;
        LinkedList<InsideArenaType, Block> impl;
        arenaSize_t _length;
        const arenaSize_t blockSize;
        void* tail = nullptr;

        inline void shrink();
    };

    /**
     * @brief Attorney class to allow direct ULL Node manipulation. Allows fast call-stack-like behaviour, but breaks encapsulation somewhat. Try not to use this unless you really want the performance.
     *
     * @tparam InsideArenaType Arena type to for associated ULL
     * @tparam alignment Alignment type for associated ULL
     */
    template<typename InsideArenaType, typename alignment>
    requires std::is_base_of_v<BaseArena, InsideArenaType>
    class NodeExaminer {
    public:
        /**
         *
         * @param ull ULL to examine
         * @return void* pointer to current ULL tail node
         */
        static void* currentTail(ULL<InsideArenaType, alignment>& ull) { return ull.tail; }
        /**
         * @brief Force shrink ULL if possible
         *
         * @param ull ULL to shrink
         */
        static void shrink(ULL<InsideArenaType, alignment>& ull){ ull.shrink(); }
        /**
         * @brief Sets the ULL tail
         *
         * @param ull ULL to set on
         * @param newTail Pointer to new tail (should get from 'void* currentTail(...)')
         * @param newLength Length to set ULL to use after switching to the new tail (should get from ULL.length() at same time as tail)
         */
        static void setTail(ULL<InsideArenaType, alignment>& ull, void* const& newTail, const arenaSize_t& newLength) {
            ull.tail = static_cast<void*>(newTail);
            ull._length = newLength;
            arenaSize_t effectiveCapacity = newLength/ull.blockSize * ull.blockSize;
            if (newLength > effectiveCapacity) effectiveCapacity += ull.blockSize;
            if (effectiveCapacity < ull.blockSize) effectiveCapacity = ull.blockSize;
            ull.impl.get_atNode(ull.tail).remainingSpace = effectiveCapacity - newLength;
        }
    };
}

/**
 * @brief Default constructor for ULL
 *
 * @param arena_ Arena object to use for block allocations
 * @param initialSize Size of blocks, including the initial block
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
SG_Allocator::ULL<InsideArenaType, alignment>::ULL(InsideArenaType& arena_, const arenaSize_t& initialSize):
    arena(arena_),
    impl(arena_),
    blockSize(initialSize)
{
    LOGGER_ASSERT_EXCEPT(blockSize > 0);
    _length = 0;
    tail = impl.construct_front(blockSize, arena.template allocArray<alignment>(blockSize));
}

template<typename InsideArenaType, typename alignment> requires std::is_base_of_v<SG_Allocator::BaseArena,
    InsideArenaType>
SG_Allocator::ULL<InsideArenaType, alignment>::~ULL() {
    if constexpr (std::is_base_of_v<PseudoArena, InsideArenaType>){
        while (impl.length() > 0) {
            arena.softDeleteArray(impl.get_fromFront(0).arr);
            impl.remove_front();
        }
    }
}


/**
 *
 * @return arenaSize_t Maximum capacity of current ULL before a new block is allocated
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
SG_Allocator::arenaSize_t SG_Allocator::ULL<InsideArenaType, alignment>::maxSize() const { return blockSize*impl.length(); }

/**
 *
 * @return arenaSize_t Current number of alignment-sized elements in ULL
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
const SG_Allocator::arenaSize_t& SG_Allocator::ULL<InsideArenaType, alignment>::length() const { return _length; }

/**
 * @brief Sets ULL size to zero and deletes all non-root nodes. Generally won't call destructors
 *
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
void SG_Allocator::ULL<InsideArenaType, alignment>::clear(){
    _length = 0;
    impl.get_fromFront(0).remainingSpace = blockSize;
    shrink();
}

/**
 * @brief Sets ULL size to zero. Maximum capacity is unaffected. Will never call destructors
 *
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
void SG_Allocator::ULL<InsideArenaType, alignment>::softClear(){
    _length = 0;
    impl.get_fromFront(0).remainingSpace = blockSize;
    tail = impl.node_fromFront(0);
}

/**
 * @brief Allocates blocks until the ULL capacity is at least newSize
 *
 * @param newSize Minimum capacity to expand to
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
void SG_Allocator::ULL<InsideArenaType, alignment>::expand(const arenaSize_t& newSize) {
    if (maxSize() >= newSize) return; //Only expand if we need to
    while (maxSize() < newSize)  impl.construct_back(blockSize, arena.template allocArray<alignment>(blockSize));
}

/**
 * @brief Delete all empty blocks
 *
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
void SG_Allocator::ULL<InsideArenaType, alignment>::shrink(){
    if (maxSize() == blockSize) return; //Don't delete the root
    if (_length + blockSize - 1 >= maxSize()) return; //Don't shrink if we're already shrunk

    while ((_length + blockSize - 1) < maxSize() && maxSize() > blockSize){
        arena.softDeleteArray(impl.get_fromBack(0).arr);
        impl.remove_back();
    }
    tail = impl.node_fromBack(0);
    impl.get_atNode(tail).remainingSpace = maxSize() - _length;
}


#define Allocator_ULL \
    if (allocSize > blockSize) {LOGGER_ASSERT_EXCEPT(0); return nullptr;} /*Too big to fit inside one block -> can't allocate*/ \
    auto& t0 = impl.get_atNode(tail); \
    if (t0.remainingSpace < allocSize){ /*Too big to fit inside current block -> make a new block*/ \
        _length += t0.remainingSpace; \
        t0.remainingSpace = 0; \
        expand(_length+allocSize); \
        tail = impl.node_after(tail); \
        impl.get_atNode(tail).remainingSpace = blockSize;\
    } \
    auto& t = impl.get_atNode(tail); \
    auto out = (T*) (&impl.get_atNode(tail).arr[blockSize-(t.remainingSpace)]); \
    _length +=  allocSize; \
    t.remainingSpace -= allocSize; \
    return out;

/**
 * @brief Allocate enough contiguous space within a block for specified data type T, aligned to 'alignment'
 *
 * @tparam T Data type to allocate
 * @return T* Pointer to allocated memory
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
template<typename T>
T* SG_Allocator::ULL<InsideArenaType, alignment>::alloc() {
    arenaSize_t allocSize = sizeof(T)/sizeof(alignment) + (sizeof(T)%sizeof(alignment) > 0);
    Allocator_ULL
}

/**
 * @brief Allocate enough contiguous space within a block for an array of specified data type T, first element aligned to 'alignment'
 *
 * @tparam T Data type to allocate
  * @param arrayLength Number of elements to allocate. May not be aligned to 'align' after the first element.
 * @return T* Pointer to allocated memory
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
template<typename T>
T* SG_Allocator::ULL<InsideArenaType, alignment>::allocArray(const arenaSize_t& arrayLength) {
    arenaSize_t allocSize = sizeof(T)*arrayLength/sizeof(alignment) + (sizeof(T)*arrayLength%sizeof(alignment) > 0);
    Allocator_ULL
}

/**
 * @brief Deallocate elements and shrink if possible. Be careful not to partially deallocate allocations larger than 'alignment'
 *
 * @param deallocAmount Number of alignment-sized elements to deallocate
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena,InsideArenaType>
void SG_Allocator::ULL<InsideArenaType, alignment>::dealloc(const arenaSize_t& deallocAmount) {
    LOGGER_ASSERT_EXCEPT(_length >= deallocAmount);
    _length -= deallocAmount;
    shrink();
}

/**
 * @brief Get pointer to element at given index. Be careful not to 'get' partway through an allocation larger than 'alignment'!
 *
 * @param index Index from start of array, in alignment-sized chunks
 */
template<typename InsideArenaType, typename alignment> requires std::is_base_of_v<SG_Allocator::BaseArena,
    InsideArenaType>
template<typename T>
T* SG_Allocator::ULL<InsideArenaType, alignment>::get(const arenaSize_t& index) {
    LOGGER_ASSERT_EXCEPT(index < _length);
    arenaSize_t i = index / blockSize;
    arenaSize_t ii = index % blockSize;
    return reinterpret_cast<T*>(&(impl.get_fromFront(i).arr[ii]));
}

/**
 * @brief Get pointer to element at given index back from the last element. Be careful not to 'get' partway through an allocation larger than 'alignment'!
 *
 * @param indexFromBack Index from end of array, in alignment-sized chunks
 */
template<typename InsideArenaType, typename alignment> requires std::is_base_of_v<SG_Allocator::BaseArena,
    InsideArenaType>
template<typename T>
T* SG_Allocator::ULL<InsideArenaType, alignment>::getFromBack(const arenaSize_t& indexFromBack) {
    LOGGER_ASSERT_EXCEPT(indexFromBack < _length);
    auto startPoint = blockSize - (impl.get_atNode(tail).remainingSpace);
    if (indexFromBack < startPoint) return reinterpret_cast<T*>(&(impl.get_atNode(tail).arr[startPoint-indexFromBack-1]));

    void* n = impl.node_before(tail);
    arenaSize_t i = indexFromBack-startPoint;
    while (i >= blockSize) {
        LOGGER_ASSERT_EXCEPT(impl.node_before(n) != nullptr);
        n = impl.node_before(n);
        i -= blockSize;
    }
    return reinterpret_cast<T*>(&(impl.get_atNode(n).arr[blockSize - i - 1]));
}
