add_custom_target(tests)

#adding unit_test boost library
# files taken from boost build - it does not compile if one take all of the *.cpp files
set(UNIT_TEST_FNAMES compiler_log_formatter debug exception_safety
                     execution_monitor framework interaction_based
                     logged_expectations plain_report_formatter
                     progress_monitor results_collector
                     results_reporter test_tools unit_test_log
                     unit_test_main unit_test_monitor
                     unit_test_parameters unit_test_suite
                     xml_log_formatter xml_report_formatter)

foreach(name ${UNIT_TEST_FNAMES})
    SET(unit_test_srcs ${unit_test_srcs}  ${PAAL}/boost/test/src/${name}.cpp)
endforeach()

ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK=1 -DBOOST_ALL_NO_LIB=1)
add_library(unit_test SHARED ${unit_test_srcs})

#adding tests
file(GLOB_RECURSE test_srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
link_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

#we have to link against unit_test lib
set(TEST_LIBS ${LIBS} unit_test)

#compilation error tests
if(${COMPILATION_ERROR_TESTS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_COMPILATION_ERROR_TEST_")

    foreach(test_src ${test_srcs})
        get_filename_component(test_name ${test_src} NAME_WE)
        if(${test_src} MATCHES ".*compilation_error_test.*")
            message("adding test " : ${test_name})
            add_executable(${test_name} ${test_src})
            if(${test_src} MATCHES ".*iterative_rounding.*"
                    OR ${test_src} MATCHES ".*linear_programming.*")
                target_link_libraries (${test_name} ${TEST_LIBS} glpk)
            endif()

            add_dependencies(tests ${test_name})
        endif()
    endforeach()
#regular tests
else()
    foreach(test_src ${test_srcs})
        get_filename_component(test_name ${test_src} NAME_WE)
        message("adding test " : ${test_name})
        if(${test_name} MATCHES ".*test.*")
            if(${test_src} MATCHES ".*long.*")
                set(long_tests ${long_tests}  ${test_src})
            else()
                set(short_tests ${short_tests}  ${test_src})
            endif()
        else()
            add_executable(${test_name} ${test_src})
            if(${test_src} MATCHES ".*iterative_rounding.*"
                    OR ${test_src} MATCHES ".*linear_programming.*"
                    OR ${test_src} MATCHES ".*fractional_winner_determination_in_MUCA*")
                target_link_libraries (${test_name} ${TEST_LIBS} glpk)
            elseif(${test_src} MATCHES ".*lsh.*" )
                target_link_libraries (${test_name} ${TEST_LIBS} boost_serialization)
            else()
                target_link_libraries (${test_name} ${TEST_LIBS})
            endif()
        endif()
    endforeach()


    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBUILD_DIR=\\\"${CMAKE_BINARY_DIR}/\\\"")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROJECT_DIR=\\\"${PAAL}/\\\"")
    add_executable(all_small_tests ${short_tests})
    target_link_libraries(all_small_tests ${TEST_LIBS} glpk boost_serialization boost_iostreams)
    add_dependencies(all_small_tests bins)
    add_dependencies(tests all_small_tests)

    add_executable(all_long_tests ${long_tests})
    target_link_libraries(all_long_tests ${TEST_LIBS} glpk)
    add_dependencies(tests all_long_tests)
endif()
