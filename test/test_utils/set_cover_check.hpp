/**
 * @file set_cover_check.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-10-09
 */
#ifndef PAAL_SET_COVER_CHECK_HPP
#define PAAL_SET_COVER_CHECK_HPP

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/range/algorithm/max_element.hpp>
#include <boost/test/unit_test.hpp>

#include <vector>

template <typename Sets, typename SetsToElements, typename Result>
double set_cover_result_check(Sets sets, SetsToElements sets_to_elements,
                              Result result) {
    int max_item = 0;
    size_t max_size = 0;
    for (auto i : sets) {
        max_item = std::max(max_item, *boost::max_element(sets_to_elements(i)));
        max_size = std::max(sets_to_elements(i).size(), max_size);
    }
    std::vector<bool> is_covered(max_item + 1);
    for (auto i : result) {
        for (auto j : sets_to_elements(i)) {
            is_covered[j] = true;
        }
    }
    BOOST_CHECK(boost::algorithm::all_of_equal(is_covered, true));
    double approximation_ratio = 0.;
    for (size_t x = 1; x <= max_size; x++) {
        approximation_ratio += 1. / double(x);
    }
    return approximation_ratio;
}

#endif /* PAAL_SET_COVER_CHECK_HPP */
