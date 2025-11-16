
### Configs ###
#set(SIZE_LOCAL_T "std::uint_fast32_t" CACHE STRING "Type for LocalDataStructure Sizes" )
#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.ixx.in" "${CMAKE_CURRENT_SOURCE_DIR}/config.ixx")

### Library Files ###
add_library(Logger)
target_include_directories(Logger
        PUBLIC
        include
)
target_sources(Logger
        PUBLIC
            FILE_SET CXX_MODULES FILES
            Logger.ixx
)
target_link_libraries(Logger PRIVATE stdc++exp)
