//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file read_dist.hpp
 * @brief
 * @author Jakub Oćwieja
 * @version 1.0
 * @date 2014-05-19
 */

#ifndef PAAL_READ_DIST_HPP
#define PAAL_READ_DIST_HPP

#include "paal/utils/irange.hpp"

#include <boost/graph/adjacency_list.hpp>

#include <cassert>

namespace paal {
namespace {

typedef  boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::undirectedS,
        boost::no_property,
        boost::property < boost::edge_weight_t, int> >
    Graph;

Graph read_dist(std::istream& ist) {
    assert(ist.good());

    int n, m; ist >> n >> m;
    std::vector< std::pair<int, int> > edges(m);
    std::vector< int > edgeWeights(m);

    for (auto i: paal::irange(m)) {
        ist >> edges[i].first >> edges[i].second >> edgeWeights[i];
    }

    return Graph(edges.begin(), edges.end(), edgeWeights.begin(), n);
}

}
} // paal

#endif // PAAL_READ_DIST_HPP
