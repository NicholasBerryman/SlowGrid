
### Library Files ###
add_library(SG_Pathfind)
target_sources(SG_Pathfind
        PUBLIC
            FILE_SET CXX_MODULES FILES
                Pathfind.ixx
                BFS.ixx
                PriorityQueue/BasePriorityQueue.ixx
                PriorityQueue/BucketQueue.ixx
                PriorityQueue/BinaryHeap.ixx
                PriorityQueue/HashMapBucketQueue.ixx
                PriorityQueue/HashMapBinaryHeap.ixx
                PriorityQueue/STDPriorityQueue.ixx
                HashMap/BaseHashMap.ixx
                HashMap/GridRangeHashmap.ixx
                HashMap/STDHashmap.ixx
)
target_link_libraries(SG_Pathfind PRIVATE Logger LocalDataStructures SG_Allocator SG_Grid )
