/**
 * @file tree_augmentation.hpp
 * @brief 
 * @author Attila Bernath, Piotr Smulewicz, Piotr Wygocki 
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


        struct TARoundCondition {
            ///Round a coordinate up if it is at least half in the
            ///current solution.
            template <typename Solution, typename LP>
            boost::optional<double> operator()(Solution & sol, LP & lp, ColId col) {
                ///Round a coordinate up if it is at least half.
                auto res = m_roundHalf(sol, lp, col);
                //Save some auxiliary info
                if(res) {
                    sol.addToSolution(col);
                }
                return res;
            }
        private:
            RoundConditionGreaterThanHalf m_roundHalf;
        };
            
            
        struct TARelaxCondition {
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
            template <typename Solution, typename LP>
            bool operator()(Solution & sol, const LP & lp, RowId row) {
                for (auto e: sol.getCoveredBy(sol.rowToEdge(row))) {
                    if (sol.isInSolution(e))
                        return true;
                }
                return false;
            }
        };
            
        class TAInit {
        public:
            ///Initialize the cut LP.
            template <typename Solution, typename LP>
            void operator()(Solution & sol, LP & lp) {    
                sol.init();
                lp.setLPName("Tree augmentation");
                lp.setMinObjFun(); 

                addVariables(sol, lp);

                addCutConstraints(sol, lp);

                lp.loadMatrix();
            }
        private:
            //adding variables
            //returns the number of variables
            template <typename Solution, typename LP>
            void addVariables(Solution & sol,LP & lp) {
                auto edges = boost::edges(sol.getLinksGraph());
                int eIdx(0);
                int whatever=-1;

                for(auto e : utils::make_range(edges)) {
                    std::string colName = getEdgeName(eIdx);
                    ColId colIdx=lp.addColumn(sol. getCost(e), LO, 0, whatever, colName);
                    sol.bindEdgeWithCol(e, colIdx);
                    ++eIdx;
                }
            }

            //adding the cut constraints
            //returns the number of rows added
            template <typename Solution, typename LP>
            void addCutConstraints(Solution & sol,LP & lp) {
                int whatever=-1;
                int dbIndex=0;
                auto const & tree = sol.getTreeGraph();
                BGL_FORALL_EDGES_T(e, tree, puretype(tree)){
                    std::string rowName = getRowName(dbIndex);
                    RowId rowIdx = lp.addRow(LO, 1, whatever, rowName );
                    sol.bindEdgeWithRow(e, rowIdx);

                    for (auto pe : sol.getCoveredBy(e)){
                        lp.addConstraintCoef(rowIdx, sol.edgeToCol(pe));
                    }
                    ++dbIndex;
                }
            }
            
            std::string getEdgeName(int eIdx) const {
                return std::to_string(eIdx);
            }

            std::string getCutConstrPrefix() const {
                return "cutConstraint";
            }

            std::string getRowName(int dbIdx) const {
                return getCutConstrPrefix() + std::to_string(dbIdx);
            }
        };

        template <
            typename SolveLPToExtremePoint = DefaultSolveLPToExtremePoint, 
            typename RoundCondition = TARoundCondition, 
            typename RelaxContition = TARelaxCondition, 
            typename Init = TAInit,
            typename SetSolution = utils::DoNothingFunctor>
                using  TAComponents = IRComponents<SolveLPToExtremePoint, RoundCondition, RelaxContition, Init, SetSolution>;

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
        template <typename Graph, typename TreeMap, typename CostMap, typename SetEdge >
        class TreeAug {
        public:

            typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
            typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
            typedef double CostValue;

            typedef std::map< Edge, bool > EdgeBoolMap;
            
            typedef boost::filtered_graph<Graph, BoolMapToTreeFilter<TreeMap> > TreeGraph;
            typedef boost::filtered_graph<Graph, BoolMapToNonTreeFilter<TreeMap> > NonTreeGraph;

            typedef std::vector< Edge > EdgeList;
            typedef std::map< Edge , EdgeList > CoverMap;

            //cross reference between links and column names
            typedef boost::bimap< Edge, ColId> EdgeToColId;
            typedef std::map<RowId, Edge> RowId2Edge;

            ///Constructor.
            ///
            ///@param _g  the graph to work with
            ///@param _treeMap designate a spanning tree in \c _g
            ///@param _costMap costs of the links (=non-tree edges). The costs assigned to tree edges are not used.
            ///@param _resultSetEdge Result set of edges
            TreeAug(const Graph & _g, const TreeMap & _treeMap, const CostMap & _costMap, SetEdge & _resultSetEdge) :
                m_g(_g),m_treeMap(_treeMap),m_costMap(_costMap),
                m_Solution(_resultSetEdge),
                m_tree(m_g, BoolMapToTreeFilter<TreeMap>(m_treeMap)),
                m_ntree(m_g, BoolMapToNonTreeFilter<TreeMap>(m_treeMap)),
                m_solCost(0)
            { }//end of Constructor

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

            const NonTreeGraph & getLinksGraph() const {
                return m_ntree;
            }
            
            const TreeGraph & getTreeGraph() const {
                return m_tree;
            }
            
            decltype(boost::get(std::declval<CostMap>(), std::declval<Edge>()))
            getCost(Edge e) {
                return boost::get(m_costMap, e);
            }

            void addToSolution(ColId col) {
                auto tmp=m_Solution.insert(m_edgeToColId.right.at(col));
                assert(tmp.second);
                m_solCost+=m_costMap[m_edgeToColId.right.at(col)];
            }
                    
            void bindEdgeWithCol(Edge e, ColId col) {
                auto tmp=m_edgeToColId.insert(typename EdgeToColId::value_type(e, col));
                assert(tmp.second);
            }
            
            void bindEdgeWithRow(Edge e, RowId row) {
                auto tmp=m_rowId2Edge.insert(typename RowId2Edge::value_type(row, e));
                assert(tmp.second);
            }

            void init() {    
                //We need to fill very useful auxiliary data structures:
                //\c m_coveredBy - containing lists of
                //edges. For a tree edge \c t the list \c m_coveredBy[t]
                //contains the list of links covering \c t.
                
                //First we erase the previous contents
                BGL_FORALL_EDGES_T(ei,m_tree,TreeGraph){
                    m_coveredBy[ei].clear();
                }
                
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

            Edge rowToEdge(RowId row) const {
                return m_rowId2Edge.at(row);
            }

            ColId edgeToCol(Edge e) const {
                return m_edgeToColId.left.at(e);
            }

            EdgeList & getCoveredBy(Edge e) {
                return m_coveredBy[e];
            }
            
            bool isInSolution(Edge e) const {
                return m_Solution.count(e);
            }

            ///Return the solution found
            CostValue getSolutionValue() const {
                return m_solCost;
            }


        private:

            ///The input
            const Graph & m_g;
            const TreeMap & m_treeMap;
            const CostMap & m_costMap;
            
            //Which links are chosen in the solution
            SetEdge & m_Solution;
            
            ///Auxiliary data structures
            EdgeToColId m_edgeToColId;
            
            //The spanning tree
            TreeGraph  m_tree;
            //The non-tree (=set of links)
            NonTreeGraph m_ntree;
            //Cost of the solution found
            CostValue m_solCost;
            //Structures for the "m_coveredBy" relations
            CoverMap m_coveredBy;
            //reference between tree edges and row names
            RowId2Edge m_rowId2Edge;
        };


        template <typename Graph, typename TreeMap, typename CostMap, typename EdgeSet >
        TreeAug<Graph,  TreeMap,  CostMap, EdgeSet >
        makeTreeAug(const Graph & _g, const TreeMap & _treeMap, const CostMap & _costMap, EdgeSet & solution) {
            return TreeAug< Graph,  TreeMap,  CostMap, EdgeSet >(_g,  _treeMap,  _costMap, solution);

        }
        
        template <typename Graph, typename TreeMap, 
                  typename CostMap, typename EdgeSet, 
                  typename IRComponents, typename Visitor = TrivialVisitor>
        void tree_augmentation_iterative_rounding(
                const Graph & _g, 
                const TreeMap & _treeMap, 
                const CostMap & _costMap, 
                EdgeSet & _resultSetEdge, 
                IRComponents comps, 
                Visitor vis = Visitor()) {
            paal::ir::TreeAug<Graph, TreeMap, CostMap, EdgeSet> treeaug(_g, _treeMap, _costMap, _resultSetEdge);
            solve_iterative_rounding(treeaug, std::move(comps), std::move(vis));
        }



    } //ir
} //paal

#endif /* TREE_AUG_HPP */
