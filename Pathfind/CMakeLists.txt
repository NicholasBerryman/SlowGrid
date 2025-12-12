
### Library Files ###
add_library(SG_Pathfind)
target_sources(SG_Pathfind
        PUBLIC
            FILE_SET CXX_MODULES FILES
                Pathfind.ixx
                BreadthFirst.ixx
                PriorityQueue/BasePriorityQueue.ixx
                PriorityQueue/BucketQueue.ixx
                PriorityQueue/BinaryHeap.ixx
                PriorityQueue/HashSetBucketQueue.ixx
                PriorityQueue/HashSetBinaryHeap.ixx
                HashSet/BaseHashSet.ixx
                HashSet/GridRangeHashset.ixx
)
target_link_libraries(SG_Pathfind PRIVATE Logger SG_Allocator SG_Grid)
