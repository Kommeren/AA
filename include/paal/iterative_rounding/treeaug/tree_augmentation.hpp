/**
 * @file tree_augmentation.hpp
 * @brief 
 * @author Attila Bernath
 * @version 1.0
 * @date 2013-06-20
 */
#ifndef TREE_AUG_HPP
#define TREE_AUG_HPP

#include <fstream>
#include <tuple>

#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each

#include <boost/bimap.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/stoer_wagner_min_cut.hpp>

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/ir_components.hpp"

#include <boost/graph/iteration_macros.hpp>

namespace paal {
    namespace ir {

        ///This function returns the number of edges in a graph. The
        ///reason this function was written is that BGL's num_edges()
        ///function does not work properly for filtered_graph.  See
        ///http://www.boost.org/doc/libs/1_54_0/libs/graph/doc/filtered_graph.html#2
        template <class EdgeListGraph>
        int my_num_edges(const EdgeListGraph& G)
        {
            typename boost::graph_traits<EdgeListGraph>::edge_iterator ei,ei_end;
            boost::tie(ei,ei_end)=edges(G);
            return std::distance(ei,ei_end);
        }


        ///A class that translates bool map on the edges to a filter, which can be used with
        ///boost::filtered_graph. That is, we translate operator[] to
        ///operator().  We do a little more: we will also need the negated
        ///version of the map (for the graph of the non-tree edges).
        template <typename EdgeBoolMap>
        struct BoolMapToTreeFilter {
            BoolMapToTreeFilter() { }
            BoolMapToTreeFilter(EdgeBoolMap  m) : ebmap(m) { }
            template <typename Edge>
            bool operator()(const Edge& e) const {
                    return get(ebmap, e);
            }
            EdgeBoolMap ebmap;
        };

        template <typename EdgeBoolMap>
        struct BoolMapToNonTreeFilter {
            BoolMapToNonTreeFilter() { }
            BoolMapToNonTreeFilter(EdgeBoolMap  m) : ebmap(m) { }
            template <typename Edge>
            bool operator()(const Edge& e) const {
                    return !get(ebmap, e);
            }
            EdgeBoolMap ebmap;
        };


        ///A boost graph map that returns a constant integer value
        template <typename KeyType, int num>
        class ConstIntMap
            : public boost::put_get_helper<int, ConstIntMap<KeyType, num> >
        {
        public:
            typedef boost::readable_property_map_tag category;
            typedef int value_type;
            typedef int reference;
            typedef KeyType key_type;
            reference operator[ ](KeyType e) const { return num; }
        };




        /**
         * \brief This is Jain's iterative rounding
         * 2-approximation algorithm for the Generalised Steiner Network
         * Problem, specialized for the Tree Augmentation Problem.
         *
         * The Tree Augmentation Problem is the following. Given a
         * 2-edge connected graph, in which a spanning tree is
         * designated. The non-tree edges are also called links. The
         * links have non-negative costs. The problem is to find a
         * minimum cost subset of links which, together with the
         * tree-edges give a 2-edge-connected graph.
         *
         * Example:  tree_augumentaation_example.cpp
         *
         * @tparam Graph the graph type used
         * @tparam TreeMap it is assumed to be a bool map on the edges of a graph of type Graph. It is used for designating a spanning tree in the graph.
         * @tparam CostMap type for the costs of the links.
         */
        template <typename Graph, typename TreeMap, typename CostMap >
        class TreeAug : public IRComponents <DefaultSolveLPToExtremePoint, RoundConditionGreaterThanHalf> {
        public:

            typedef IRComponents <DefaultSolveLPToExtremePoint, RoundConditionGreaterThanHalf> Base;


            typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
            typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
            typedef double CostValue;

            typedef std::map< Edge, bool > EdgeBoolMap;
            typedef std::set< Edge> SetEdge;

