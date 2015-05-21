/**
 * @file set_cover_long_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-02-25
 */

#include "test_utils/logger.hpp"
#include "test_utils/read_orlib_sc.hpp"
#include "test_utils/test_result_check.hpp"
#include "test_utils/set_cover_check.hpp"
#include "test_utils/get_test_dir.hpp"
#include "test_utils/system.hpp"

#include "paal/greedy/set_cover/set_cover.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/parse_file.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/algorithm/max_element.hpp>
#include <boost/algorithm/cxx11/all_of.hpp>

#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

BOOST_AUTO_TEST_CASE(set_cover_long) {
    const std::string test_dir = paal::system::get_test_data_dir("SC_ORLIB");
    using paal::system::build_path;

    paal::parse(build_path(test_dir, "cases.txt"),
                [&](const std::string &fname, std::istream &is_test_cases) {
        LOGLN("TEST " << fname);
        std::ifstream ifs(build_path(test_dir, "/cases/" + fname));
        assert(ifs.good());

        int sample_result;
        is_test_cases >> sample_result;
        auto sc = paal::read_ORLIB_SC(ifs);
        auto &costs_of_sets = sc.first;
        auto &elements_of_sets = sc.second;
        auto sets = boost::irange(size_t(0), costs_of_sets.size());
        auto set_to_elements =
            paal::utils::make_array_to_functor(elements_of_sets);
        std::vector<int> result;
        auto element_index = paal::utils::identity_functor{};
        auto cost = paal::greedy::set_cover(
            sets, paal::utils::make_array_to_functor(costs_of_sets),
            set_to_elements, std::back_inserter(result), element_index);
        double approximation_ratio = set_cover_result_check(sets,set_to_elements,result);
        check_result_compare_to_bound(cost, sample_result, approximation_ratio);
    });
}
