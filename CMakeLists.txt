#[===[
cmake_minimum_required(VERSION 3.10)

project(FluidSimulation)

# Установить стандарт C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Определение переменных
set(TYPES "" CACHE STRING "Типы данных")
set(SIZES "" CACHE STRING "Размеры поля")

# Печать значений переменных
message(STATUS "TYPES: ${TYPES}")
message(STATUS "SIZES: ${SIZES}")

# Генерация кода на основе типов и размеров
file(WRITE ${CMAKE_BINARY_DIR}/generated_code.hpp "// Сгенерированный код\n")
foreach(TYPE IN LISTS TYPES)
    file(APPEND ${CMAKE_BINARY_DIR}/generated_code.hpp "#include <${TYPE}.hpp>\n")
endforeach()

foreach(SIZE IN LISTS SIZES)
    file(APPEND ${CMAKE_BINARY_DIR}/generated_code.hpp "#define SIZE_${SIZE}\n")
endforeach()

# Добавление исходных файлов
add_executable(fluid_simulation src/main.cpp src/fluid.hpp src/fluid.cpp ${CMAKE_BINARY_DIR}/generated_code.hpp)

]===]

cmake_minimum_required(VERSION 3.10)
project(fluid_simulation)

set(CMAKE_CXX_STANDARD 17)

# Parse TYPES and SIZES
if (DEFINED TYPES)
    foreach(type ${TYPES})
        string(TOUPPER ${type} type_upper)
        add_definitions(-D${type_upper})
    endforeach()
endif()

if (DEFINED SIZES)
    foreach(size ${SIZES})
        string(REPLACE "(" "_" size ${size})
        string(REPLACE "," "_" size ${size})
        string(REPLACE ")" "" size ${size})
        add_definitions(-D${size})
    endforeach()
endif()

# Add your source files here
add_executable(fluid_simulation src/main.cpp src/parameter_selector.hpp src/types.hpp)