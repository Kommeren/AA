/**
 * @file zelikovsky_11_per_6_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */



#define BOOST_TEST_MODULE zeliokvsky_test

#include <boost/test/unit_test.hpp>

#include <iosfwd>

#include "utils/logger.hpp"

#include "paal/steiner_tree/zelikovsky_11_per_6.hpp"
#include "utils/sample_graph.hpp"
#include "utils/euclidean_metric.hpp"

template <typename T>
std::ostream & operator<<(std::ostream & o, const std::pair<T, T> & p) {
    o << p.first << "," << p.second;
    return o;
}

std::ostream & operator<<(std::ostream & o, const std::pair<int, int> & p) {
    o << p.first << "," << p.second;
    return o;
}

BOOST_AUTO_TEST_CASE(zelikowsky_test) {
    typedef  SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSteiner();
    typedef decltype(gm) Metric;
    typedef paal::data_structures::Voronoi<Metric> VoronoiT;
    typedef typename VoronoiT::GeneratorsSet FSet;
    VoronoiT voronoi(FSet{SGM::A, SGM::B, SGM::C, SGM::D},FSet{SGM::E}, gm);

    auto steiner = paal::steiner_tree::getSteinerVertices(gm, voronoi); 
    BOOST_CHECK_EQUAL(steiner.size(), size_t(1));
    BOOST_CHECK_EQUAL(steiner.front(), SGM::E);
}

BOOST_AUTO_TEST_CASE(euclidean_metric_test) {
    paal::EuclideanMetric em;
    using std::make_pair;
    typedef paal::data_structures::Voronoi<paal::EuclideanMetric> VoronoiT;
    typedef typename VoronoiT::GeneratorsSet FSet;
    VoronoiT voronoi(FSet{make_pair(0,0), make_pair(0,2), make_pair(2, 0), make_pair(2, 2)}, FSet{make_pair(1,1)}, em);

    auto steiner = paal::steiner_tree::getSteinerVertices(em, voronoi); 
    BOOST_CHECK_EQUAL(steiner.size(), size_t(1));
//    BOOST_CHECK_EQUAL(steiner.front(), make_pair(1,1));
}
