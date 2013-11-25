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
        for(int i=0;i<words.size();i++){
            for(int j=0;j<words[i].size();j++)
                assert(words[i][j]>0);
                
        }
        std::string res=paal::greedy::shortestSuperstring(words);
        double s = res.size();
        LOGLN(std::setprecision(20) <<  "GREEDY " << s);
        if(opt!=0){
        LOGLN(res);
        BOOST_CHECK(utils::Compare<double>(0.001).le(opt,s));
        LOGLN( std::setprecision(20) << "APPROXIMATION RATIO: " << s / opt);
        }     
    });
}
