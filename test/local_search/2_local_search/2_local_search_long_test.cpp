//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include "test_utils/read_tsplib.hpp"
#include "test_utils/2_local_search_logger.hpp"
#include "test_utils/test_result_check.hpp"
#include "test_utils/get_test_dir.hpp"

#include "paal/local_search/2_local_search/2_local_search.hpp"
#include "paal/data_structures/cycle/simple_cycle.hpp"
#include "paal/data_structures/components/components_replace.hpp"
#include "paal/local_search/custom_components.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>
#include <iomanip>
#include <cmath>

using std::string;
using std::vector;
using namespace paal::local_search;
using namespace paal;

std::string path = get_test_dir("TSPLIB/symmetrical");

BOOST_AUTO_TEST_CASE(TSPLIB) {
    read_tsplib::TSPLIB_Directory dir(path);
    read_tsplib::TSPLIB_Matrix mtx;
    std::string fname;
    float opt;
    while (dir.get_graph(fname, opt)) {
        std::ifstream is(fname);
        read_tsplib::TSPLIB_Directory::Graph g(is);
        g.load(mtx);
        auto size = mtx.size();
        std::vector<int> v(size);
        std::iota(v.begin(), v.end(), 0);

        // create random solution
        std::random_shuffle(v.begin(), v.end());
        data_structures::simple_cycle<int> cycle(v.begin(), v.end());

        // creating local search
        auto lsc = get_default_two_local_components(mtx);

        // printing
        LOGLN("Graph:\t" << fname);
        LOGLN("Length before\t" << get_cycle_length(mtx, cycle));

        // setting logger
        auto logger = utils::make_two_ls_logger(mtx, 100);

        // search
        two_local_search(cycle, paal::local_search::first_improving_strategy{},
                         logger, utils::always_false(), lsc);
        check_result(float(get_cycle_length(mtx, cycle)), opt,
                     4 * sqrt(size));
    }
}

// currently this is too long !
// hope that in the future stronger algorithms'll handle these cases
//
//

/*using paal::local_search::search_components;

BOOST_AUTO_TEST_CASE(TSPLIB_long) {
    const string indexFName = "index.long";
    read_tsplib::TSPLIB_Directory dir(path + "/long/", indexFName);
    read_tsplib::TSPLIB_Matrix mtx;
    std::string fname;
    float opt;
    while(dir.get_graph(fname ,opt))  {
        std::ifstream is(fname);
        read_tsplib::TSPLIB_Directory::Graph g(is);
        g.load(mtx);
        auto size = mtx.size();
        std::vector<int> v(size);
        std::iota(v.begin(), v.end(), 0);

        //create random solution
        std::random_shuffle(v.begin(), v.end());
        data_structures::simple_cycle<int> cycle(v.begin(), v.end());
        int startLen = simple_algo::get_length(mtx, cycle);

        //creating local search
        auto lsc = get_default_two_local_components(mtx);
        typedef local_search::search_components_traits<puretype(lsc)>::GainT
GainT;
        typedef paal::local_search::gain_cut_small_improves<GainT, int> CIC;
        double epsilon = 0.001;
        CIC  cut(lsc.get<local_search::Gain>(), startLen, epsilon);
        auto cutLsc =
data_structures::replace<local_search::Gain>(std::move(cut), lsc);

        //setting logger
        auto logger = utils::make_two_ls_logger(mtx, 100);

        //printing
        LOGLN("Graph:\t" << fname);
        LOGLN("Length before\t" << simple_algo::get_length(mtx, cycle));

        //search
        for(int j = 0; j < 20; ++j) {
            epsilon /= 2;
            LOGLN("epsilon = " << epsilon);
            cutLsc.get<local_search::Gain>().set_epsilon(epsilon);
            two_local_search(cycle, logger, utils::always_false(), cutLsc);
        }

        LOGLN("Normal search at the end");
        two_local_search(cycle, logger, utils::always_false(), lsc);
    }
}*/
