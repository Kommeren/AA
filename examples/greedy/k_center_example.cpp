/**
 * @file k_center_example.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-01-28
 */

#include <iostream>
#include <vector>
#include <boost/range/irange.hpp>
#include "paal/greedy/k_center/k_center.hpp"
#include "paal/data_structures/metric/basic_metrics.hpp"

int main(){
//! [K Center Example]
    // sample data
    const int parts=2;
    paal::data_structures::array_metric<int> m(3);
    m(0,1)=3;
    m(0,2)=4;
    m(1,2)=5;
    m(1,0)=3;
    m(2,0)=4;
    m(2,1)=5;
    auto vertices=boost::irange(0,3);
    std::vector<int> centers;

    //solution
    std::cout<<paal::greedy::kCenter(m,parts,vertices.begin(),vertices.end(),back_inserter(centers))<<std::endl;


//! [K Center Example]
}
