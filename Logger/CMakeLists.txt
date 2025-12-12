
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
