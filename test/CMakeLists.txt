file(GLOB_RECURSE test_bins "${TEST_DIR}/*.cpp")

#we have to link against unit_test lib
link_directories(${Boost_LIBRARY_DIRS})

ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK) 
include_directories (${TEST_DIR})
#adding tests
set(TEST_LIBS ${LIBS} ${Boost_LIBRARIES})

foreach(t ${test_bins})
    get_filename_component(n ${t} NAME_WE)
    message("adding test " : ${n})
    if(${n} MATCHES ".*test.*")
        if(${t} MATCHES ".*long.*")
            set(long_tests ${long_tests}  ${t})
        else()
            set(short_tests ${short_tests}  ${t})
        endif()
    else()
        add_executable(${n} ${t})
        if(${t} MATCHES ".*iterative_rounding.*") 
            target_link_libraries (${n} ${TEST_LIBS} glpk) 
        else()
            target_link_libraries (${n} ${TEST_LIBS}) 
        endif()
    endif()
endforeach()

set(TEST_LIBS ${TEST_LIBS} glpk)

add_executable(all_small_tests ${short_tests})
target_link_libraries(all_small_tests ${TEST_LIBS})

add_executable(all_long_tests ${long_tests})
target_link_libraries(all_long_tests ${TEST_LIBS})
