include(const.cmake)
set(SOURCE_DIR ${PROJECT_DIR}/include) 
set(BIN_DIR ${PROJECT_DIR}/bin) 
set(TEST_DIR ${PROJECT_DIR}/test) 
set(EXAMPLE_DIR ${PROJECT_DIR}/examples) 
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_COLOR_MAKEFILE ON)
if(${USE_CLANG}) 
    SET (CMAKE_CXX_COMPILER  "clang++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-tautological-constant-out-of-range-compare")
    
    #    ADD_DEFINITIONS("-stdlib=libc++")
    #    set(LIBS c++)
endif()
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pthread -Wall -fno-strict-aliasing")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -pipe")

# Boost
find_package(Boost 1.52 COMPONENTS unit_test_framework REQUIRED)
MESSAGE("Boost information:")
MESSAGE("  Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
MESSAGE("  Boost_LIBRARIES: ${Boost_LIBRARIES}")
MESSAGE("  Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
include_directories(${Boost_INCLUDE_DIRS})

include_directories (${SOURCE_DIR})

add_subdirectory(examples)
add_subdirectory(test)

