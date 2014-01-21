/**
 * @file shortest_superstring_long_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-20
 */

#include "utils/logger.hpp"
#include "utils/read_ss.hpp"
#include "utils/parse_file.hpp"
#include "utils/test_result_check.hpp"

#include "paal/greedy/shortest_superstring/shortest_superstring.hpp"
#include "paal/utils/floating.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace paal;

BOOST_AUTO_TEST_CASE(shortest_superstringLong) {
    std::string testDir = "test/data/SS/";
    parse(testDir + "capopt.txt",
          [&](const std::string & fname, std::istream & is_test_cases) {
        int opt;
        is_test_cases >> opt;
        LOGLN("TEST " << fname);
        LOGLN(std::setprecision(20) << "OPT " << opt);

        std::ifstream ifs(testDir + "/cases/" + fname + ".in");
        auto words = read_SS(ifs);

        for (auto word: words) {
            for (auto letter: word) assert(letter > 0);
        }
        std::string res = paal::greedy::shortestSuperstring(words);
        int s = res.size();
        // check feasibility
        if (s < 100) {
            for (auto word: words)
               BOOST_CHECK(res.find(word) != std::string::npos);
        }
        if (opt != 0) {
            LOGLN(res);
            check_result(s, opt, 3.5);
        }
    });
}
