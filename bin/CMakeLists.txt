
include_directories (${PROJECT_DIR})
file(GLOB_RECURSE sources_bin "${BIN_DIR}/*.cpp")

add_definitions(-DBOOST_ALL_DYN_LINK=1 -DBOOST_ALL_NO_LIB=1)
file(GLOB_RECURSE boost_program_options "${PROJECT_DIR}/boost/program_options/src/*.cpp")
add_library(program_options SHARED ${boost_program_options})

foreach(t ${sources_bin})
    get_filename_component(n ${t} NAME_WE)
    message("compiling binary" : ${n})
    add_executable(${n} ${t})
    target_link_libraries (${n} ${LIBS} program_options)

endforeach()
