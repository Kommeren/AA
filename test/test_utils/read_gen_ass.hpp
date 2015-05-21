//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file read_gen_ass.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-05-07
 */
#ifndef PAAL_READ_GEN_ASS_HPP
#define PAAL_READ_GEN_ASS_HPP

#include "paal/data_structures/metric/basic_metrics.hpp"
#include "paal/utils/irange.hpp"

#include <cassert>

namespace paal {

using M = data_structures::rectangle_array_metric<int>;

inline void read_gen_ass(std::istream & is, M & costs, M & times,
                 std::vector<int> & machines_bounds,
                 boost::integer_range<int> & machines,
                 boost::integer_range<int> & jobs) {
    assert(is.good());

    int machines_cnt, jobs_cnt;
    is >> machines_cnt >> jobs_cnt;
    machines = paal::irange(machines_cnt);
    jobs     = paal::irange(jobs_cnt);
    costs = M(jobs_cnt, machines_cnt);
    times = M(jobs_cnt, machines_cnt);
    machines_bounds.resize(machines_cnt);

    for (int m : machines) {
        for (int j : jobs) {
            is >> costs(j, m);
        }
    }

    for (int m : machines) {
        for (int j : jobs) {
            is >> times(j, m);
        }
    }

    for(int m : machines) {
        is >> machines_bounds[m];
    }
}
}
#endif // PAAL_READ_GEN_ASS_HPP
