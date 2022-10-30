cmake_minimum_required(VERSION 3.22)
project(math)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_library(mt-maths STATIC src/mtmath_c.cpp src/mtmath_c.h src/impl/rational.cpp src/impl/rational.h src/impl/big_int.cpp src/impl/big_int.h src/impl/byte_array.cpp src/impl/byte_array.h)

add_executable(mt-maths-tests tests/main.cpp tests/rationals.cpp tests/big_int.cpp tests/byte_array.cpp)
target_link_libraries(mt-maths-tests PUBLIC mt-maths)
target_include_directories(mt-maths-tests PUBLIC src)

option(BUILD_PYTHON "Build python libraries" ON)
option(BUILD_JAVA "Build java libraries" ON)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src)

if (BUILD_PYTHON OR BUILD_JAVA OR BUILD_RUBY)
    FIND_PACKAGE(SWIG REQUIRED)
    INCLUDE(${SWIG_USE_FILE})
    SET(CMAKE_SWIG_FLAGS "")
endif()

if (BUILD_PYTHON)
    FIND_PACKAGE(PythonLibs)
    find_package (Python3 COMPONENTS Interpreter)
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
    add_subdirectory(python)
endif()

if (BUILD_JAVA)
    find_package(Java REQUIRED)
    find_package(JNI REQUIRED)
    include(UseJava)
    include_directories(${JNI_INCLUDE_DIRS})
    add_subdirectory(java)
endif()

#SWIG_ADD_LIBRARY(pyMtMaths python src/placeholder.i)
#SWIG_LINK_LIBRARIES(pyMtMaths ${PYTHON_LIBRARIES} mt-maths)
