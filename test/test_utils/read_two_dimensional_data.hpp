/**
 * @file read_two_dimensional_data.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-09-17
 */

#ifndef READ_SS_HPP
#define READ_SS_HPP

#include <vector>
#include <istream>

namespace paal {

template <typename Point = std::vector<double>>
auto read_two_dimensional_data(std::istream &ist) {
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
#endif /* READ_SS_HPP */
