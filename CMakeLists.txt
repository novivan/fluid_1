project(fluid_simulation)

cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)
include_directories(src/utils)

file(GLOB SOURCES "src/*.cpp" "src/utils/*.cpp")

add_executable(fluid_simulation ${SOURCES})