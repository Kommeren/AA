/**
 * @file steiner_tree_example.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include "paal/steiner_tree/steiner_tree.hpp"
#include "utils/sample_graph.hpp"

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

    //steiner tree object
    paal::steiner_tree::SteinerTree<Metric, VoronoiT> st(gm, voronoi);

    //run algorithm
    auto steiner =  st.getResultSteinerVertices();

    //print result
    std::cout << "Steiner points:" << std::endl;
    std::copy(steiner.begin(), steiner.end(), std::ostream_iterator<int>(std::cout, "\n"));
//! [Steiner Tree Example]
    return 0;
}