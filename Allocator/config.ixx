//
// Created by nickberryman on 1/12/25.
//
module;
#include <cstdint>
#define ARENA_SIZE_T uint32_t
#define SUBLIFETIME_DEPTH_T uint_fast8_t
#define CACHEQUEUE_CACHESIZE_T std::uint_fast8_t
export module SG_AllocatorConfigs;

export namespace SG_Allocator {
    typedef ARENA_SIZE_T arenaSize_t;
    typedef SUBLIFETIME_DEPTH_T sublifetimeDepth_t;
    typedef CACHEQUEUE_CACHESIZE_T cacheSize_t;
}

