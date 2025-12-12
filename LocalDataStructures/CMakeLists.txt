
### Configs ###
set(SIZE_LOCAL_T "std::uint_fast32_t" CACHE STRING "Type for LocalDataStructure Sizes" )
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.ixx.in" "${CMAKE_CURRENT_BINARY_DIR}/config.ixx")

### Library Files ###
add_library(LocalDataStructures)
target_sources(LocalDataStructures
        PUBLIC
            FILE_SET CXX_MODULES BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" FILES
                ${CMAKE_CURRENT_BINARY_DIR}/config.ixx
                LocalDataStructures.ixx
                Stack.ixx
                Queue.ixx
                Dequeue.ixx
)
target_link_libraries(LocalDataStructures PRIVATE Logger)
