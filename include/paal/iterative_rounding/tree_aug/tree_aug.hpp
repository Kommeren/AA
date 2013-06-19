/**
 * @file generalised_assignment.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-05-06
 */
#ifndef TREE_AUG_HPP
#define TREE_AUG_HPP

#include <iostream>                  // for std::cout
#include <fstream>
#include <tuple>

#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
//  #include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>



#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/ir_components.hpp"


//using namespace boost;

namespace paal {
  namespace ir {


    template <typename EdgeBoolMap, bool negate>
    struct BoolMapToFilter {
      BoolMapToFilter() { }
      BoolMapToFilter(EdgeBoolMap  m) : ebmap(m) { }
      template <typename Edge>
      bool operator()(const Edge& e) const {
	if (negate)
	  return get(ebmap, e);
	else
	  return !get(ebmap, e);
      }
      EdgeBoolMap ebmap;
    };


    template <typename Graph, typename TreeMap, typename CostMap >
    class TreeAug : public IRComponents < > {
    public:

                           
      typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
      typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;

      typedef std::map< Edge, bool > EdgeBoolMap;

      TreeAug(const Graph & _g, const TreeMap & _treeMap, const CostMap & _costMap) :
	IRComponents< >(),
	g(_g),  treeMap(_treeMap), costMap(_costMap),
	inTreeFilter(treeMap),tree(g,inTreeFilter),
	nonTreeFilter(treeMap),ntree(g,nonTreeFilter){ }
    

      
      template <typename LP>
      std::pair<bool, double> roundCondition(const LP & lp, int col) 
      {
	double x = lp.getColPrim(col);
        if(utils::Compare<double>::ge(x,0.5)) {
	  inSolution[colName2Edge[lp.getColName(col)]]=true;
	  return std::make_pair(true, 1);
        }
        return std::make_pair(false, -1);

      }
    
      template <typename LP>
      bool relaxCondition(const LP & lp, int row) {
        // if (isDegBoundName(lp.getRowName(row))) {
	//   int vIdx = getDegBoundIndex(lp.getRowName(row));
	//   Vertex v = m_vertexList[vIdx];
	//   return Compare::le(nonZeroIncEdges(lp, v), m_degBoundMap[v] + 1);
        // }
        // else {

	for (Edge e: coveredBy[rowName2Edge[lp.getRowName(row)]])
	  {
	    if (inSolution[e])
	      return true;
	  }
	return false;
      // }
      }

      template <typename LP>
      void init(LP & lp) {    

	std::cout << "The graph:" << std::endl;
	print_graph(g);

	std::cout << "The spanning tree:" << std::endl;
	print_graph(tree);

	std::cout << "The rest:" << std::endl;
	print_graph(ntree);


	// get the property map for vertex indices
	typedef typename boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
	IndexMap index = get(boost::vertex_index, g);
	int numberOfVertices=num_vertices(g);


  
	std::vector< Edge > pred(numberOfVertices);

	{
	  typename boost::graph_traits<NonTreeGraph>
	    ::edge_iterator ei, ei_end;
	  for(boost::tie(ei,ei_end) = edges(ntree); ei != ei_end; ++ei){
	    std::cout << index[source(*ei,ntree)]<<"-"<<index[target(*ei,ntree)] << " ";
    
	    //typename graph_traits<TreeGraph>::vertex_descriptor a = source(*ei,ntree);

	    breadth_first_search(tree, source(*ei,ntree),
				 visitor(make_bfs_visitor(record_edge_predecessors(&pred[0], boost::on_tree_edge()))));
      
	    //my_bfs(tree,source(*ei,ntree),predEdgeMap);
      
	    Vertex node=target(*ei,ntree);
	    while (node!=source(*ei,ntree)){
	      covers[*ei].push_back(pred[node]);
	      coveredBy[pred[node]].push_back(*ei);
      
	      node=source(pred[node],tree);
	    }



	    // breadth_first_search(tree, source(*ei,ntree),
	    // 			 make_bfs_visitor(record_edge_predecessors(pred.begin(), on_tree_edge())));

	  }
	  std::cout << std::endl;
	}
	{
	  typename boost::graph_traits<NonTreeGraph>
	    ::edge_iterator ei, ei_end;
	  for(boost::tie(ei,ei_end) = edges(ntree); ei != ei_end; ++ei){
	    std::cout << index[source(*ei,ntree)]<<"-"<<index[target(*ei,ntree)] << "  has path in the tree: ";
	    for (auto pe:covers[*ei]){
	      std::cout << index[source(pe,tree)]<<"-"<<index[target(pe,tree)] << ", ";
	
	    }
	    std::cout << std::endl;    
	  }  
	}




        lp.setLPName("Tree augmentation");
        lp.setMinObjFun(); 

	int num=addVariables(lp);

	std::cout<<"Number of columns initially: "<<num<<std::endl;
	num=addCutConstraints(lp);
	std::cout<<"Number of rows initially: "<<num<<std::endl;

        // addDegreeBoundConstraints(lp);
        // addAllSetEquality(lp);
        
        lp.loadMatrix();
        
        // BoundedDegreeMSTBase::m_solveLP.setOracle(&m_separationOracle);
        // m_separationOracle.init(&m_g, &m_vertexList, &m_edgeMap);
      }
    
