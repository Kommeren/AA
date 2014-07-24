
include_directories (${PROJECT_DIR})
file(GLOB_RECURSE example_bins "${EXAMPLE_DIR}/*.cpp")

#adding examples
if(${COMPILATION_ERROR_TESTS})
    # no examples
else()
    foreach(t ${example_bins})
        get_filename_component(n ${t} NAME_WE)
        message("adding example " : ${n})
        add_executable(${n} ${t})
        if(${t} MATCHES ".*iterative_rounding.*" OR ${t} MATCHES ".*linear_programming.*" OR ${t} MATCHES ".*fractional_winner_determination_in_MUCA*")
            target_link_libraries (${n} ${LIBS} glpk)
        else()
            target_link_libraries (${n} ${LIBS})
        endif()
    endforeach()
endif()
