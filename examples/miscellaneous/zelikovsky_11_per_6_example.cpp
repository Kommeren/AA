/**
 * @file zelikovsky_11_per_6_example.cpp
 * @brief This is example for zelikovsky 11/6 approximation for steiner tree problem.
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */
#include <iostream>
#include "paal/steiner_tree/zelikovsky_11_per_6.hpp"
#include "test/utils/sample_graph.hpp"

int main() {
//! [Steiner Tree Example]
    //sample metric
    typedef  SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSteiner();
    typedef decltype(gm) Metric;
    
    //sample voronoi
    typedef paal::data_structures::Voronoi<Metric> VoronoiT;
    typedef paal::data_structures::VoronoiTraits<VoronoiT> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    VoronoiT voronoi(GSet{SGM::A, SGM::B, SGM::C, SGM::D}, VSet{SGM::E}, gm);

    //run algorithm
    std::vector<int> steinerPoints; 
    paal::steiner_tree::steinerTreeZelikovsky11per6approximation(gm, voronoi, std::back_inserter(steinerPoints));

    //print result
    std::cout << "Steiner points:" << std::endl;
    std::copy(steinerPoints.begin(), steinerPoints.end(), std::ostream_iterator<int>(std::cout, "\n"));
//! [Steiner Tree Example]
    return 0;
}
