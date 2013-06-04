/**
 * @file bounded_degree_mst.hpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-03
 */
#ifndef BOUNDED_DEGREE_MST_HPP
#define BOUNDED_DEGREE_MST_HPP 

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/ir_components.hpp"


namespace paal {
namespace ir {

template <typename Graph, typename CostMap, typename DegreeBoundMap>
class BoundedDegreeMST : public IRComponents<> {
public:
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef std::map<Edge, bool> SpanningTree;
  
    BoundedDegreeMST(const Graph & g, const CostMap & costMap, const DegreeBoundMap & degBoundMap) :
              m_g(g), m_costMap(costMap), m_degBoundMap(degBoundMap) {}


    template <typename LP>
    void init(LP & lp) {
        lp.setLPName("bounded degree minimum spanning tree");
        lp.setMinObjFun(); 
        lp.loadMatrix();
    }
    
    SpanningTree & getSolution() {
        return m_spanningTree;
    }

private:

    const Graph & m_g;
    const CostMap & m_costMap;
    const DegreeBoundMap & m_degBoundMap;
    SpanningTree m_spanningTree;
};

template <typename Graph, typename CostMap, typename DegreeBoundMap>
BoundedDegreeMST<Graph, CostMap, DegreeBoundMap>
make_BoundedDegreeMST(const Graph & g, const CostMap & costMap, const DegreeBoundMap & degBoundMap) {
    return BoundedDegreeMST<Graph, CostMap, DegreeBoundMap>(g, costMap, degBoundMap);
}

} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_HPP */
