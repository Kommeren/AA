/**
 * @file metric_to_bgl_mst_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#define BOOST_TEST_MODULE metric_to_bgl_mst_test

#include <boost/test/unit_test.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>

#include <vector>

#include "local_search/steiner_tree/steiner_tree.hpp"
#include "utils/sample_graph.hpp"

BOOST_AUTO_TEST_CASE(metric_to_bgl_mst_test) {
    typedef  SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetric();
    typedef decltype(gm) Metric;
    
    typedef paal::data_structures::Voronoi<int, Metric> VoronoiT;
    typedef typename VoronoiT::GeneratorsSet FSet;
    VoronoiT voronoi(FSet{SGM::A, SGM::B},
            FSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);

    paal::local_search::steiner_tree::SteinerTree<Metric, VoronoiT> st(gm, voronoi);
    st.getSteinerTree(); 
   //BOOST_CHECK_EQUAL(s, 6);
}
