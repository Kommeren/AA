/**
 * @file read_orlib_fc.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */
#ifndef READ_ORLIB_FC_HPP
#define READ_ORLIB_FC_HPP

#include "paal/data_structures/metric/basic_metrics.hpp"
#include "utils/logger.hpp"

namespace paal {

data_structures::ArrayMetric<double> readORLIB_FL(std::istream & ist, std::vector<double> & facCosts, std::vector<int> & facCap,
                       std::vector<int> & demands,
                       boost::integer_range<int> & fac,
                       boost::integer_range<int> & clients ) { 
    int N, F;
    ist >> F >> N;
    
    fac =     boost::irange(0, F);
    clients = boost::irange(F, N + F);
    
    data_structures::ArrayMetric<double> m(N + F);
    demands.resize(N);
    facCosts.resize(F);
    facCap.resize(F);
    for(int i : boost::irange(0,F)) {
        ist >> facCap[i] >> facCosts[i];
    }
    
    double l;
    for(int i : boost::irange(0,N)) {
        ist >> demands[i];    
        for(int j : boost::irange(0,F)) {
            ist >> l;
            m(i+F, j) = l;
            m(j, i+F) = l;
    //        LOG(i+F << " "<< j << " " << l);
        }
    }


    //LOG_COPY(facCosts.begin(), facCosts.end());
    return m;   
}

}
#endif /* READ_ORLIB_FC_HPP */
