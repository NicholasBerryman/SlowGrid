//
// Created by nickberryman on 1/12/25.
//

module;
#include <type_traits>
export module SG_Allocator:UnrolledLinkedList;
import :BaseArena;
import SG_AllocatorConfigs;
import Logger;

export namespace SG_Allocator {
	template<typename InsideArenaType, typename alignment = char>
	requires std::is_base_of_v<BaseArena, InsideArenaType>
	class NodeExaminer;

	/**
	 * @brief Heterogenous unrolled doubly-linked list, with arena allocation support
	 * 
	 * @tparam InsideArenaType Arena type to allocated blocks into (PseudoArena if head allocation preferred)
	 * @tparam alignment Data type to use for element size. e.g. char gives 1 byte per element, uint64_t give 4 bytes per element.
	 */
    template<typename InsideArenaType, typename alignment = char>
	requires std::is_base_of_v<BaseArena, InsideArenaType>
    class ULL {
    	friend class NodeExaminer<InsideArenaType, alignment>;
    public:
        ULL(InsideArenaType& arena, const arenaSize_t initialSize);
        template<typename T> inline T* alloc();
        template<typename T> inline T* allocArray(const arenaSize_t arrayLength);

		inline void dealloc(arenaSize_t deallocAmount);
        inline void clear();
        inline void softClear();
        inline void expand(const arenaSize_t newSize);

    	template<typename T> inline T* get(arenaSize_t index);

        [[nodiscard]] inline arenaSize_t maxSize() const;
        [[nodiscard]] inline arenaSize_t length() const;

    private:
    	struct Node{
    		Node* previous;
    		Node* next;
    		arenaSize_t remainingSpace; //Maybe benchmark using this vs modulo operations??
    		alignment* arr;
    	};

        InsideArenaType& arena;
        Node root;
        Node* tail;
        arenaSize_t _length;
        arenaSize_t size;
        const arenaSize_t blockSize;

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
		static void setTail(ULL<InsideArenaType, alignment>& ull, void* newTail, arenaSize_t newLength) {
			ull.tail = static_cast<ULL<InsideArenaType, alignment>::Node*>(newTail);
			ull._length = newLength;
			arenaSize_t effectiveCapacity = newLength/ull.blockSize * ull.blockSize;
			if (newLength > effectiveCapacity) effectiveCapacity += ull.blockSize;
			if (effectiveCapacity <ull. blockSize) effectiveCapacity = ull.blockSize;
			ull.tail->remainingSpace = effectiveCapacity - newLength;
		}
	};
}

/**
 * @brief Default constructor for ULL
 * 
 * @param arena Arena object to use for block allocations
 * @param initialSize Size of blocks, including the initial block
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
SG_Allocator::ULL<InsideArenaType, alignment>::ULL(InsideArenaType& arena, const arenaSize_t initialSize):
    arena(arena),
	blockSize(initialSize)
{
	Logging::assert_except(blockSize > 0);
    _length = 0;
    size = initialSize;
    root.next = nullptr;
    root.previous = nullptr;
    root.arr = arena.template allocArray<alignment>(blockSize);
    root.remainingSpace = blockSize;
    tail = &root;
}


/**
 * 
 * @return arenaSize_t Maximum capacity of current ULL before a new block is allocated
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
SG_Allocator::arenaSize_t SG_Allocator::ULL<InsideArenaType, alignment>::maxSize() const { return size; }

/**
 * 
 * @return arenaSize_t Current number of alignment-sized elements in ULL
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
SG_Allocator::arenaSize_t SG_Allocator::ULL<InsideArenaType, alignment>::length() const { return _length; }

/**
 * @brief Sets ULL size to zero and deletes all non-root nodes. Generally won't call destructors
 * 
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
void SG_Allocator::ULL<InsideArenaType, alignment>::clear(){
	_length = 0;
	root.remainingSpace = blockSize;
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
	root.remainingSpace = blockSize;
	tail = &root;
}

/**
 * @brief Allocates blocks until the ULL capacity is at least newSize
 * 
 * @param newSize Minimum capacity to expand to
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
void SG_Allocator::ULL<InsideArenaType, alignment>::expand(const arenaSize_t newSize) {
	if (size >= newSize) return; //Only expand if we need to
	Node* n = tail;
	while (size < newSize){
		if (n->next == nullptr) {
			n->next = arena.template alloc<Node>();
			n->next->previous = n;
			n->next->next = nullptr;
			n->next->arr = arena.template allocArray<alignment>(blockSize);
		}
		n = n->next;
		n->remainingSpace = blockSize;
		size += blockSize;
	}
}

/**
 * @brief Delete all empty blocks
 * 
 */
template<typename InsideArenaType, typename alignment>
requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
void SG_Allocator::ULL<InsideArenaType, alignment>::shrink(){
	if (size == blockSize) return; //Don't delete the root
	if (_length + blockSize - 1 >= size) return; //Don't shrink if we're already shrunk

	while (tail->next) tail = tail->next;
	while ((_length + blockSize - 1) < size && size > blockSize){
		arena.softDeleteArray(tail->arr);
		tail = tail->previous;
		arena.softDelete(tail->next);
		size -= blockSize;
	}
	tail->next = nullptr;
	tail->remainingSpace = size - _length;
}


#define Allocator_ULL \
	if (allocSize > blockSize) {Logging::assert_except(0); return nullptr;} /*Too big to fit inside one block -> can't allocate*/ \
	if (tail->remainingSpace < allocSize){ /*Too big to fit inside current block -> make a new block*/ \
		_length += tail->remainingSpace; \
		expand(_length+allocSize); \
		if (tail->next) { \
			tail = tail->next; \
			tail->remainingSpace = blockSize; \
		} \
	} \
	T* out = (T*) (&tail->arr[blockSize-(tail->remainingSpace)]); \
	tail->remainingSpace -= allocSize; \
	_length += allocSize; \
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
T* SG_Allocator::ULL<InsideArenaType, alignment>::allocArray(const arenaSize_t arrayLength) {
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
void SG_Allocator::ULL<InsideArenaType, alignment>::dealloc(const arenaSize_t deallocAmount) {
	Logging::assert_except(_length >= deallocAmount);
	_length -= deallocAmount;
	shrink();
}

/**
 * @brief Get pointer to element at given index. Be careful not to 'get' partway through an allocation larger than 'alignment'
 * 
 * @param index Index from start of array, in alignment-sized chunks
 */
template<typename InsideArenaType, typename alignment> requires std::is_base_of_v<SG_Allocator::BaseArena,
	InsideArenaType>
template<typename T>
T* SG_Allocator::ULL<InsideArenaType, alignment>::get(const arenaSize_t index) {
	Logging::assert_except(index < _length);
	arenaSize_t i = 0;
	Node* n = &root;
	while ((index - i) >= blockSize) {
		Logging::assert_except(n->next != nullptr);
		n = n->next;
		i += blockSize;
	}
	return reinterpret_cast<T*>(&(n->arr[index - i]));
}
