include(const.cmake)
set(SOURCE_DIR ${PROJECT_DIR}/include) 
set(BIN_DIR ${PROJECT_DIR}/bin) 
set(TEST_DIR ${PROJECT_DIR}/test) 
set(EXAMPLE_DIR ${PROJECT_DIR}/examples) 
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_COLOR_MAKEFILE ON)
if(${USE_CLANG}) 
    SET (CMAKE_CXX_COMPILER  "clang++")
    #    ADD_DEFINITIONS("-stdlib=libc++")
    #    set(LIBS c++)
endif()
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
include_directories (${TEST_DIR})

ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK) 

#set(CMAKE_BUILD_TYPE Release)

file(GLOB_RECURSE test_bins "${TEST_DIR}/*.cpp")
file(GLOB_RECURSE example_bins "${EXAMPLE_DIR}/*.cpp")

message(${test_bins})

set(LIBS ${LIBS} ${Boost_LIBRARIES} glpk)

foreach(t ${test_bins})
    get_filename_component(n ${t} NAME_WE)
    message("adding test " : ${n})
    add_executable(${n} ${t})
    target_link_libraries (${n} ${LIBS}) 
endforeach()

foreach(t ${example_bins})
    get_filename_component(n ${t} NAME_WE)
    message("adding examaple " : ${n})
    add_executable(${n} ${t})
    target_link_libraries (${n} ${LIBS}) 
endforeach()
