/**
 * @file tree_augmentation_long_test.cpp
 * @brief
 * @author Attila Bernath
 * @version 1.0
 * @date 2013-07-08
 */


#include <fstream>
#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "paal/iterative_rounding/treeaug/tree_augmentation.hpp"

#include "utils/parse_file.hpp"
#include "utils/logger.hpp"

using namespace  paal;
using namespace  paal::ir;
using namespace  boost;


// create a typedef for the Graph type
typedef adjacency_list<vecS, vecS, undirectedS,
        no_property,
        property < edge_weight_t, double,
        property < edge_color_t, bool> > > Graph;

typedef adjacency_list_traits < vecS, vecS, undirectedS > Traits;
typedef graph_traits < Graph >::vertex_descriptor Vertex;
typedef graph_traits < Graph >::edge_descriptor Edge;

typedef property_map < Graph, vertex_index_t >::type Index;
typedef property_map < Graph, edge_weight_t >::type Cost;
typedef property_map < Graph, edge_color_t >::type TreeMap;

//Read instance in format
// @nodes 6
// label
// 0
// 1
// 2
// 3
// 4
// 5
// @edges 10
//                 label   intree  cost
// 0       1       0       1       0
// 1       2       1       1       0
// 1       3       2       1       0
// 3       4       3       1       0
// 3       5       4       1       0
// 0       3       0       0       1
// 0       2       1       0       1
// 2       4       2       0       1
// 2       5       3       0       1
// 4       5       4       0       1
void readTreeAugFromStream(std::ifstream & is,
        Graph & g, Cost & cost, TreeMap & treeMap) {
    std::string s;
    std::unordered_map<std::string, Vertex> nodes;
    int verticesNum, edgesNum;
    is >> s; is >> verticesNum; is >> s;

    for (int i = 0; i < verticesNum; i++) {
        std::string nlabel;
        is >> nlabel;
        nodes[nlabel] = add_vertex(g);
    }

    LOGLN(num_vertices(g));

    is >> s; is >> edgesNum; is >> s; is >> s; is >> s;

    for (int i = 0; i < edgesNum; i++) {
        // read from the file
        std::string u, v;
        double c;
        int dummy;
        bool inT;
        is >> u >> v >> dummy >> inT >> c;

        bool b;
        Traits::edge_descriptor e;
        std::tie(e, b) = add_edge(nodes[u], nodes[v], g);
        assert(b);
        put(cost, e, c);
        put(treeMap, e, inT);
    }
}

template <typename TA>
//the copy is intended
double getLowerBound(TA ta) {
    paal::ir::TAComponents<> comps;
    lp::GLPBase lp;
    comps.call<Init>(ta, lp);
    auto probType = comps.call<SolveLPToExtremePoint>(ta, lp);
    BOOST_CHECK_EQUAL(probType, lp::OPTIMAL);
    return lp.getObjValue();
}


BOOST_AUTO_TEST_CASE(tree_augmentation_long) {
    std::string testDir = "test/data/TREEAUG/";
    parse(testDir + "tree_aug.txt", [&](const std::string & fname, std::istream &) {
        LOGLN(fname);
        std::string filename = testDir + "cases/" + fname + ".lgf";
        std::ifstream ifs(filename);

        if (!ifs) {
            std::cerr << "File " << filename << " could not be opened." << std::endl;
            return;
        }

        Graph g;
        Cost cost       = get(edge_weight, g);
        TreeMap treeMap = get(edge_color, g);

        readTreeAugFromStream(ifs, g, cost, treeMap);
        typedef std::vector<Edge> SetEdge;
        SetEdge solution;

        auto treeaug(paal::ir::make_TreeAug(g, treeMap, cost, std::back_inserter(solution)));

        auto invalid = treeaug.checkInputValidity();
        BOOST_ASSERT_MSG(!invalid, invalid->c_str());
        LOGLN("Input validation " << filename << " ends.");

        paal::ir::TAComponents<> comps;

        double lplowerbd = getLowerBound(treeaug);
        auto probType = paal::ir::solve_iterative_rounding(treeaug, comps);
        BOOST_CHECK_EQUAL(probType, lp::OPTIMAL);

        double solval = treeaug.getSolutionValue();
        LOGLN("Cost of solution found: " << solval << ", LP lower bound: " << lplowerbd);
        BOOST_CHECK(solval <= 2 * lplowerbd);
    });
}