            ///Constructor.
            ///
            ///@param _g  the graph to work with
            ///@param _treeMap designate a spanning tree in \c _g
            ///@param _costMap costs of the links (=non-tree edges). The costs assigned to tree edges are not used.
            TreeAug(const Graph & _g, const TreeMap & _treeMap, const CostMap & _costMap) :
                m_g(_g),m_treeMap(_treeMap),m_costMap(_costMap),
                m_inTreeFilter(m_treeMap),m_tree(m_g,m_inTreeFilter),
                m_nonTreeFilter(m_treeMap),m_ntree(m_g,m_nonTreeFilter),m_solCost(0)
            {

            }//end of Constructor

            ///Check validity of the input
            /**This function can be used to check validity of the
               input. The input is valid if it consists of a
               2-edge-connected, in which a spanning tree is
               designated. The input validity is not checked by
               default.
            */
            bool checkInputValidity(std::string & errorMsg){
                //Num of edges == num of nodes-1 in the tree?
                {
                    int nV=num_vertices(m_g);
                    //This is not very nice, but BGL sucks for filtered_graph
                    int nE=my_num_edges(m_tree);

                    if (nE != nV-1){
                        errorMsg="The number of edges in the spanning tree is not good. It is "+std::to_string(nE)+", and it should be "+std::to_string(nV-1)+".";
                        return false;
                    }
                }
                //Is the tree connected?
                {
                    std::vector<int> component(num_vertices(m_g));
                    int num = connected_components(m_tree, &component[0]);

                    if (num>1){
                        errorMsg="The spanning tree is not connected.";
                        return false;
                    }
                }
                //Is the graph 2-edge-connected?
                {
                    ConstIntMap<Edge, 1> const1EdgeMap;
                    //This stoer-wagner algorithm is unnecessarily slow for some reason
                    int w = boost::stoer_wagner_min_cut(m_g, const1EdgeMap);
                    if (w<2){
                        errorMsg="The graph is not 2-edge-connected.";
                        return false;
                    }
                }
                errorMsg="";
                return true;
            }

            ///Round a coordinate up if it is at least half in the
            ///current solution.
            template <typename LP>
            boost::optional<double> roundCondition(LP & lp, ColId col) {

                ///Round a coordinate up if it is at least half.
                auto res = Base::roundCondition(lp, col);
                //Save some auxiliary info
                if(res) {
                    auto tmp=m_Solution.insert(m_edgeToColId.right.at(col));
                    assert(tmp.second);
                    m_solCost+=m_costMap[m_edgeToColId.right.at(col)];
                }
                return res;
            }

            ///Initialize the cut LP.
            template <typename LP>
            void init(LP & lp) {    

                //We need to fill very useful auxiliary data structures:
                //\c m_coveredBy - containing lists of
                //edges. For a tree edge \c t the list \c m_coveredBy[t]
                //contains the list of links covering \c t.
                
                //First we erase the previous conents
                BGL_FORALL_EDGES_T(ei,m_tree,TreeGraph){
                    m_coveredBy[ei].clear();
                }
                
                {
                    //Here we start filling the new content
                    std::vector< Edge > pred(num_vertices(m_g));
                    std::set< Vertex > seen;
                    BGL_FORALL_VERTICES_T(ui, m_g, Graph){
                        auto tmp=seen.insert(ui);
                        assert(tmp.second);
                        breadth_first_search(m_tree, ui,
                                             visitor(make_bfs_visitor(record_edge_predecessors(&pred[0], boost::on_tree_edge()))));

                        typename boost::graph_traits<NonTreeGraph>::out_edge_iterator ei, ei_end;
                        for(boost::tie(ei,ei_end) = out_edges(ui,m_ntree); ei != ei_end; ++ei){
                            Vertex node=target(*ei,m_ntree);
                            if (!seen.count(node)){
                                while (node!=ui){
                                    m_coveredBy[pred[node]].push_back(*ei);
                                    node=source(pred[node],m_tree);
                                }
                            }
                        }

                    }

                }
                lp.setLPName("Tree augmentation");
                lp.setMinObjFun(); 

                addVariables(lp);

                addCutConstraints(lp);

                lp.loadMatrix();
            }

