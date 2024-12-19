cmake_minimum_required(VERSION 3.10)
project(fluid_simulation)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)


# Добавление исполняемого файла и связывание с библиотекой
add_executable(fluid_simulation
    src/main.cpp
    src/Simulator.cpp
)

# Удаляем связывание с несуществующей библиотекой
# target_link_libraries(fluid_simulation PRIVATE explicit_templates)
