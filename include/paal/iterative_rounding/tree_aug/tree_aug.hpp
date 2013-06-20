/**
 * @file tree_aug.hpp
 * @brief 
 * @author Attila Bernath
 * @version 1.0
 * @date 2013-06-20
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
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>



#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/ir_components.hpp"


//using namespace boost;

namespace paal {
  namespace ir {


    ///A class that translates bool map on the edges to a filter, which can be used with
    ///boost::filtered_graph. That is, we translate operator[] to
    ///operator().  We do a little more: we will also need the negated
    ///version of the map (for the graph of the non-tree edges).
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


    /**
     * class TreeAug @brief This is Jain's iterative rounding
     * 2-approximation algorithm for the Generalised Steiner Network
     * Problem, specialized for the Tree Augmentation Problem
     *
     * Example:  tree_aug_example.cpp
     *
     * @tparam Graph the graph type used
     * @tparam TreeMap it is assumed to be a bool map on the edges of a graph of type Graph. It is used for designating a spanning tree in the graph.
     * @tparam CostMap type for the costs of the links.
     */
    template <typename Graph, typename TreeMap, typename CostMap >
    class TreeAug : public IRComponents < > {
    public:

                           
      typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
      typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;

      typedef std::map< Edge, bool > EdgeBoolMap;

      ///Constructor.
      ///
      ///@param _g  the graph to work with
      ///@param _treeMap designate a spanning tree in \c _g
      ///@param _costMap costs of the links (=non-tree edges). The costs assigned to tree edges are not used.
      TreeAug(const Graph & _g, const TreeMap & _treeMap, const CostMap & _costMap) :
	IRComponents< >(),
	g(_g),  treeMap(_treeMap), costMap(_costMap),
	inTreeFilter(treeMap),tree(g,inTreeFilter),
	nonTreeFilter(treeMap),ntree(g,nonTreeFilter)
      { 
	
	//we print out the input.
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


	//We need to fill two very useful auxiliary data structures:
	//\c covers and \c coveredBy. These are containing lists of
	//edges. For a tree edge \c t the list \c coveredBy[t]
	//contains the list of links covering \c t. Similarly, for a
	//link \c e, the list \c covers[e] contains the list of tree
	//edges covered by \c e (that is, the tree edges in the unique
	//path in the tree between the endnodes of \c e).
	{
	  std::vector< Edge > pred(numberOfVertices);	  
	  std::map< Vertex, bool > wasSeen;
	  typename boost::graph_traits<Graph>
	    ::vertex_iterator ui, ui_end;
	  for (boost::tie(ui,ui_end) = vertices(g); ui != ui_end; ++ui) {
	    wasSeen[*ui]=true;

	    breadth_first_search(tree, *ui,
				 visitor(make_bfs_visitor(record_edge_predecessors(&pred[0], boost::on_tree_edge()))));

	    typename boost::graph_traits<NonTreeGraph>::out_edge_iterator ei, ei_end;
	    for(boost::tie(ei,ei_end) = out_edges(*ui,ntree); ei != ei_end; ++ei){
	      Vertex node=target(*ei,ntree);
	      if (!wasSeen[node]){
		while (node!=*ui){
		    covers[*ei].push_back(pred[node]);
		    coveredBy[pred[node]].push_back(*ei);
		    node=source(pred[node],tree);
		  }
	      }
	    }
		
	  }	
	  std::cout << std::endl;
      
	}
	//This is just for debug purposes
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

      }//end of Constructor
    
      ///Initialize the cut LP.
      template <typename LP>
      void init(LP & lp) {    

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

      
      ///A variable is rounded up to 1, if it has value at least half in the solution
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
    
      ///A condition belonging to a tree edge \c t can be relaxed if
      ///some link in the fundamental cut belonging to \c t is chosen
      ///in the solution.
      ///
      ///In fact we don't even need to relax, since these constraints became trivial after rounding. So returning always false should be fine, too.
      template <typename LP>
      bool relaxCondition(const LP & lp, int row) {

	for (Edge e: coveredBy[rowName2Edge[lp.getRowName(row)]])
	  {
	    if (inSolution[e])
	      return true;
	  }
	return false;
      }

    
      ///Return the solution found
      template <typename LP>
      EdgeBoolMap & getSolution(const LP & lp) {
	return inSolution;
      }


    private:
      ///The input
      const Graph & g;
      const TreeMap & treeMap;
      const CostMap & costMap;
      
      ///Auxiliary data structures
      BoolMapToFilter<TreeMap,true> inTreeFilter;
      typedef boost::filtered_graph<Graph, BoolMapToFilter<TreeMap,true> > TreeGraph;
      //The spanning tree
      TreeGraph  tree;
      
      BoolMapToFilter<TreeMap,false> nonTreeFilter;
      typedef boost::filtered_graph<Graph, BoolMapToFilter<TreeMap,false> > NonTreeGraph;
      //The non-tree (=set of links)
      NonTreeGraph ntree;

      //Structures for the "covers" and "coveredBy" relations
      typedef std::list< Edge > EdgeList;
      typedef std::map< Edge , EdgeList > CoverMap;
      CoverMap covers,coveredBy;
      

      //cross reference between links and column names
      std::map<Edge, std::string> edge2ColName;
      std::map<std::string, Edge> colName2Edge;

      //reference between tree edges and row names
      std::map<std::string, Edge> rowName2Edge;
      
      //Which links are chosen in the solution
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
    
      //adding the cut constraints
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

  } //ir
} //paal

#endif /* TREE_AUG_HPP */
