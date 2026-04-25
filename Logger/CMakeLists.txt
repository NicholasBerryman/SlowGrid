add_library(Logger)


### External libraries ###
include(FetchContent)

### cpptrace (since llvm libc++ doesn't support std::stacktrace)
FetchContent_Declare(
        cpptrace
        GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
        GIT_TAG        v1.0.4 # <HASH or TAG>
)
FetchContent_MakeAvailable(cpptrace)


### Library Files ###
target_include_directories(Logger
        PUBLIC
        include
)
target_sources(Logger
        PUBLIC
        FILE_SET CXX_MODULES FILES
        Logger.ixx
)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(Logger PRIVATE stdc++exp cpptrace::cpptrace)
endif()
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_link_libraries(Logger PRIVATE cpptrace::cpptrace)
endif()


if(WIN32)
    add_custom_command(
            TARGET Logger POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:cpptrace::cpptrace>
            $<TARGET_FILE_DIR:Logger>
    )
endif()