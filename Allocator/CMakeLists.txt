
### Configs ###
set(ARENA_SIZE_T "std::uint32_t" CACHE STRING "Type for max Arena size" )
set(SUBLIFETIME_DEPTH_T "std::uint_fast8_t" CACHE STRING "Type for max number of alive Arena sublifetimes" )
set(CACHEQUEUE_CACHESIZE_T "std::uint_fast8_t" CACHE STRING "Type for max number of cached CachedQeueue elements" )
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.ixx.in" "${CMAKE_CURRENT_SOURCE_DIR}/config.ixx")

### Library Files ###
add_library(SG_Allocator)
target_sources(SG_Allocator
        PUBLIC
        FILE_SET CXX_MODULES FILES
        config.ixx
        SG_Allocator.ixx
        BaseArena.ixx
        UnrolledLinkedList.ixx
        PseudoArena.ixx
        #Arena.ixx
        #CachedQueue.ixx
)
target_link_libraries(SG_Allocator PRIVATE LocalDataStructures)
