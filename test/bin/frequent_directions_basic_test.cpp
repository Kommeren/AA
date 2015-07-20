//=======================================================================
// Copyright (c) 2015
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file frequent_directions_basic_test.cpp
 * @brief frequent_directions binnary basic functionality
 * @author Tomasz Strozak
 * @version 1.0
 * @date 2015-07-26
 */
#include "paal/utils/irange.hpp"
#include "paal/utils/read_rows.hpp"
#include "test_utils/logger.hpp"
#include "test_utils/get_test_dir.hpp"
#include "test_utils/system.hpp"

#include <boost/range/algorithm/equal.hpp>
#include <boost/test/unit_test.hpp>

#include <cassert>
#include <fstream>
#include <limits>
#include <string>

using coordinate_t = double;
using paal::system::create_tmp_file;

namespace {
std::string fd_bin = paal::system::get_build_dir("/bin/frequent-directions");

void call(std::string const &command) {
    BOOST_CHECK_MESSAGE(paal::system::exec(command) == 0, "Command failed: " << command);
}

void call_fail(std::string const &command) {
    BOOST_CHECK_MESSAGE(paal::system::exec(command) != 0, "Command that should fail succeeded: " << command);
}

auto read_sketch_check_size(std::string const &output, std::size_t size1, std::size_t size2) {
    std::ifstream ifs(output);

    std::vector<std::vector<coordinate_t>> row_buffer;
    paal::read_rows_first_row_size<coordinate_t>(ifs, row_buffer, std::numeric_limits<std::size_t>::max());

    BOOST_CHECK_EQUAL(boost::size(row_buffer), size1);
    assert(!row_buffer.empty());
    BOOST_CHECK_EQUAL(boost::size(row_buffer.front()), size2);
    return row_buffer;
}

using paal::system::get_temp_file_path;

} //! anonymous

BOOST_AUTO_TEST_SUITE(frequent_directions_bin_tests)

BOOST_AUTO_TEST_CASE(help) {
    call(fd_bin + " --help");
}

BOOST_AUTO_TEST_CASE(simple) {
    std::string input = create_tmp_file("input_s", "0 1 2\n2 3 4");
    std::string output = get_temp_file_path("output_s");

    call(fd_bin + " -i " + input + " -o " + output + " -r 4");

    read_sketch_check_size(output, 2, 3);
}

BOOST_AUTO_TEST_CASE(standard_input) {
    std::string output = get_temp_file_path("output_si");

    call("echo \"0 1 2\" | " + fd_bin + " -o " + output + " -r 4");

    read_sketch_check_size(output, 2, 3);
}

BOOST_AUTO_TEST_CASE(standard_output) {
    std::string input = create_tmp_file("input_so", "0 1 2\n2 3 4");
    std::string output = get_temp_file_path("output_so");

    std::string command = fd_bin + " -i " + input + " -r 4";
    BOOST_CHECK_MESSAGE(paal::system::exec(command, false, output) == 0, "Command failed: " << command);

    read_sketch_check_size(output, 2, 3);
}

BOOST_AUTO_TEST_CASE(without_sketch_rows) {
    std::string input = create_tmp_file("input_wsr", "0 1 2\n2 3 4");
    std::string output = get_temp_file_path("output_wsr");

    call_fail(fd_bin + " -i " + input + " -o " + output);
}

BOOST_AUTO_TEST_CASE(compress_size) {
    std::string input = create_tmp_file("input_cs", "0 1 2\n2 3 4");
    std::string output = get_temp_file_path("output_cs");

    call(fd_bin + " -i " + input + " -o " + output + " -r 4 -s 1");

    read_sketch_check_size(output, 1, 3);
}

BOOST_AUTO_TEST_CASE(final_compress) {
    std::string input = create_tmp_file("input_fc", "0 1 2\n2 3 4");
    std::string output = get_temp_file_path("output_fc");

    call(fd_bin + " -i " + input + " -o " + output + " -r 4 --final_compress 0");

    auto row_buffer = read_sketch_check_size(output, 2, 3);
    BOOST_CHECK(boost::equal(row_buffer, std::vector<std::vector<coordinate_t>>{{0, 1, 2}, {2, 3, 4}}));
}

BOOST_AUTO_TEST_CASE(row_buffer_size) {
    std::string input = create_tmp_file("input_rbs", "0 1 2\n2 3 4");
    std::string output = get_temp_file_path("output_rbs");

    call(fd_bin + " -i " + input + " -o " + output + " -r 4 --row_buffer_size 10");

    read_sketch_check_size(output, 2, 3);
}

BOOST_AUTO_TEST_CASE(row_buffer_empty_size) {
    std::string input = create_tmp_file("input_rbes", "0 1 2\n2 3 4");
    std::string output = get_temp_file_path("output_rbes");

    call_fail(fd_bin + " -i " + input + " -o " + output + " -r 4 --row_buffer_size 0");
}

BOOST_AUTO_TEST_CASE(model) {
    std::string input = create_tmp_file("input_m", "0 1 2\n2 3 4");
    std::string output = get_temp_file_path("output_m");
    std::string model = get_temp_file_path("model_m");

    call(fd_bin + " --model_out " + model + " -i " + input + " -o " + output + " -r 4");
    call(fd_bin + " --model_in " + model + " -i " + input + " -o " + output);

    read_sketch_check_size(output, 2, 3);
}

BOOST_AUTO_TEST_CASE(multi_read) {
    std::ostringstream data;
    for (auto row_id : paal::irange(500)) {
        data << row_id << " " << (row_id+1) << " " << (row_id+2) << "\n";
    }
    std::string input = create_tmp_file("input_mr", data.str());
    std::string output = get_temp_file_path("output_mr");

    call(fd_bin + " -i " + input + " -o " + output + " -r 4 --row_buffer_size 100");

    read_sketch_check_size(output, 2, 3);
}

BOOST_AUTO_TEST_SUITE_END()
