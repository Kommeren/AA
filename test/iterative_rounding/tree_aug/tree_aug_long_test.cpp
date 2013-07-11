/**
 * @file tree_aug_long_test.cpp
 * @brief 
 * @author Attila Bernath
 * @version 1.0
 * @date 2013-07-08
 */

#define BOOST_TEST_MODULE tree_aug_long_test


#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "utils/logger.hpp"
#include "paal/iterative_rounding/tree_aug/tree_aug.hpp"

using namespace  paal;
using namespace  paal::ir;
using namespace  boost;


// create a typedef for the Graph type
typedef adjacency_list<vecS, vecS, undirectedS,
		       //property < vertex_index_t, int >,
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
// @nodes	6
// label
// 0
// 1
// 2
// 3
// 4
// 5
// @edges	10
// 		label	intree	cost
// 0	1	0	1	0
// 1	2	1	1	0
// 1	3	2	1	0
// 3	4	3	1	0
// 3	5	4	1	0
// 0	3	0	0	1
// 0	2	1	0	1
// 2	4	2	0	1
// 2	5	3	0	1
// 4	5	4	0	1
void readTreeAugFromStream(  std::ifstream & is, 
		 Graph & g, Cost & cost, TreeMap & treeMap) {
  std::string s;
    
  std::map< std::string, Vertex> nodes;
  int verticesNum, edgesNum;
    
  is >> s; is>>verticesNum; is >> s; 
    
  for (int i = 0; i < verticesNum; i++) {
      std::string nlabel;
    is >> nlabel;
    nodes[nlabel]=add_vertex(g);
    //degBounds[u] = b;
  }

  std::cout<<num_vertices(g)<<std::endl;

  is >> s; is>>edgesNum; is >> s; is >> s; is >> s;
    
  for (int i = 0; i < edgesNum; i++) {
    //read from the file
      std::string u, v;
      double c;
      int dummy;
      bool inT;
      is >> u >> v >> dummy >> inT >> c;

    bool b;
    Traits::edge_descriptor e;
    std::tie(e, b) = add_edge(nodes[u], nodes[v], g);
    assert(b);
    //cost[e]=c;
    put(cost,e,c);
    put(treeMap,e,inT);

    // if (num_vertices(g)>2391){
    //     std::cout<<u<<","<<v<<std::endl;
    //     exit(1);
    // }
        

  }
  
  //std::cout<<num_vertices(g)<<std::endl;

}


// void checkResult(Graph & g, std::map<Edge, bool> & tree,
//                  Cost & costs, Bound & degBounds,
//                  int verticesNum, double bestCost) {
//     int treeEdges(0);
//     double treeCost(0);
//     for (const std::pair<Edge, bool> & e : tree) {
//         if (e.second) {
//             ++treeEdges;
//             treeCost += costs[e.first];
//         }
//     }

//     LOG("tree edges: " << treeEdges);
//     BOOST_CHECK(treeEdges == verticesNum - 1);
//     BOOST_CHECK(treeCost <= bestCost);
    
//     auto verts = vertices(g);
//     int numOfViolations(0);
    
//     for (const Vertex & v : paal::utils::make_range(verts.first, verts.second)) {
//         int treeDeg(0);
//         auto adjVertices = adjacent_vertices(v, g);
        
//         for(const Vertex & u : paal::utils::make_range(adjVertices.first, adjVertices.second)) {
//             bool b; Edge e;
//             std::tie(e, b) = boost::edge(v, u, g);
//             assert(b);
            
//             if (tree[e]) {
//                 ++treeDeg;
//             }
//         }
        
//         BOOST_CHECK(treeDeg <= degBounds[v] + 1);
//         if (treeDeg > degBounds[v]) {
//             ++numOfViolations;
//         }
//     }
        
//     LOG("Found cost = " << treeCost << ", cost upper bound = " << bestCost);
//     LOG("Number of violated constraints = " << numOfViolations);
        
//     Graph treeG(verticesNum);
        
//     for (const std::pair<Edge, bool> & e : tree) {
//         if (e.second) {
//             add_edge(source(e.first, g), target(e.first, g), treeG);
//         }
//     }
        
//     std::vector<int> component(verticesNum);
//     BOOST_CHECK(connected_components(treeG, &component[0]) == 1);
// }

BOOST_AUTO_TEST_CASE(tree_aug_long) {
    std::string testDir = "../test/data/TREEAUG/";
    std::ifstream is_test_cases(testDir + "tree_aug.txt");

    assert(is_test_cases.good());
    while(is_test_cases.good()) {
        std::string fname;
        int MAX_LINE = 256;
        char buf[MAX_LINE];
        int verticesNum, edgesNum;
        double bestCost;
        is_test_cases.getline(buf, MAX_LINE);
        if(buf[0] == 0) {
            return;
        }
        
        if(buf[0] == '#')
            continue;
        std::stringstream ss;
        ss << buf;
        
        ss >> fname;

        LOG(fname);
        std::string filename=testDir + "/cases/" + fname + ".lgf";
        std::ifstream ifs(filename);

        if (!ifs){
            std::cerr<<"File "<<filename<<" could not be opened."<<std::endl;
            return;
        }
        

        Graph g;
        Cost cost      = get(edge_weight, g);
        TreeMap treeMap      = get(edge_color, g);
    
        //std::cerr<<"Reading "<<filename<<"."<<std::endl;

        readTreeAugFromStream(ifs,g,cost,treeMap);
        


        
        paal::ir::TreeAug<Graph, TreeMap, Cost> treeaug(g,treeMap,cost);
        //std::cerr<<"Init "<<filename<<" ends."<<std::endl;
        //std::cerr<<"Reading "<<num_vertices(g)<<" ends."<<std::endl;
        
        std::string error;
        BOOST_ASSERT_MSG(treeaug.checkInputValidity(error),error.c_str());
        std::cerr<<"Inputvalidation "<<filename<<" ends."<<std::endl;
        
        paal::ir::IterativeRounding<decltype(treeaug)> irw(treeaug);

        paal::ir::solve_iterative_rounding(irw);
        

    }
}