    template <typename LP>
    EdgeBoolMap & getSolution(const LP & lp) {
      return inSolution;
    }


    private:
      const Graph & g;
      const TreeMap & treeMap;
      const CostMap & costMap;
      
      BoolMapToFilter<TreeMap,true> inTreeFilter;
      typedef boost::filtered_graph<Graph, BoolMapToFilter<TreeMap,true> > TreeGraph;
      TreeGraph  tree;
      
      BoolMapToFilter<TreeMap,false> nonTreeFilter;
      typedef boost::filtered_graph<Graph, BoolMapToFilter<TreeMap,false> > NonTreeGraph;
      NonTreeGraph ntree;

      //Structures for the "covers" and "coveredBy" relations
      typedef std::list< Edge > EdgeList;
      typedef std::map< Edge , EdgeList > CoverMap;
      CoverMap covers,coveredBy;
      
      typedef std::map<Edge, std::string> Edge2ColName;
      Edge2ColName edge2ColName;

      typedef std::map<std::string, Edge> ColName2Edge;
      ColName2Edge colName2Edge;
      
      std::map<std::string, Edge> rowName2Edge;
      
      EdgeBoolMap inSolution;


      std::string getEdgeName(int eIdx) const {
        return std::to_string(eIdx);
      }

      std::string getCutConstrPrefix() const {
        return "cutConstraint";
      }      
      std::string getRowName(int dbIdx) const {
        return getCutConstrPrefix() + std::to_string(dbIdx);
      }

      //adding variables
      //returns the number of variables
      template <typename LP>
      int addVariables(LP & lp) {
	auto edges = boost::edges(ntree);
	int eIdx(0);
	int whatever=42;
	
	for(Edge e : utils::make_range(edges.first, edges.second)) {
	  std::string colName = getEdgeName(eIdx);
	  lp.addColumn(costMap[e], LO, 0, whatever, colName);
	  edge2ColName[e] = colName;
	  colName2Edge[colName]=e;
	  inSolution[e] = false;
	  // m_edgeList[eIdx] = e;
	  ++eIdx;
	}
	return eIdx;
      }
    
      //adding variables
      //returns the number of rows added
      template <typename LP>
      int addCutConstraints(LP & lp) {
	int whatever=42;
	int dbIndex=0;
	typename boost::graph_traits<TreeGraph>
	  ::edge_iterator ei, ei_end;
	for(boost::tie(ei,ei_end) = edges(tree); ei != ei_end; ++ei){
	  std::string rowName = getRowName(dbIndex);
	  int rowIdx = lp.addRow(LO, 1, whatever, rowName );
	  rowName2Edge[rowName]=*ei;
	  std::cout<<rowIdx<<":  ";

	  for (Edge pe:coveredBy[*ei]){
	    std::cout<<edge2ColName[pe]<<", ";
	    lp.addConstraintCoef(rowIdx, lp.getColByName(edge2ColName[pe]));
	    
	  }
	  std::cout<<std::endl;
	  
	  // m_edgeMap[e] = colName;
	  // m_spanningTree[e] = false;
	  // m_edgeList[eIdx] = e;
	  ++dbIndex;
	}
	return dbIndex;
      }
      
      
  
    };



    template <typename Graph, typename TreeMap, typename CostMap >
    TreeAug<Graph,  TreeMap,  CostMap >
    makeTreeAug(const Graph & _g, const TreeMap & _treeMap, const CostMap & _costMap) {
      
      return TreeAug< Graph,  TreeMap,  CostMap >(_g,  _treeMap,  _costMap);
      
    }

    // template <typename Graph, typename CostMap, typename DegreeBoundMap>
    // BoundedDegreeMST<Graph, CostMap, DegreeBoundMap>
    // make_BoundedDegreeMST(const Graph & g, const CostMap & costMap, const DegreeBoundMap & degBoundMap) {
    //   return BoundedDegreeMST<Graph, CostMap, DegreeBoundMap>(g, costMap, degBoundMap);
    // }
  } //ir
} //paal

#endif /* TREE_AUG_HPP */
