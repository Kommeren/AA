/**
 * @file read_gen_ass.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-05-07
 */
#ifndef READ_GEN_ASS_HPP
#define READ_GEN_ASS_HPP

#include "paal/data_structures/metric/basic_metrics.hpp"

#include <boost/range/irange.hpp>

namespace paal {

using M = data_structures::rectangle_array_metric<int>;

void read_gen_ass(std::istream & is, M & costs, M & times,
                 std::vector<int> & machines_bounds,
                 boost::integer_range<int> & machines,
                 boost::integer_range<int> & jobs) {
    int machines_cnt, jobs_cnt;
    is >> machines_cnt >> jobs_cnt;
    machines = boost::irange(0, machines_cnt);
    jobs    = boost::irange(0, jobs_cnt);
    costs = M(jobs_cnt, machines_cnt);
    times = M(jobs_cnt, machines_cnt);
    machines_bounds.resize(machines_cnt);

    for(int m : machines) {
        for(int j : jobs) {
            is >> costs(j, m);
        }
    }

    for(int m : machines) {
        for(int j : jobs) {
            is >> times(j, m);
        }
    }

    for(int m : machines) {
        is >> machines_bounds[m];
    }
}

}
#endif /* READ_GEN_ASS_HPP */
