//
// Created by nickberryman on 1/12/25.
//

module;
#include <type_traits>
#include <concepts>
#include "include/Allocator.h"

export module SG_Allocator:Arena;
import SG_AllocatorConfigs;
import :BaseArena;
import :UnrolledLinkedList;
import :PseudoArena;
import LocalDataStructures;


//TODO add option to return 'ArenaPointers' instead of normal pointers -> can be smaller than a normal pointer, should overload the -> and *() operators, but you can't just get the address of a given value, so might be weaker? -> Also add a NULLPTR equivalend (max value??)
export namespace SG_Allocator {
	PseudoArena globalPseudoArena;

	/**
	 * @brief Arena allocator class with sublifetimes for call-stack-like behaviour
	 * @tparam blockSize Number of bytes per block
	 * @tparam maxSublifetimes Max number of simultaneously open sublifetimes
	 */
	template<std::integral auto blockSize, localSize_t maxSublifetimes, std::integral arenaSize_t_ = defaultArenaSize_t>
    class Arena_ULL {
        static_assert(blockSize > 0, "Blocksize must be > 0");

        public:
		using arenaSize_t = arenaSize_t_;

		Arena_ULL();
		Arena_ULL(const Arena_ULL&) = delete;

    	template<typename T> inline T* alloc();
    	template<typename T> inline T* allocArray(const arenaSize_t& arrayLength);
		template<typename T, typename... ConstructorArgs> inline T* allocConstruct(ConstructorArgs&&... args);
		template<typename T, typename... ConstructorArgs> inline T* allocConstructArray(const arenaSize_t& arrayLength, ConstructorArgs&&... args);

    	inline void sublifetime_open();
    	inline void sublifetime_rollback();
    	inline void sublifetime_softRollback();

    	inline void clear();
    	inline void softClear();
    	inline void expand(const arenaSize_t& newSize);

    	[[nodiscard]] inline arenaSize_t maxSize() const;
    	[[nodiscard]] inline arenaSize_t usedSpace() const;

    	template<typename T> static inline void softDelete(T* toDelete){};
    	template<typename T> static inline void softDeleteArray(T* toDelete){};


		private:
            struct Sublifetime{
                void* rootNode;
                arenaSize_t initialLength;
            };

			ULL<PseudoArena<>, char> impl;
    		LocalDataStructures::Stack<Sublifetime, maxSublifetimes> sublifetimeStack;
    };
	static_assert(OptionalArena_c<Arena_ULL<100,100>, char, char>);
	static_assert(OptionalArena_c<Arena_ULL<100,100>, int, int>);
}

/**
 * @brief Constructs an arena object with an Unrolled Linked List backend
 */
template<std::integral auto blockSize, localSize_t maxSublifetimes, std::integral arenaSize_t_>
SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::Arena_ULL():
	impl(globalPseudoArena, blockSize),
	sublifetimeStack()
{}

/**
 * @brief Allocate some contiguous memory in the current sublifetime - DOES NOT CALL CONSTRUCTORS
 * @tparam T Type to allocate space for
 * @return Pointer to allocated memory of type T*
 */
template<std::integral auto blockSize, localSize_t maxSublifetimes, std::integral arenaSize_t_>
template<typename T>
T * SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::alloc() {
#ifdef DISABLE_NONFUNDAMENTAL_ALLOC
#ifndef NDEBUG
	static_assert(std::is_fundamental_v<T> || (std::is_standard_layout_v<T> && std::is_trivially_default_constructible_v<T> && std::is_trivially_copyable_v<T>) || std::is_pointer_v<T>);
#endif
#endif
	return impl.alloc<T>();
}

/**
 * @brief Allocate some contiguous array memory in the current sublifetime - DOES NOT CALL CONSTRUCTORS
 * @tparam T Type to allocate space for
 * @param arrayLength Length of array to allocate
 * @return Pointer to allocated array (address of first element)
 */
template<std::integral auto blockSize, localSize_t maxSublifetimes, std::integral arenaSize_t_>
template<typename T>
T * SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::allocArray(const arenaSize_t& arrayLength) {
	return impl.allocArray<T>(arrayLength);
}

/**
 * @brief Allocate some contiguous memory in the current sublifetime - calls constructor with specified arguments (including empty arguments)
 * @tparam T Type to allocate space for
 * @tparam ConstructorArgs Types of constructor args (can be inferred from args, can be empty)
 * @param args Values of constructor args (can be empty)
 * @return Pointer to allocated memory of type T*
 */
