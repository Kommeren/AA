#adding unit_test boost library
set(UNIT_TEST_FNAMES compiler_log_formatter debug exception_safety
                     execution_monitor framework interaction_based
                     logged_expectations plain_report_formatter
                     progress_monitor results_collector
                     results_reporter test_tools unit_test_log
                     unit_test_main unit_test_monitor
                     unit_test_parameters unit_test_suite
                     xml_log_formatter xml_report_formatter)

foreach(name ${UNIT_TEST_FNAMES})
    SET(unit_test_bins ${unit_test_bins}  ${PROJECT_DIR}/boost/test/src/${name}.cpp)
endforeach()

ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK=1 -DBOOST_ALL_NO_LIB=1) 
add_library(unit_test SHARED ${unit_test_bins})

#adding tests
file(GLOB_RECURSE test_bins "${TEST_DIR}/*.cpp")
link_directories(${TEST_DIR})

include_directories (${TEST_DIR})

#we have to link against unit_test lib
set(TEST_LIBS ${LIBS} unit_test  glpk)

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
        if(${t} MATCHES ".*iterative_rounding.*" OR ${t} MATCHES ".*linear_programming.*") 
            target_link_libraries (${n} ${TEST_LIBS} glpk) 
        else()
            target_link_libraries (${n} ${TEST_LIBS}) 
        endif()
    endif()
endforeach()

add_executable(all_small_tests ${short_tests})
target_link_libraries(all_small_tests ${TEST_LIBS})

add_executable(all_long_tests ${long_tests})
target_link_libraries(all_long_tests ${TEST_LIBS})
