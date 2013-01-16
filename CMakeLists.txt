include(const.cmake)
set(SOURCE_DIR ${PROJECT_DIR}/lib) 
set(BIN_DIR ${PROJECT_DIR}/bin) 
set(TEST_DIR ${PROJECT_DIR}/test) 
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_CXX_FLAGS "-pthread -Wall -fno-strict-aliasing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -pipe")

# Boost
find_package(Boost 1.52 COMPONENTS unit_test_framework REQUIRED)
MESSAGE("Boost information:")
MESSAGE("  Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
MESSAGE("  Boost_LIBRARIES: ${Boost_LIBRARIES}")
MESSAGE("  Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories (${SOURCE_DIR})
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK) 
#include_directories (${BOOST_DIR})

add_executable (main  ${BIN_DIR}/main.cpp)
add_executable (test  ${TEST_DIR}/cycle_manager_test.cpp)
# Link the executable to the Hello library.
target_link_libraries (main) 
target_link_libraries (test ${Boost_LIBRARIES}) 