template<std::integral auto blockSize, localSize_t maxSublifetimes, std::integral arenaSize_t_>
template<typename T, typename ... ConstructorArgs>
T * SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::allocConstruct(ConstructorArgs&&... args) {
	return new (impl.alloc<T>()) T(args...);
}

/**
 * @brief Allocate some contiguous array memory in the current sublifetime - calls constructor with specified arguments on each element (including empty arguments)
 * @tparam T Type to allocate space for
 * @tparam ConstructorArgs Types of constructor args (can be inferred from args, can be empty)
 * @param arrayLength Length of array to allocate
 * @param args Values of constructor args (can be empty)
 * @return Pointer to allocated array (address of first element)
 */
template<std::integral auto blockSize, localSize_t maxSublifetimes, std::integral arenaSize_t_>
template<typename T, typename ... ConstructorArgs>
T * SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::allocConstructArray(const arenaSize_t& arrayLength, ConstructorArgs&&... args) {
	T* out = allocArray<T>(arrayLength);
	for (auto i = 0; i < arrayLength; i++) new (&out[i]) T(args...);
	return out;
}

/**
 * @brief Marks all memory in arena as overwritable, and deallocates non-root blocks - DOES NOT CALL DESTRUCTORS
 */
template<std::integral auto blockSize, localSize_t maxSublifetimes, std::integral arenaSize_t_>
void SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::clear() {
	impl.clear();
	sublifetimeStack.clear();
}

/**
 * @brief Marks all memory in arena as overwritable, but does not deallocate non-root blocks - DOES NOT CALL DESTRUCTORS
 */
template<std::integral auto blockSize, localSize_t maxSublifetimes, std::integral arenaSize_t_>
void SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::softClear() {
	impl.softClear();
	sublifetimeStack.clear();
}

/**
 * @brief Pre-allocates enough blocks to fit the specified number of bytes
 * @param newSize Size to allocate space up to
 */
template<std::integral auto blockSize, localSize_t maxSublifetimes, std::integral arenaSize_t_>
void SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::expand(const arenaSize_t& newSize) {
	impl.expand(newSize);
}

/**
 *
 * @return Max number of bytes that can be allocated before a new block is required
 */
template<std::integral auto blockSize, localSize_t maxSublifetimes, std::integral arenaSize_t_>
SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::arenaSize_t SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::maxSize() const {
	return impl.maxSize();
}

/**
 *
 * @return Current number of bytes that have been allocated within the Arena. Includes fragmented memory that may occur at block tails
 */
template<std::integral auto blockSize, localSize_t maxSublifetimes, std::integral arenaSize_t_>
SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::arenaSize_t SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::usedSpace() const {
	return impl.length();
}

/**
 * @brief Begin a sublifetime and add it to the top of the sublifetime stack
 */
template<std::integral auto blockSize, localSize_t maxSublifetimes, std::integral arenaSize_t_>
void SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::sublifetime_open() {
	Sublifetime sl;
	sl.initialLength = impl.length();
	sl.rootNode = NodeExaminer<PseudoArena<>, char>::currentTail(impl);
	sublifetimeStack.push(sl);
}

/**
 * @brief Pop a sublifetime from the sublifetime stack and deallocate memory allocated and blocks created since it opened. DOES NOT CALL DESTRUCTORS
 */
template<std::integral auto blockSize, localSize_t maxSublifetimes, std::integral arenaSize_t_>
void SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::sublifetime_rollback(){
	Sublifetime rollback = sublifetimeStack.pop();
	NodeExaminer<PseudoArena<>, char>::setTail(impl, rollback.rootNode, rollback.initialLength);
	NodeExaminer<PseudoArena<>, char>::shrink(impl);
}

/**
 * Pop a sublifetime from the sublifetime stack and deallocate memory allocated but not blocks created since it opened. DOES NOT CALL DESTRUCTORS
 */
template<std::integral auto blockSize, localSize_t maxSublifetimes, std::integral arenaSize_t_>
void SG_Allocator::Arena_ULL<blockSize, maxSublifetimes, arenaSize_t_>::sublifetime_softRollback(){
	Sublifetime rollback = sublifetimeStack.pop();
	NodeExaminer<PseudoArena<>, char>::setTail(impl, rollback.rootNode, rollback.initialLength);
}

