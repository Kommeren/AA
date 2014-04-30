/**
 * @file zelikovsky_11_per_6_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */
#include <boost/test/unit_test.hpp>

//this include must be here! //hack for clang
#include "paal/steiner_tree/zelikovsky_11_per_6.hpp"

#include "utils/logger.hpp"
#include "utils/sample_graph.hpp"
#include "utils/euclidean_metric.hpp"

#include <iosfwd>

namespace std {
    std::ostream & operator<<(std::ostream & o, const std::pair<int, int> & p) {
        o << p.first << "," << p.second;
        return o;
    }
}


BOOST_AUTO_TEST_CASE(zelikowsky_test) {
    typedef  sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_steiner();
    typedef decltype(gm) Metric;
    typedef paal::data_structures::voronoi<Metric> voronoiT;
    typedef typename voronoiT::GeneratorsSet FSet;
    voronoiT voronoi(FSet{SGM::A, SGM::B, SGM::C, SGM::D},FSet{SGM::E}, gm);

    std::vector<int> steiner;

    paal::steiner_tree::steiner_tree_zelikovsky11per6approximation(gm, voronoi, std::back_inserter(steiner));
    BOOST_CHECK_EQUAL(steiner.size(), std::size_t(1));
    BOOST_CHECK_EQUAL(steiner.front(), SGM::E);
}

BOOST_AUTO_TEST_CASE(euclidean_metric_test) {
    paal::euclidean_metric em;
    using std::make_pair;
    typedef paal::data_structures::voronoi<paal::euclidean_metric> voronoiT;
    typedef typename voronoiT::GeneratorsSet FSet;
    voronoiT voronoi(FSet{{0,0}, {0,2}, {2, 0}, {2, 2}}, FSet{{1,1}}, em);
    std::vector<std::pair<int, int>> steiner;

    paal::steiner_tree::steiner_tree_zelikovsky11per6approximation(em, voronoi, std::back_inserter(steiner));
    BOOST_CHECK_EQUAL(steiner.size(), std::size_t(1));
    BOOST_CHECK_EQUAL(steiner.front(), make_pair(1,1));
}
