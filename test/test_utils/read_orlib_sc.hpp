/**
 * @file read_orlib_sc.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-01
 */

#ifndef READ_ORLIB_SC_HPP
#define READ_ORLIB_SC_HPP

#include <boost/range/irange.hpp>

#include <vector>
#include <utility>
#include <istream>

namespace paal {

std::pair<std::vector<int>, std::vector<std::vector<int>>>
read_ORLIB_SC(std::istream &ist) {
    int number_of_elements, number_of_set, cov, setI_id;
    ist >> number_of_elements >> number_of_set;
    std::vector<std::vector<int>> sets(number_of_set);
    std::vector<int> costs(number_of_set);
    for (auto &cost:costs) {
        ist >> cost;
    }
    for (int i:boost::irange(0,number_of_elements)) {
        ist >> cov;
        for (int j = 0; j < cov; j++) {
            ist >> setI_id;
            sets[setI_id - 1].push_back(i);
        }
    }
    return std::make_pair(costs, sets);
}
}
#endif /* READ_ORLIB_FC_HPP */
