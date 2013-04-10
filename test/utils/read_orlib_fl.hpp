/**
 * @file read_orlib_fc.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */
#ifndef READ_ORLIB_FC_HPP
#define READ_ORLIB_FC_HPP

#include <type_traits>

#include "paal/data_structures/metric/basic_metrics.hpp"
#include "utils/logger.hpp"

namespace paal {


long long cast(double d) {
    static const double MULTIPL = 1000000;
    return (long long)(d * MULTIPL) + 1;
}

namespace cap {
    class uncapacitated;
    class capacitated;
}


template <typename IsCapacitated, typename IStream>
typename std::enable_if<std::is_same<IsCapacitated, cap::uncapacitated>::value, int>::type  
readDemand(IStream & i){
    int a;
    i >> a;
    return 1;
}

template <typename IsCapacitated, typename IStream>
typename std::enable_if<std::is_same<IsCapacitated, cap::capacitated>::value, int>::type
readDemand(IStream & ist){
    int a;
    ist >> a;
    return a;
}


template <typename IsCapacitated>
data_structures::ArrayMetric<long long> readORLIB_FL(std::istream & ist, std::vector<long long> & facCosts, std::vector<int> & facCap,
                       std::vector<int> & demands,
                       boost::integer_range<int> & fac,
                       boost::integer_range<int> & clients ) {
    int N, F;
    ist >> F >> N;
    
    fac =     boost::irange(0, F);
    clients = boost::irange(F, N + F);
    
    data_structures::ArrayMetric<long long> m(N + F);
    demands.resize(N);
    facCosts.resize(F);
    facCap.resize(F);
    
    double l;
    for(int i : boost::irange(0,F)) {
        ist >> facCap[i] >> l;
        facCosts[i] = cast(l);
    }
    
    for(int i : boost::irange(0,N)) {

        demands[i] = readDemand<IsCapacitated>(ist);    
        for(int j : boost::irange(0,F)) {
            ist >> l;
            l /= double(demands[i]);
            m(i+F, j) = cast(l);
            m(j, i+F) = cast(l);
    //        LOG(i+F << " "<< j << " " << l);
        }
    }


    //LOG_COPY(facCosts.begin(), facCosts.end());
    return m;   
}

}
#endif /* READ_ORLIB_FC_HPP */
