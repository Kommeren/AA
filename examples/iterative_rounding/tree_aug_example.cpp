#include <iostream>                  // for std::cout
#include <fstream>
// #include <tuple>

// #include <utility>                   // for std::pair
// #include <algorithm>                 // for std::for_each
// #include <boost/graph/graph_traits.hpp>
// #include <boost/graph/adjacency_list.hpp>
// //  #include <boost/graph/dijkstra_shortest_paths.hpp>
// #include <boost/graph/filtered_graph.hpp>
// #include <boost/graph/graph_utility.hpp>
// #include <boost/graph/breadth_first_search.hpp>



// #include "paal/iterative_rounding/iterative_rounding.hpp"
// #include "paal/iterative_rounding/ir_components.hpp"

#include "paal/iterative_rounding/treeaug/tree_aug.hpp"
//#include "paal/iterative_rounding/glp_lpbase.hpp"

using namespace boost;

// create a typedef for the Graph type
typedef adjacency_list<vecS, vecS, undirectedS,
		       //property < vertex_index_t, int >,
		       no_property,
		       property < edge_weight_t, double,  
				  property < edge_color_t, bool> > > Graph;


typedef adjacency_list_traits < vecS, vecS, undirectedS > Traits;
typedef Traits::vertex_descriptor Vertex;
typedef Traits::edge_descriptor  Edge;

typedef property_map < Graph, edge_weight_t >::type Cost;
typedef property_map < Graph, edge_color_t >::type TreeMap;


void readTreeAugFromFile(const std::string & filename, 
		 Graph & g, Cost & cost, TreeMap & treeMap) {
  std::string s;

  std::ifstream is(filename.c_str());    

  if (!is){
    std::cerr<<"File "<<filename<<" could not be opened."<<std::endl;
    return;
  }
    

  int verticesNum, edgesNum;
    
  is >> s; is>>verticesNum; is >> s; 
    
  for (int i = 0; i < verticesNum; i++) {
    int dummy;
    is >> dummy;
    add_vertex(g);
    //degBounds[u] = b;
  }
    
  is >> s; is>>edgesNum; is >> s; is >> s; is >> s;
    
  for (int i = 0; i < edgesNum; i++) {
    //read from the file
    int u, v;
    double c;
    int dummy;
    bool inT;
    is >> u >> v >> dummy >> c>> inT;

    bool b;
    Traits::edge_descriptor e;
    std::tie(e, b) = add_edge(u, v, g);
    assert(b);
    //cost[e]=c;
    put(cost,e,c);
    put(treeMap,e,inT);

  }

}
///The main entry point
int main(int argc, char* argv[])
{
  
  if (argc != 2) {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " <problem_instance> " << std::endl;   
    std::cerr <<  "where <problem_instance> is a file describing a problem instance. The file examples/tree_aug_example_input.lgfmod is an example of such a file. " << std::endl;   

    return -1;
  }

  Graph g;
  Cost cost      = get(edge_weight, g);
  TreeMap treeMap      = get(edge_color, g);
    
  //  std::string eT_file=argv[1];

  readTreeAugFromFile(argv[1],g,cost,treeMap);

  paal::ir::TreeAug<Graph, TreeMap, Cost> treeaug(g,treeMap,cost);
  
  std::string error;
  if (!treeaug.checkInputValidity(error)){
      std::cerr<<"The input is not valid!"<<std::endl;
      std::cerr<<error<<std::endl;
      return -1;
  }
  
  paal::ir::IterativeRounding<decltype(treeaug)> ir(treeaug);


  
  paal::ir::solve_iterative_rounding(ir);

  std::cout<<"The total cost of the solution is "<<treeaug.getSolutionValue()<<std::endl;


  typename boost::property_map<Graph, boost::vertex_index_t>::type  index = get(boost::vertex_index, g);
	
  auto & sol = ir.getSolution();
  
  std::cout<<"The solution found contains the following nontree edges:"<<std::endl;

  int numEdges(0);
  double totalCost(0);
  for (const std::pair<Edge, bool> & e : sol) {
    if (e.second) {
      ++numEdges;
      totalCost += cost[e.first];
      std::cout << index[source(e.first,g)]<<"-"<<index[target(e.first,g)] << " ";

    }
  }
  std::cout<<std::endl;
  std::cout<<"The total cost of the solution is "<<totalCost<<std::endl;
  std::cout<<"The total cost of the solution is "<<ir.getSolutionValue()<<std::endl;


  return 0;
}
