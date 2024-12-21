cmake_minimum_required(VERSION 3.10)
project(modernized_fluid)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

include_directories(${CMAKE_SOURCE_DIR}/headers)

set(SIZES "S(36,84),S(10,10),S(1,1)")
set(TYPES "FIXED(32,16),FAST_FIXED(32,16),DOUBLE,FLOAT,FIXED(8,5),FAST_FIXED(5,2)")

add_executable(modernized_fluid modernized.cpp)
target_compile_definitions(modernized_fluid PUBLIC -DSIZES=${SIZES} -DTYPES=${TYPES})

add_executable(initial fluid.cpp)