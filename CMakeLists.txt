cmake_minimum_required(VERSION 3.22)
project(math)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_library(mt-maths STATIC src/mtmath_c.cpp src/mtmath_c.h src/impl/rational.cpp src/impl/rational.h src/impl/big_int.cpp src/impl/big_int.h src/impl/byte_array.cpp src/impl/byte_array.h src/include.hpp)

add_executable(mt-maths-tests tests/main.cpp tests/rationals.cpp tests/big_int.cpp tests/byte_array.cpp tests/c_bindings/big_int.cpp)
target_link_libraries(mt-maths-tests PUBLIC mt-maths)
target_include_directories(mt-maths-tests PUBLIC src)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src)
