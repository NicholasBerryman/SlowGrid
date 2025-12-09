project(SlowGrid_tests)

function(newTest testName)
    add_executable(test_${testName} test_${testName}.cpp)
    target_link_libraries(test_${testName} PRIVATE SlowGrid)
    add_test(${testName} test_${testName})
endfunction()

# -- Local Stack --
newTest(Logger)
newTest(LocalStack)
newTest(LocalQueue)
newTest(LocalDequeue)
newTest(UnrolledLinkedList)
newTest(Arena)
newTest(CachedQueue)
newTest(GridPoint)
newTest(FullGrid)
newTest(BitfieldGrid)
newTest(SparseGrid)


