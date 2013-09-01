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

typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS,
            boost::no_property, boost::property < boost::edge_weight_t, int > > Graph;
typedef paal::data_structures::GraphMetric<Graph, int> GraphMT;

struct SteinerTreeTest {
    SteinerTreeTest(std::string name, int opt, std::vector<int> term, std::vector<int> steiner, Graph g):
        testName(name), optimal(opt), terminals(term), steinerPoints(steiner), graph(g), metric(g) {}

    std::string testName;
    int optimal;
    std::vector<int> terminals;
    std::vector<int> steinerPoints;
    Graph graph;
    GraphMT metric;
};

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

Graph readSTEINLIB(std::istream & is, std::vector<int> & terminals, std::vector<int> & steinerPoints) {
    typedef std::pair<int, int> Edge;

    goToSection(is, "Graph");
    int N, E, T;
    N = readInt(is, "Nodes");
    ++N; // Nodes are nubered from 1 in STEINLIB
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
    Graph g(edges.begin(), edges.end(), &weights[0], N);
    goToSection(is, "Terminals");
    T = readInt(is, "Terminals");
    terminals.resize(T);
    for(int i : boost::irange(0,T)) {
        terminals[i] = readInt(is, "T");
    }
    return g;
}

void readLine(std::istream & is, std::string & fname, int & OPT) {
    int dummy;
    std::string dummys;
    is >> fname >> dummy >> dummy >> dummy >> dummys >> OPT;
    fname += ".stp";
}

void readSTEINLIBtests(std::vector<SteinerTreeTest>& data) {
    std::string testDir = "test/data/STEINLIB/";
    std::ifstream is_test_cases(testDir + "/index");
    assert(is_test_cases.good());
    while (is_test_cases.good()) {
        std::string fname;
        int opt;
        readLine(is_test_cases, fname, opt);
        if (fname == ".stp")
            return;
        std::ifstream ifs(testDir + "/I080/" + fname);
        assert(ifs.good());
        assert(ifs.good());
        std::vector<int> terminals;
        std::vector<int> steinerPoints;
        Graph graph(paal::readSTEINLIB(ifs, terminals, steinerPoints));
        SteinerTreeTest test(fname, opt, terminals, steinerPoints, graph);
        data.push_back(test);
    }
}

}
#endif /* READ_ORLIB_FC_HPP */
