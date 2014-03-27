/**
 * @file read_orlib_fc.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */
#ifndef READ_ORLIB_FC_HPP
#define READ_ORLIB_FC_HPP

#include <fstream>
#include "paal/data_structures/metric/graph_metrics.hpp"
#include "utils/logger.hpp"

namespace paal {

typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS,
          boost::property<boost::vertex_color_t, int>, boost::property < boost::edge_weight_t, int > > Graph;
typedef paal::data_structures::graph_metric<Graph, int> GraphMT;

struct steiner_tree_test {
    steiner_tree_test(std::string name, int opt, std::vector<int> term, std::vector<int> steiner, Graph g):
        test_name(name), optimal(opt), terminals(term), steiner_points(steiner), graph(g), metric(g) {}

    std::string test_name;
    int optimal;
    std::vector<int> terminals;
    std::vector<int> steiner_points;
    Graph graph;
    GraphMT metric;
};

inline std::istream & go_to(std::istream & is, const std::string & s){
    static const int MAX_LINE_SIZE = 1024;
    char line[MAX_LINE_SIZE];
    memset(line, 0, MAX_LINE_SIZE);
    while(s != line) {
        is.getline(line, MAX_LINE_SIZE);
    }
    return is;
}

inline std::istream & go_to_section(std::istream & is, const std::string & s){
    return go_to(is, "SECTION " + s);
}

inline int read_int(std::istream & is, const std::string & token){
    std::string s;
    int i;
    is >> s >> i;
    assert(s == token);
    return i;
}

inline Graph read_steinlib(std::istream & is, std::vector<int> & terminals, std::vector<int> & steiner_points) {
    typedef std::pair<int, int> Edge;

    go_to_section(is, "Graph");
    int N, E, T;
    N = read_int(is, "Nodes");
    ++N; // Nodes are nubered from 1 in STEINLIB
    E = read_int(is, "Edges");

    steiner_points.resize(N);

    std::iota(steiner_points.begin(), steiner_points.end(), 0);
    std::vector<int> weights(E);
    std::vector<Edge> edges(E);

    for(int i : boost::irange(0,E)) {
        Edge e;
        std::string s;
        is >> s >> e.first >> e.second >> weights[i];
        edges[i] = e;
        assert(s == "E");
    }
    Graph g(edges.begin(), edges.end(), &weights[0], N);
    go_to_section(is, "Terminals");
    T = read_int(is, "Terminals");
    terminals.resize(T);
    auto color = get(boost::vertex_color, g);
    for(int i : boost::irange(0,T)) {
        terminals[i] = read_int(is, "T");
        put(color, terminals[i], 1);
    }
    return g;
}

inline void read_line(std::istream & is, std::string & fname, int & OPT) {
    int dummy;
    std::string dummys;
    is >> fname >> dummy >> dummy >> dummy >> dummys >> OPT;
    fname += ".stp";
}

inline void read_steinlib_tests(std::vector<steiner_tree_test>& data) {
    std::string testDir = "test/data/STEINLIB/";
    std::ifstream is_test_cases(testDir + "/index");
    assert(is_test_cases.good());
    while (is_test_cases.good()) {
        std::string fname;
        int opt;
        read_line(is_test_cases, fname, opt);
        if (fname == ".stp")
            return;
        std::ifstream ifs(testDir + "/I080/" + fname);
        assert(ifs.good());
        assert(ifs.good());
        std::vector<int> terminals;
        std::vector<int> steiner_points;
        Graph graph(paal::read_steinlib(ifs, terminals, steiner_points));
        steiner_tree_test test(fname, opt, terminals, steiner_points, graph);
        data.push_back(test);
    }
}

}
#endif /* READ_ORLIB_FC_HPP */
