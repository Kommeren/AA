include(const.cmake)
set(SOURCE_DIR ${PROJECT_DIR}/lib) 
set(BIN_DIR ${PROJECT_DIR}/bin) 
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_CXX_FLAGS "-pthread -Wall -fno-strict-aliasing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -pipe")

include_directories (${SOURCE_DIR})
include_directories (${BOOST_DIR})

add_executable (main  ${BIN_DIR}/main.cpp)
# Link the executable to the Hello library.
target_link_libraries (main) 
