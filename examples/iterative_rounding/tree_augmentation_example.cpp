#include <iostream>                  // for std::cout
#include <fstream>

#include "paal/iterative_rounding/treeaug/tree_augmentation.hpp"

//! [Tree Augmentation Example]
using namespace boost;

// create a typedef for the Graph type
typedef adjacency_list<vecS, vecS, undirectedS,
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
        put(cost,e,c);
        put(treeMap,e,inT);

    }

}
///The main entry point
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " <problem_instance> " << std::endl;   
        std::cerr <<  "where <problem_instance> is a file describing a problem instance. The file examples/tree_aug_example_input.lgfmod is an example of such a file. " << std::endl;   

        return -1;
    }

    Graph g;
    Cost cost      = get(edge_weight, g);
    TreeMap treeMap      = get(edge_color, g);

    readTreeAugFromFile(argv[1],g,cost,treeMap);
            
    typedef std::set< Edge> SetEdge;
    SetEdge solution;

    std::string error;
    //TODO handle
/*    if (!treeaug.checkInputValidity(error)){
        std::cerr<<"The input is not valid!"<<std::endl;
        std::cerr<<error<<std::endl;
        return -1;
    }*/

    paal::ir::tree_augmentation_iterative_rounding(g, treeMap, cost, solution, paal::ir::TAComponents<>());

    //TODO handle
//    std::cout<<"The total cost of the solution is "<<treeaug.getSolutionValue()<<std::endl;

    auto index = get(boost::vertex_index, g);

    std::cout<<"The solution found contains the following nontree edges:"<<std::endl;

    int numEdges(0);
    double totalCost(0);
    for (const Edge & e : solution) {
        //TODO change after adding of cost computation to the IR
        ++numEdges;
        totalCost += cost[e];
        std::cout << get(index, source(e,g)) << "-" << get(index, target(e,g)) << " ";
    }
    std::cout<<std::endl;
    std::cout<<"The total cost of the solution is "<<totalCost<<std::endl;
    //TODO handle
//    std::cout<<"The total cost of the solution is "<<treeaug.getSolutionValue()<<std::endl;
    paal::lp::GLP::freeEnv();

    return 0;
}
//! [Tree Augmentation Example]
