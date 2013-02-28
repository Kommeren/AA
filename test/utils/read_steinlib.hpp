/**
 * @file read_orlib_fc.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */
#ifndef READ_ORLIB_FC_HPP
#define READ_ORLIB_FC_HPP

#include "paal/data_structures/metric/graph_metrics.hpp"
#include "utils/logger.hpp"

namespace paal {

typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property < boost::edge_weight_t, int > > graph_t;
typedef paal::data_structures::GraphMetric<graph_t, int> GraphMT;

std::istream & goTo(std::istream & is, const std::string & s){
    static const int MAX_LINE_SIZE = 1024;
    char line[MAX_LINE_SIZE];
    memset(line, 0, MAX_LINE_SIZE);
    while(s != line) {
        is.getline(line, MAX_LINE_SIZE);
    }
    return is;
}

std::istream & goToSection(std::istream & is, const std::string & s){
    return goTo(is, "SECTION " + s);
}

int readInt(std::istream & is, const std::string & token){
    std::string s;
    int i;
    is >> s >> i;
    assert(s == token);
    return i;
}

GraphMT readSTEINLIB(std::istream & is, std::vector<int> & terminals, std::vector<int> & steinerPoints) {
    typedef std::pair<int, int> Edge;

    goToSection(is, "Graph");
    int N, E, T;
    N = readInt(is, "Nodes");
    ++N; // Nodes are nubered form 1 in STEINLIB
    E = readInt(is, "Edges");

    steinerPoints.resize(N);
    
    std::iota(steinerPoints.begin(), steinerPoints.end(), 0);
    std::vector<int> weights(E);
    std::vector<Edge> edges(E);
    
    for(int i : boost::irange(0,E)) {
        Edge e;
        std::string s;
        is >> s >> e.first >> e.second >> weights[i];
        edges[i] = e;
        assert(s == "E");
    }
    graph_t g(edges.begin(), edges.end(), &weights[0], N);
    goToSection(is, "Terminals");
    T = readInt(is, "Terminals");
    terminals.resize(T);
    for(int i : boost::irange(0,T)) {
        terminals[i] = readInt(is, "T");
    }
    
    return GraphMT(g);
}

}
#endif /* READ_ORLIB_FC_HPP */
