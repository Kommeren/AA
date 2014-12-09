
include_directories (${PROJECT_DIR})
file(GLOB_RECURSE bin_srcs "${BIN_DIR}/*.cpp")
add_custom_target(bins)

add_definitions(-DBOOST_ALL_DYN_LINK=1 -DBOOST_ALL_NO_LIB=1)
file(GLOB_RECURSE boost_program_options "${PROJECT_DIR}/boost/program_options/src/*.cpp")
add_library(program_options SHARED ${boost_program_options})

foreach(bin_src ${bin_srcs})
    get_filename_component(bin_name ${bin_src} NAME_WE)
    message("adding binary" : ${bin_name})
    add_executable(${bin_name} ${bin_src})
    target_link_libraries (${bin_name} ${LIBS} program_options)

    add_dependencies(bins ${bin_name})

endforeach()
