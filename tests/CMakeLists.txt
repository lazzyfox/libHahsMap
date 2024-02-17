cmake_minimum_required(VERSION 3.6)
project(libhashmap_tests VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_COMPILER_VERSION 13)
set(CMAKE_C_COMPILER ${C_COMPILER})
set (CMAKE_CXX_COMPILER ${CXX_COMPILER})
set(CMAKE_CXX_FLAGS "-ggdb -std=c++2b -std=gnu++2b -Wall -Wextra -fPIC -pipe")
# Find additional packages
find_package(GTest CONFIG REQUIRED)
include(CTest)
# Creating unit tests
message(STATUS "Making Unit tests")
add_executable(libhashmap_tests tests.cpp ${PROJECT_SOURCE_DIR}/../src/libHashMap.hpp)
target_link_libraries(libhashmap_tests PRIVATE GTest::gtest -lgcc -lstdc++ -ldl -lm)
# Run unit tests after compilation
enable_testing()
add_test(NAME TestHashMap COMMAND libhashmap_tests)

