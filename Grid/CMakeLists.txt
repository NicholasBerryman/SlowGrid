
### Configs ###
set(COORDINATE_T "std::int32_t" CACHE STRING "Type for coordinate x/y values" )
set(FLOAT_DISTANCE_T "double" CACHE STRING "Type for floating point distance/bearing values" )
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.ixx.in" "${CMAKE_CURRENT_SOURCE_DIR}/config.ixx")

### Library Files ###
add_library(SG_Grid)
target_sources(SG_Grid
        PUBLIC
        FILE_SET CXX_MODULES FILES
        Grid.ixx
		config.ixx
		Point.ixx
		BaseGrid.ixx
		BitfieldFullGrid.ixx
		FullGrid.ixx
		SparseGrid.ixx
)
target_link_libraries(SG_Grid PRIVATE LocalDataStructures Logger)
