//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file read_orlib_fc.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */
#ifndef PAAL_READ_ORLIB_FL_HPP
#define PAAL_READ_ORLIB_FL_HPP

#include "test_utils/logger.hpp"

#include "paal/data_structures/metric/basic_metrics.hpp"
#include "paal/utils/irange.hpp"

#include <type_traits>

namespace paal {

static const long long MULTIPL = 1000000;

inline long long cast(double d) { return (long long)(d * MULTIPL) + 1; }

namespace cap {
class uncapacitated;
class capacitated;
}

template <typename IsCapacitated, typename IStream>
typename std::enable_if<std::is_same<IsCapacitated, cap::uncapacitated>::value,
                        int>::type
read_demand(IStream &i) {
    int a;
    i >> a;
    return 1;
}

template <typename IsCapacitated, typename IStream>
typename std::enable_if<std::is_same<IsCapacitated, cap::capacitated>::value,
                        int>::type
read_demand(IStream &ist) {
    int a;
    ist >> a;
    return a;
}

template <typename IsCapacitated>
data_structures::array_metric<long long>
read_orlib_FL(std::istream &ist, std::vector<long long> &facCosts,
              std::vector<int> &facCap, std::vector<int> &demands,
              boost::integer_range<int> &fac,
              boost::integer_range<int> &clients) {
    int N, F;
    ist >> F >> N;

    fac = paal::irange(F);
    clients = paal::irange(F, N + F);

    data_structures::array_metric<long long> m(N + F);
    demands.resize(N);
    facCosts.resize(F);
    facCap.resize(F);

    double l;
    for (int i : fac) {
        ist >> facCap[i] >> l;
        facCosts[i] = cast(l);
    }

    for (int i : paal::irange(N)) {

        demands[i] = read_demand<IsCapacitated>(ist);
        for (int j : fac) {
            ist >> l;
            l /= double(demands[i]);
            m(i + F, j) = cast(l);
            m(j, i + F) = cast(l);
            //        LOGLN(i+F << " "<< j << " " << l);
        }
    }

    // LOG_COPY(facCosts.begin(), facCosts.end());
    return m;
}
}
#endif // PAAL_READ_ORLIB_FL_HPP
