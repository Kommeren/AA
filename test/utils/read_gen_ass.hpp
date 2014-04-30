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

typedef data_structures::rectangle_array_metric<int> M;

void read_gen_ASS(std::istream & is, M & costs, M & times,
                 std::vector<int> & machinesBounds,
                 boost::integer_range<int> & machines,
                 boost::integer_range<int> & jobs) {
    int machinesCnt, jobsCnt;
    is >> machinesCnt >> jobsCnt;
    machines = boost::irange(0, machinesCnt);
    jobs    = boost::irange(0, jobsCnt);
    costs = M(jobsCnt, machinesCnt);
    times = M(jobsCnt, machinesCnt);
    machinesBounds.resize(machinesCnt);

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
        is >> machinesBounds[m];
    }
}

}
#endif /* READ_GEN_ASS_HPP */
