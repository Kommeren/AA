//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file read_steinlib.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */
#ifndef PAAL_READ_STEINLIB_HPP
#define PAAL_READ_STEINLIB_HPP

#include "test_utils/logger.hpp"

#include "paal/data_structures/metric/graph_metrics.hpp"
#include "paal/utils/irange.hpp"

#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/range/algorithm/find.hpp>

#include <fstream>

namespace paal {

using Graph =
    boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                          boost::property<boost::vertex_color_t, int>,
                          boost::property<boost::edge_weight_t, int>>;
using GraphMT = paal::data_structures::graph_metric<Graph, int>;

struct steiner_tree_test {
    steiner_tree_test(std::string name, int opt, std::vector<int> term,
                      std::vector<int> steiner, Graph g)
        : test_name(name), optimal(opt), terminals(term),
          steiner_points(steiner), graph(g) {}

    std::string test_name;
    int optimal;
    std::vector<int> terminals;
    std::vector<int> steiner_points;
    Graph graph;
};

struct steiner_tree_test_with_metric : public steiner_tree_test {
    steiner_tree_test_with_metric(std::string name, int opt,
                                  std::vector<int> term,
                                  std::vector<int> steiner, Graph g)
        : steiner_tree_test(name, opt, term, steiner, g), metric(g) {}

    GraphMT metric;
};

inline std::istream &go_to(std::istream &is, const std::string &s) {
    static const int MAX_LINE_SIZE = 1024;
    char line[MAX_LINE_SIZE];
    memset(line, 0, MAX_LINE_SIZE);
    while (s != line) {
        is.getline(line, MAX_LINE_SIZE);
    }
    return is;
}

inline std::istream &go_to_section(std::istream &is, const std::string &s) {
    return go_to(is, "SECTION " + s);
}

inline int read_int(std::istream &is, const std::string &token) {
    std::string s;
    int i;
    is >> s >> i;
    assert(s == token);
    return i;
}

inline Graph read_steinlib(std::istream &is, std::vector<int> &terminals,
                           std::vector<int> &steiner_points) {
    using Edge = std::pair<int, int>;

    go_to_section(is, "Graph");
    int N, E, T;
    N = read_int(is, "Nodes");
    E = read_int(is, "Edges");

    steiner_points.resize(N);

    // we actually add one additional steiner point not connected to any other
    // point
    boost::iota(steiner_points, 0);
    std::vector<int> weights(E);
    std::vector<Edge> edges(E);

    for (int i : paal::irange(E)) {
        Edge e;
        std::string s;
        is >> s >> e.first >> e.second >> weights[i];
        --e.first;
        --e.second;
        edges[i] = e;
        assert(s == "E");
    }
    Graph g(edges.begin(), edges.end(), &weights[0], N);
    go_to_section(is, "Terminals");
    T = read_int(is, "Terminals");
    terminals.resize(T);
    auto color = get(boost::vertex_color, g);
    for (int i : paal::irange(T)) {
        terminals[i] = read_int(is, "T") - 1;
        steiner_points.erase(boost::find(steiner_points, terminals[i]));
        put(color, terminals[i], 1);
    }
    return g;
}

inline void read_line(std::istream &is, std::string &fname, int &OPT) {
    int dummy;
    std::string dummys;
    is >> fname >> dummy >> dummy >> dummy >> dummys >> OPT;
    fname += ".stp";
}

inline void
read_steinlib_tests(std::vector<steiner_tree_test_with_metric> &data) {
    std::string test_dir = PROJECT_DIR"test/data/STEINLIB/";
    std::ifstream is_test_cases(test_dir + "/index");
    assert(is_test_cases.good());
    while (is_test_cases.good()) {
        std::string fname;
        int opt;
        read_line(is_test_cases, fname, opt);
        if (fname == ".stp") return;
        std::ifstream ifs(test_dir + "/I080/" + fname);
        assert(ifs.good());

        std::vector<int> terminals;
        std::vector<int> steiner_points;

        Graph graph(paal::read_steinlib(ifs, terminals, steiner_points));
        LOGLN("Terminals: ");
        LOG_COPY_RANGE_DEL(terminals, " ");
        LOGLN("");
        LOGLN("Steiner points: ");
        LOG_COPY_RANGE_DEL(steiner_points, " ");
        LOGLN("");
        LOGLN("");
        steiner_tree_test_with_metric test(fname, opt, terminals,
                                           steiner_points, graph);
        data.push_back(test);
    }
}
}
#endif // PAAL_READ_STEINLIB_HPP
