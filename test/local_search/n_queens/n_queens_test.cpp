/**
 * @file n_queens_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-01-04
 */

#include <boost/test/unit_test.hpp>

#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include "paal/local_search/n_queens/n_queens_local_search.hpp"

#include "paal/local_search/local_search.hpp"
#include "paal/data_structures/components/components_replace.hpp"
#include "utils/logger.hpp"


BOOST_AUTO_TEST_CASE(n_queens_test) {
    namespace ls = paal::local_search;
    typedef ls::NQueensSolutionAdapter<std::vector<int>> Adapter;
    for(int i : {4, 5, 6 ,7, 8, 9, 10, 100, 500}) {
        std::vector<int> queens(i);
        boost::iota(queens, 0);
        boost::random_shuffle(queens);

        LOGLN("n = "<< i << " start obj fun val = " << Adapter(queens).objFun());

        ls::NQueensLocalSearchComponents<> comps;
        ls::nQueensSolutionLocalSearchSimple(queens, comps);
        LOGLN("end obj fun val = " << Adapter(queens).objFun());
    }
}



