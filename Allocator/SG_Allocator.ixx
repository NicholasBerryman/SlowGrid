//
// Created by nickberryman on 1/12/25.
//

module;
#include <concepts>

export module SG_Allocator;
export import :BaseArena;
export import :Arena;
export import :CachedQueue;
export import :UnrolledLinkedList;
export import :UnrolledLinkedList2;
export import :PseudoArena;
export import SG_AllocatorConfigs;
export import :LinkedList2;
export import :RuntimeArray;

export namespace SG_Allocator { template <std::integral auto blockSize = 2048u, std::integral auto sublifetimeDepth = 16u> using defaultArena = SG_Allocator::Arena_ULL<blockSize, sublifetimeDepth>; }
