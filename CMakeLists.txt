### Project Setup ###
cmake_minimum_required(VERSION 4.0)
project(SlowGrid)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options("-fconcepts-diagnostics-depth=100")
endif()
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++>")
    add_link_options("$<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++>")
    add_link_options("-static-libstdc++")
endif()


### Subdirectories ###
add_subdirectory(Logger)
add_subdirectory(LocalDataStructures)
add_subdirectory(Allocator)
add_subdirectory(Grid)
add_subdirectory(Pathfind)

### Main Library Definition ###
add_library(SlowGrid)
target_link_libraries(SlowGrid
        PUBLIC
        Logger
        LocalDataStructures
        SG_Allocator
        SG_Grid
        SG_Pathfind
)
target_sources(SlowGrid
        PUBLIC
        FILE_SET CXX_MODULES FILES
        SlowGrid.ixx
)

### Testing Setup ###
ENABLE_TESTING()
ADD_SUBDIRECTORY(tests)


### Custom Build Types ###
set(CMAKE_CXX_FLAGS_RELWITHDEBUGANDASSERT "-g -O3" CACHE STRING
        "Flags used by the C++ compiler during RelWithDebugAndAssert builds."
        FORCE)
mark_as_advanced(CMAKE_CXX_FLAGS_RELWITHDEBUGANDASSERT)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELWITHDEBUGANDASSERT "${CMAKE_CXX_FLAGS_RELWITHDEBUGANDASSERT} -fsanitize=address -g")
