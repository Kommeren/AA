
include_directories (${PROJECT_DIR})
file(GLOB_RECURSE example_bins "${EXAMPLE_DIR}/*.cpp")

#adding examples
foreach(t ${example_bins})
    get_filename_component(n ${t} NAME_WE)
    message("adding examaple " : ${n})
    add_executable(${n} ${t})
    if(${t} MATCHES ".*iterative_rounding.*") 
        target_link_libraries (${n} ${LIBS} glpk) 
    else()
        target_link_libraries (${n} ${LIBS}) 
    endif()
endforeach()
