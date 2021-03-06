/**
 * @file read_two_dimensional_data.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-09-17
 */
#ifndef PAAL_READ_TWO_DIMENSIONAL_DATA_HPP
#define PAAL_READ_TWO_DIMENSIONAL_DATA_HPP

#include <cassert>
#include <vector>
#include <istream>

namespace paal {

template <typename Point = std::vector<double>>
auto read_two_dimensional_data(std::istream &ist) {
    assert(ist.good());

    int n;
    bool special = false;
    ist >> n;
    if (n == 0) {
        special = true;
        ist >> n;
    }
    double x, y;
    int z;
    std::vector<Point> points;
    while (n--) {
        ist >> x >> y;
        points.push_back({x, y});
        if (special) {
            ist >> z;
        }
    }
    return points;
}

} //!paal

#endif /* PAAL_READ_TWO_DIMENSIONAL_DATA_HPP */
