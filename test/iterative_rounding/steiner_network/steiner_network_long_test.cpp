/**
 * @file steiner_network_long_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-10
 */

#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <utility>
#include <map>
#include <cstdlib>

#include <boost/range/irange.hpp>
#include <boost/range/join.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

#include "paal/iterative_rounding/steiner_network/steiner_network.hpp"

#include "utils/logger.hpp"
#include "utils/read_bounded_deg_mst.hpp"
#include "utils/parse_file.hpp"

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                        boost::property<boost::vertex_index_t, int>,
                        boost::property<boost::edge_weight_t, double>> Graph;
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::undirectedS> Traits;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;

typedef boost::property_map<Graph, boost::edge_weight_t>::type Cost;

typedef std::vector<std::vector<int>> RestrictionVec;
typedef std::map<std::pair<int, int>, int> Edges;

namespace {

int rand(int min, int max) {
    return ::rand() % (max - min + 1) + min;
}

int randCost() {
    static const int MAX_COST = 100;
    return rand(1, MAX_COST);
}

void generateRestrictions(RestrictionVec & res, int maxRes) {
    for (int i : boost::irange(0, int(res.size()))) {
        for (int j : boost::irange(i+1, int(res.size()))) {
            res[i][j] = res[j][i] = rand(2, maxRes);
        }
    }
}

bool nonZero(const std::vector<int> & vec) {
    for (auto v : vec)
        if (v != 0)
            return true;

    return false;
}

bool nonZero(const RestrictionVec & res) {
    for (auto row : res)
        if (nonZero(row))
            return true;

    return false;
}

void dec(std::vector<int> & vec) {
    for (auto & v : vec)
        v = std::max(0, v-1);
}

void addEdge(Edges & edges, int u, int v, int & edgeNum) {
    auto edge = std::make_pair(std::min(u,v), std::max(u,v));
    if (edges.find(edge) == edges.end()) {
        edges.insert(std::make_pair(edge, 1));
        ++edgeNum;
    }
    else {
        ++edges[edge];
        edgeNum += 2;
    }
}

void randomTree(Edges & edges, RestrictionVec & restrictions,
        int specialVertices, int verticesNum, int & edgeNum) {
    int first = rand(specialVertices, verticesNum - 2);
    std::vector<int> vertices(1, first);

    // Generates edge between current vertex and already processed (tree) vertices.
    auto generateTreeEdge = [&](int v){
        int index = rand(0, vertices.size() - 1);
        addEdge(edges, v, vertices[index], edgeNum);
        vertices.push_back(v);
    };

    // Generate a random tree on non-special vertices.
    for (int v : boost::join(boost::irange(first + 1, verticesNum),
                        boost::irange(specialVertices, first))) {
        generateTreeEdge(v);
    }

    // Join special vertices with non-zero restrictions to the random tree.
    // There is be a path in the tree between every pair of vertices,
    // so we decrement special vertices restrictions.
    for (int v : boost::irange(0, specialVertices)) {
        if (nonZero(restrictions[v])) {
            generateTreeEdge(v);
            dec(restrictions[v]);
        }
    }
}

void fillGraph(Graph & g, const Edges & edges, int & verticesNum) {
    g.clear();
    auto cost = get(boost::edge_weight, g);
    for (auto e : edges) {
        // add regular edge
        paal::addEdge(g, cost, e.first.first, e.first.second, randCost());
        // add new vertices on multi-edges, so that we get a graph not a multigraph
        for (int i = 1; i < e.second; ++i) {
            paal::addEdge(g, cost, e.first.first, verticesNum, randCost());
            paal::addEdge(g, cost, e.first.second, verticesNum, randCost());
            ++verticesNum;
        }
    }
}

struct CliqueTag{};
struct SparseGraphTag{};

int generateInstance(Graph & g, RestrictionVec & res, int verticesNum,
        int minEdgeNum, int specialVertices, int maxRes, unsigned int seed,
        SparseGraphTag) {
    srand(seed);
    assert(specialVertices + 1 < verticesNum);
    assert(maxRes >= 2);

    int edgeNum(0);
    Edges edges;
    res = RestrictionVec(specialVertices, std::vector<int>(specialVertices, 0));

    generateRestrictions(res, maxRes);
    RestrictionVec tempRes = res;

    while (nonZero(tempRes)) {
        randomTree(edges, tempRes, specialVertices, verticesNum, edgeNum);
    }

    while (edgeNum < minEdgeNum) {
        int u = rand(0, verticesNum - 1);
        int v = rand(u + 1, verticesNum);
        addEdge(edges, u, v, edgeNum);
    }

    fillGraph(g, edges, verticesNum);

    LOGLN("edges: " << edgeNum);

    return verticesNum;
}

int generateInstance(Graph & g, RestrictionVec & res, int verticesNum,
        int minEdgeNum, int restrictedVertices, int maxRes, unsigned int seed,
        CliqueTag) {
    srand(seed);
    assert(restrictedVertices <= verticesNum);
    assert(maxRes >= 2);

    res = RestrictionVec(restrictedVertices, std::vector<int>(restrictedVertices, 0));
    generateRestrictions(res, maxRes);

    auto cost = get(boost::edge_weight, g);
    for (int i : boost::irange(0, verticesNum)) {
        for (int j : boost::irange(i+1, verticesNum)) {
            paal::addEdge(g, cost, i, j, randCost());
        }
    }

    return verticesNum;
}

template <template <typename> class Oracle, typename Restrictions>
void runSingleTest(const Graph & g, const Cost & costs, const Restrictions & restrictions) {
    namespace ir = paal::ir;
    namespace lp = paal::lp;

    typedef std::vector<Edge> ResultNetwork;
    ResultNetwork resultNetwork;
    auto steinerNetwork(ir::make_SteinerNetwork<
                    ir::SteinerNetworkOracle<Oracle>>(g, restrictions,
                            std::back_inserter(resultNetwork)));
    auto invalid = steinerNetwork.checkInputValidity();
    BOOST_CHECK(!invalid);

    auto result = ir::solve_iterative_rounding(steinerNetwork,
                        ir::SteinerNetworkIRComponents<>());
    BOOST_CHECK(result.first == lp::OPTIMAL);
}

template <typename Restrictions>
void runTest(const Graph & g, const Cost & costs, const Restrictions & restrictions,
        int verticesNum) {
    // default heuristics
    for (int i : boost::irange(0, 5)) {
        LOGLN("random violated, seed " << i);
        srand(i);
        runSingleTest<paal::lp::RandomViolatedSeparationOracle>(g, costs, restrictions);
    }

    // non-default heuristics
    if (verticesNum <= 80) {
        LOGLN("most violated");
        runSingleTest<paal::lp::MostViolatedSeparationOracle>(g, costs, restrictions);
    }

    // non-default heuristics
    if (verticesNum <= 50) {
        LOGLN("first violated");
        runSingleTest<paal::lp::FirstViolatedSeparationOracle>(g, costs, restrictions);
    }
}

template <typename GraphTypeTag>
void runTestCase(const std::vector<int> & verticesNum, const std::vector<int> & minEdgeNum,
        const std::vector<int> & specialVertices, const std::vector<int> & maxRes,
        GraphTypeTag graphTypeTag) {
    Graph g;
    RestrictionVec res;
    auto seed = [](int x) {return 12345 * x;};

    for (int i : boost::irange(0, int(verticesNum.size()))) {
        int vertices = generateInstance(g, res, verticesNum[i], minEdgeNum[i],
                specialVertices[i], maxRes[i], seed(i), graphTypeTag);
        LOGLN("vertices: " << vertices);
        LOGLN("special vertices: " << specialVertices[i]);
        LOGLN("max restriction: " << maxRes[i]);

        auto restrictions = [&](int i, int j){
                if (std::size_t(i) < res.size() && std::size_t(j) < res.size())
                    return res[i][j];
                else
                    return 1;
            };

        runTest(g, get(boost::edge_weight, g), restrictions, vertices);
    }
}

} //namespace

BOOST_AUTO_TEST_SUITE(SteinerNetwork)

BOOST_AUTO_TEST_CASE(steiner_network_long) {
    std::vector<int> verticesNum     = { 6, 10, 10,  10,  15,  20,  40,  50,  75};
    std::vector<int> minEdgeNum      = {30, 50, 80, 200, 200, 250, 400, 350, 500};
    std::vector<int> specialVertices = { 4,  4,  6,   5,   5,  10,  15,  15,  20};
    std::vector<int> maxRes          = { 3,  3,  5,   6,   7,   5,   8,   6,   6};

    runTestCase(verticesNum, minEdgeNum, specialVertices, maxRes, SparseGraphTag());
}

BOOST_AUTO_TEST_CASE(steiner_network_long_clique) {
    std::vector<int> verticesNum     = {10, 20, 40, 60, 80};
    std::vector<int> minEdgeNum      = { 0,  0,  0,  0,  0};
    std::vector<int> specialVertices = { 5,  5, 20, 40, 60};
    std::vector<int> maxRes          = { 5,  8, 15, 25, 30};

    runTestCase(verticesNum, minEdgeNum, specialVertices, maxRes, CliqueTag());
}

BOOST_AUTO_TEST_SUITE_END()
