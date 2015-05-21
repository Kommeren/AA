//=======================================================================
// Copyright (c) 2014 Karol Wegrzycki
//
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

/**
 * @file mapped_file_test.cpp
 * @brief Testing mapped file structur
 * @author Karol Wegrzycki
 * @version 1.0
 * @date 2014-12-22
 */

#include "test_utils/get_test_dir.hpp"

#include "paal/data_structures/mapped_file.hpp"
#include "paal/utils/irange.hpp"

#include <boost/test/unit_test.hpp>

#include <cstdio>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>



BOOST_AUTO_TEST_SUITE(mapped_file_tests)


BOOST_AUTO_TEST_CASE(for_each_line_temporary_test) {
    std::string input_file = paal::system::create_tmp_file("mapped_temp_test1", "1\n2\n3\n4");

    auto add_one = [](std::string const & line) { return std::to_string(stoi(line)+1);};
    std::vector<std::string> expected_results {"2","3","4","5"};
    for (auto threads : paal::irange(1,4)) {
        auto r = paal::data_structures::for_each_line(add_one, input_file, threads);
        BOOST_CHECK_EQUAL_COLLECTIONS(expected_results.begin(), expected_results.end(), r.begin(), r.end());
    }
}

BOOST_AUTO_TEST_CASE(mapped_file_structure) {
    std::string data = "abcdef\nghijkl\nmnopqr\nstuwvx\nyz";
    paal::data_structures::mapped_file mmfile(data.c_str(), data.size(), 2, 6);
    auto line = mmfile.get_line();
    BOOST_CHECK("mnopqr" == line);
}

BOOST_AUTO_TEST_CASE(mapped_file_structure_lines_same_as_threads) {
    std::vector<std::string> lines = {"a","b","c","d","e","f","g"};
    std::string data = "";
    for (auto i : lines) {
        data += i + "\n";
    }
    for (auto i : paal::irange(lines.size())) {
        paal::data_structures::mapped_file mmfile(data.c_str(), data.size(), i, lines.size());
        auto line = mmfile.get_line();
        BOOST_CHECK(lines[i] == line);
    }
}

BOOST_AUTO_TEST_SUITE_END()
