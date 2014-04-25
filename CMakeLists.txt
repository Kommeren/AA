set(PROJECT_DIR ${CMAKE_SOURCE_DIR})

set(SOURCE_DIR ${PROJECT_DIR}/include)
set(BIN_DIR ${PROJECT_DIR}/bin)
set(TEST_DIR ${PROJECT_DIR}/test)
set(EXAMPLE_DIR ${PROJECT_DIR}/examples)
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_COLOR_MAKEFILE ON)
    
if(${USE_CLANG_AND_DIST})
    SET (CMAKE_CXX_COMPILER  "/usr/lib/distcc/clang++")
    set(USE_CLANG 1)
endif()

if(${USE_CLANG})
    if(NOT USE_CLANG_AND_DIST)
        SET (CMAKE_CXX_COMPILER  "clang++")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-tautological-constant-out-of-range-compare")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu")
    #    ADD_DEFINITIONS("-stdlib=libc++")
    #    set(LIBS c++)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
endif()
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pthread -Wall -fno-strict-aliasing")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -pipe")

set(BOOST_SUBS config functional type_traits detail static_assert utility move
               tuple iterator preprocessor fusion mpl unordered range graph
               assert concept_check integer smart_ptr multi_index exception
               property_map foreach serialization math optional bind typeof
               parameter bimap multi_array conversion array tti function_types
               regex disjoint_sets)

foreach(dir ${BOOST_SUBS})
    include_directories("boost/${dir}/include/")
endforeach()

include_directories (${SOURCE_DIR})

add_subdirectory(examples)
add_subdirectory(test)

