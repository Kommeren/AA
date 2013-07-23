file(GLOB_RECURSE test_bins "${TEST_DIR}/*.cpp")

#we have to link against unit_test lib
link_directories(${Boost_LIBRARY_DIRS})

ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK) 
include_directories (${TEST_DIR})

#adding tests
foreach(t ${test_bins})
    set(TEST_LIBS ${LIBS} ${Boost_LIBRARIES})

    get_filename_component(n ${t} NAME_WE)
    message("adding test " : ${n})
    add_executable(${n} ${t})
    if(${t} MATCHES ".*iterative_rounding.*") 
        target_link_libraries (${n} ${TEST_LIBS} glpk) 
    else()
        target_link_libraries (${n} ${TEST_LIBS}) 
    endif()
endforeach()
