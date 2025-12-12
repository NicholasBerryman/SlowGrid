
### Configs ###
set(ARENA_SIZE_T "std::uint32_t" CACHE STRING "Type for max Arena size" )
set(DISABLE_NONFUNDAMENTAL_ALLOC "1" CACHE STRING "When set - prevents compilation when non-contructor allocator called on non-fundamental/trivial/pointer type" )
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.ixx.in" "${CMAKE_CURRENT_BINARY_DIR}/config.ixx")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/Allocator.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/Allocator.h")

### Library Files ###
add_library(SG_Allocator)
target_sources(SG_Allocator
        PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" FILES
        ${CMAKE_CURRENT_BINARY_DIR}/config.ixx
        SG_Allocator.ixx
        BaseArena.ixx
        UnrolledLinkedList.ixx
        UnrolledLinkedList2.ixx
        PseudoArena.ixx
        Arena_ULL.ixx
        CachedQueue.ixx
)
target_include_directories(SG_Allocator PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/")
target_link_libraries(SG_Allocator PRIVATE LocalDataStructures Logger)