            ///Relax condition.
            ///
            ///A condition belonging to a tree edge \c t can
            ///be relaxed if some link in the fundamental cut
            ///belonging to \c t is chosen in the solution.
            ///
            ///In fact we don't even need to relax, since
            ///these constraints became trivial after
            ///rounding. So returning always false should be
            ///fine, too.
            template <typename LP>
            bool relaxCondition(const LP & lp, RowId row) {

                for (Edge e: m_coveredBy[m_rowId2Edge[row]])
                    {
                        if (m_Solution.count(e))
                            return true;
                    }
                return false;
            }

            ///Return the solution found
            template <typename UnUsed>
            SetEdge & getSolution(const UnUsed & ) {
                return m_Solution;
            }

            ///Return the solution found
            CostValue getSolutionValue() {
                return m_solCost;
            }


        private:


            typedef boost::filtered_graph<Graph, BoolMapToTreeFilter<TreeMap> > TreeGraph;

            typedef boost::filtered_graph<Graph, BoolMapToNonTreeFilter<TreeMap> > NonTreeGraph;

            
            typedef std::list< Edge > EdgeList;
            typedef std::map< Edge , EdgeList > CoverMap;

            //cross reference between links and column names
            typedef boost::bimap< Edge, ColId> EdgeToColId;

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
            void addVariables(LP & lp) {
                auto edges = boost::edges(m_ntree);
                int eIdx(0);
                int whatever=42;

                for(Edge e : utils::make_range(edges)) {
                    std::string colName = getEdgeName(eIdx);
                    ColId colIdx=lp.addColumn(m_costMap[e], LO, 0, whatever, colName);
                    auto tmp=m_edgeToColId.insert(typename EdgeToColId::value_type(e,colIdx));
                    assert(tmp.second);
                    ++eIdx;
                }
            }

            //adding the cut constraints
            //returns the number of rows added
            template <typename LP>
            void addCutConstraints(LP & lp) {
                int whatever=42;
                int dbIndex=0;
                BGL_FORALL_EDGES_T(ei,m_tree,TreeGraph){
                    std::string rowName = getRowName(dbIndex);
                    RowId rowIdx = lp.addRow(LO, 1, whatever, rowName );
                    m_rowId2Edge[rowIdx]=ei;

                    for (Edge pe:m_coveredBy[ei]){
                        lp.addConstraintCoef(rowIdx, m_edgeToColId.left.at(pe));

                    }
                    ++dbIndex;
                }
            }
            ///The input

            const Graph & m_g;
            const TreeMap & m_treeMap;
            const CostMap & m_costMap;
            EdgeToColId m_edgeToColId;
            ///Auxiliary data structures
            BoolMapToTreeFilter<TreeMap> m_inTreeFilter;
            //The spanning tree
            TreeGraph  m_tree;
            BoolMapToNonTreeFilter<TreeMap> m_nonTreeFilter;
            //The non-tree (=set of links)
            NonTreeGraph m_ntree;
            //Cost of the solution found
            CostValue m_solCost;
            //Structures for the "m_coveredBy" relations
            CoverMap m_coveredBy;
            //reference between tree edges and row names
            std::map<RowId, Edge> m_rowId2Edge;
            //Which links are chosen in the solution
            SetEdge m_Solution;
        };


        template <typename Graph, typename TreeMap, typename CostMap >
        TreeAug<Graph,  TreeMap,  CostMap >
        makeTreeAug(const Graph & _g, const TreeMap & _treeMap, const CostMap & _costMap) {

            return TreeAug< Graph,  TreeMap,  CostMap >(_g,  _treeMap,  _costMap);

        }

    } //ir
} //paal

#endif /* TREE_AUG_HPP */
