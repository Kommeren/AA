/**
 * @file shortest_superstring_long_test.cpp
 * @brief 
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-20
 */

#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/greedy/shortest_superstring/shortest_superstring.hpp"
#include "paal/utils/floating.hpp"
#include "utils/logger.hpp"
#include "utils/read_ss.hpp"
#include "utils/parse_file.hpp"

using namespace paal::greedy::shortest_superstring;
using namespace paal;


BOOST_AUTO_TEST_CASE(ShortestSuperstringLong) {
    std::string testDir = "test/data/SS/";
    parse(testDir + "capopt.txt", [&](const std::string  & fname, std::istream & is_test_cases) {
        double opt;
        is_test_cases >> opt;
        LOGLN("TEST " << fname);
        LOGLN(std::setprecision(20) <<  "OPT " << opt);

        std::ifstream ifs(testDir + "/cases/" + fname+".in");
        auto words= read_SS(ifs);
        std::string res=shortestSuperstring(words);
        double s = res.size();

        LOGLN(res);
        LOGLN(std::setprecision(20) <<  "GREEDY " << s);
        BOOST_CHECK(utils::Compare<double>(0.001).le(opt,s));
        LOGLN( std::setprecision(20) << "APPROXIMATION RATIO: " << s / opt);
    });
}
