### Project Setup ###
cmake_minimum_required(VERSION 4.0)
project(SlowGrid)
set(CMAKE_CXX_STANDARD 26)

### Subdirectories ###
add_subdirectory(Logger)
add_subdirectory(LocalDataStructures)
add_subdirectory(Allocator)

### Main Library Definition ###
add_library(SlowGrid)
target_link_libraries(SlowGrid
        PUBLIC
            Logger
            LocalDataStructures
            SG_Allocator
)
target_sources(SlowGrid
        PUBLIC
            FILE_SET CXX_MODULES FILES
                SlowGrid.ixx
)

### Testing Setup ###
ENABLE_TESTING()
ADD_SUBDIRECTORY(tests)

