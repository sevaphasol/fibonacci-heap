cmake_minimum_required( VERSION 3.14 )
project( FHeap )

set( CMAKE_CXX_STANDARD            17 )
set( CMAKE_CXX_STANDARD_REQUIRED   ON )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

find_package( GTest     REQUIRED )
find_package( benchmark REQUIRED )
find_package( Boost     REQUIRED )

set( TEST_DIR  test  )
set( BENCH_DIR bench )

add_executable( fheap_test  ${TEST_DIR}/fheap_test.cpp   )
add_executable( fheap_bench ${BENCH_DIR}/fheap_bench.cpp )

target_link_libraries( fheap_test  GTest::gtest         GTest::gtest_main         )
target_link_libraries( fheap_bench benchmark::benchmark benchmark::benchmark_main )

target_include_directories( fheap_test  PRIVATE fheap                       )
target_include_directories( fheap_bench PRIVATE fheap ${Boost_INCLUDE_DIRS} )
