cmake_minimum_required(VERSION 3.10)
project(fluid_simulation)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(fluid_simulation
    src/main.cpp
    src/Simulator.cpp
)
