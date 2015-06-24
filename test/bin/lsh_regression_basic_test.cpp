//=======================================================================
// Copyright (c) 2014 Karol Wegrzycki
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file lsh_regression_basic_test.cpp
 * @brief lsh_regression binnary basic functionality
 * @author Karol Wegrzycki
 * @version 1.0
 * @date 2014-11-19
 */
#include "test_utils/logger.hpp"
#include "test_utils/get_test_dir.hpp"
#include "test_utils/system.hpp"

#include <boost/range/algorithm/equal.hpp>
#include <boost/range/istream_range.hpp>
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <string>


namespace {
std::string lsh_bin = paal::system::get_build_dir("/bin/lsh-regression");
double EPSILON = 10e-9;

void call(std::string const &command) {
    BOOST_CHECK_MESSAGE(paal::system::exec(command) == 0, "Command failed: " << command);
}

void call_fail(std::string const &command) {
    BOOST_CHECK_MESSAGE(paal::system::exec(command) != 0, "Command that should fail succeeded: " << command);
}

using paal::system::get_temp_file_path;

void test_files_are_equal(std::string const &file_path_1, std::string const &file_path_2) {
    std::ifstream file_1_stream{file_path_1}, file_2_stream{file_path_2};
    auto file_1_contents = boost::istream_range<double>(file_1_stream);
    auto file_2_contents = boost::istream_range<double>(file_2_stream);

    auto check_close_doubles_always_true = [](double lhs, double rhs) {
        BOOST_CHECK_CLOSE(lhs, rhs, EPSILON);
        return true;
    };
    // TODO use BOOST_CHECK_CLOSE_RANGES when appears
    BOOST_CHECK_MESSAGE(boost::equal(file_1_contents, file_2_contents, check_close_doubles_always_true),
                        "Files " << file_path_1 << " and " << file_path_2 << " are different.");
}

} //! anonymous

using paal::system::create_tmp_file;

BOOST_AUTO_TEST_SUITE(lsh_regression_bin_tests)

BOOST_AUTO_TEST_CASE(lsh_bin_simple) {
    std::string training = create_tmp_file("training_1", "1 0:1\n0 1:1");
    std::string test = create_tmp_file("test_1", "1 0:1");
    std::string expt = create_tmp_file("expect_1", "1");
    std::string result = get_temp_file_path("simple_test_1.svm");

    call(lsh_bin + " -d " + training + " -t " + test + " -o " + result + " --dimensions=1");
    test_files_are_equal(result, expt);
}

BOOST_AUTO_TEST_CASE(lsh_bin_test_formats) {
    std::string training = create_tmp_file("training_f", "  0   1:1e-6 2:1.5 4:1. 7:.5\n1\n0");
    std::string test = create_tmp_file("test_f", "0   1:1e-6 2:1.5 4:1. 7:.5\n 1 8:1");
    std::string result = get_temp_file_path("result_test_f.svm");

    call(lsh_bin + " -d " + training + " -t " + test + " -o " + result + " --dimensions=9");
}

BOOST_AUTO_TEST_CASE(lsh_bin_test_metrics) {
    std::string training = create_tmp_file("training_l2", "0 0:1\n 1 0:10000\n 0 1:5");
    std::string test = create_tmp_file("test_l2", "0 0:3");
    std::string expect = create_tmp_file("expect_l2", "0");
    std::string result = get_temp_file_path("result_test_l2.svm");

    call(lsh_bin + " -d " + training + " -t " + test + " -o " + result + " -m l2" + " --dimensions=2");
    test_files_are_equal(result, expect);
    call(lsh_bin + " -d " + training + " -t " + test + " -o " + result + " -m l1" + " --dimensions=2");
    test_files_are_equal(result, expect);
}

BOOST_AUTO_TEST_CASE(lsh_bin_w_option) {
    std::string training = create_tmp_file("training_w", "0 0:1\n 1 0:10000");
    std::string test = create_tmp_file("test_w", "0 0:2");

    std::string result = get_temp_file_path("result_w");

    call(lsh_bin + " -d " + training + " -t " + test + " -o " + result + " -m l2 -w 1" + " --dimensions=1");
    std::string expect05 = create_tmp_file("expect_w05", "0.5");
    test_files_are_equal(result, expect05);

    call(lsh_bin + " -d " + training + " -t " + test + " -o " + result + " -m l2 -w 10" + " --dimensions=1");
    std::string expect0 = create_tmp_file("expect_w0", "0");
    test_files_are_equal(result, expect0);
}

BOOST_AUTO_TEST_CASE(lsh_bin_bad_usage) {
    std::string sample_data_file = create_tmp_file("sample_data_file", "1 0:1");
    std::string sample_test_file = create_tmp_file("sample_test_file", "1 0:2");
    std::string empty_file =  create_tmp_file("sample_file", "");

    call_fail(lsh_bin + " -d " + sample_data_file + " -t " + sample_test_file);
    call_fail(lsh_bin + " -d " + sample_data_file + " -t " + empty_file + " --dimensions=1");
    call_fail(lsh_bin + " " + sample_data_file + " -t " + sample_test_file + " --dimensions=1");
    call_fail(lsh_bin + " -o " + empty_file + " --dimensions=1");
    call_fail(lsh_bin);

    call(lsh_bin + " --help");
}

BOOST_AUTO_TEST_CASE(lsh_bin_serialization) {
    std::string example = create_tmp_file("example", "1 0:1");
    std::string test = create_tmp_file("test", "0 0:3");
    std::string model(get_temp_file_path("model.1")),
                model2(get_temp_file_path("model.2")),
                res(get_temp_file_path("res.txt"));

    call(lsh_bin + " --model_out " + model + " -d " + example + " --dimensions=1");
    call(lsh_bin + " --model_in " + model + " -o " + res + " -t " + test);
    std::string expect = create_tmp_file("expect", "1");
    test_files_are_equal(res, expect);

    std::string example2 = create_tmp_file("example2", "1 0:1\n0 0:1\n0 0:1");

    call(lsh_bin + " --model_in " + model + " --model_out " + model2 + " -d " + example2);
    call(lsh_bin + " --model_in " + model2 + " -o " + res + " -t " + test);
    std::string expect2 = create_tmp_file("expect2", "0.5");
    test_files_are_equal(res, expect2);
}

BOOST_AUTO_TEST_SUITE_END()
