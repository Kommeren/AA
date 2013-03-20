#define BOOST_TEST_MODULE local_search_test

#include <boost/test/unit_test.hpp>

#define BOOST_RESULT_OF_USE_DECLTYPE

#include <iostream>

#include "paal/min_cost_max_flow/min_cost_max_flow.hpp"

#include "utils/sample_graph.hpp"

using namespace boost;

typedef adjacency_list_traits < vecS, vecS, directedS > Traits;

template <typename Graph, typename Weight, typename Capacity, typename Reversed, typename ResidualCapacity>
class EdgeAdder {
public:
    EdgeAdder(Graph & g, Weight & w, Capacity & c, Reversed & rev, ResidualCapacity & residualCapacity) 
        : m_g(g), m_w(w), m_cap(c), m_resCap(residualCapacity), m_rev(rev) {}
void addEdge(int v, int w, int weight, int capacity) {
        //bool b;
        Traits::edge_descriptor e,f;
        e = add(v, w, weight, capacity);
        f = add(w, v, -weight, 0);
        m_rev[e] = f; 
        m_rev[f] = e; 
    }
    /*void addEdgeSym(int v, int w, int weight, int capacity) {
        auto e = add(v, w, weight, capacity);
        auto f = add(w, v, weight, capacity);
        m_rev[e] = f; 
        m_rev[f] = e; 
    }*/
private:
    Traits::edge_descriptor add(int v, int w, int weight, int capacity) {
        bool b;
        Traits::edge_descriptor e;
        std::tie(e, b) = add_edge(v, w, m_g);
        assert(b);
        m_cap[e] = capacity;
        m_resCap[e] = capacity;
        m_w[e] = weight;
        return e;
    }
    Graph & m_g;
    Weight & m_w;
    Capacity & m_cap;
    ResidualCapacity & m_resCap;
    Reversed & m_rev;
};


BOOST_AUTO_TEST_CASE(cycle_canceling_test) {
/*    auto g = SampleGraphsMetrics::getGraphSmall();
    typedef decltype(g) Graph;*/
   typedef adjacency_list < listS, vecS, directedS,
                  property < vertex_name_t, std::string >,
                      property < edge_capacity_t, long,
                          property < edge_residual_capacity_t, long,
                              property < edge_reverse_t, Traits::edge_descriptor, 
                                property <edge_weight_t, long>
                                       > 
                                    > 
                                > > Graph;

    typedef Traits::vertex_descriptor vd;
    typedef Traits::edge_descriptor ed;
    const typename boost::graph_traits<Graph>::vertices_size_type N(6);
    typedef property_map < Graph, edge_capacity_t >::type Capacity;
    typedef property_map < Graph, edge_residual_capacity_t >::type ResidualCapacity;
    typedef property_map < Graph, edge_weight_t >::type Weight;
    typedef property_map < Graph, edge_reverse_t >::type Reversed;
    
    Graph g(N);
    Capacity  capacity = get(edge_capacity, g);
    Reversed rev = get(edge_reverse, g);
    ResidualCapacity residual_capacity = get(edge_residual_capacity, g);
    
    Weight weight = get(edge_weight, g);
    Weight weight2(weight);


    vd s(0), t(5);

    EdgeAdder<Graph, Weight, Capacity, Reversed, ResidualCapacity> 
        ea(g, weight, capacity, rev, residual_capacity);

    ea.addEdge(0, 1, 4 ,2);
    ea.addEdge(0, 2, 2 ,2);

    ea.addEdge(1, 3, 1 ,1);
    ea.addEdge(1, 4, 1 ,1);
    ea.addEdge(2, 3, 1 ,1);
    ea.addEdge(2, 4, 1 ,1);
/*    ea.addEdgeSym(1, 2, 1 ,1);
    ea.addEdgeSym(1, 3, 1 ,1);
    ea.addEdgeSym(2, 4, 1 ,1);
    ea.addEdgeSym(3, 4, 1 ,1);*/

    ea.addEdge(3, 5, 4 ,20);
    ea.addEdge(4, 5, 2 ,20);

    std::vector<int> pred(N);
    std::vector<int> distance(N, (std::numeric_limits<int>::max)());
    distance[s] = 0;
    
    assert(edge(0,1, g).second);
    assert(edge(0,2, g).second);
    
    std::cout << residual_capacity[edge(0,1, g).first] << std::endl;
    std::cout << residual_capacity[edge(0,2, g).first] << std::endl;
    std::cout << residual_capacity[edge(1,0, g).first] << std::endl;
    std::cout << residual_capacity[edge(2,0, g).first] << std::endl;

    edmonds_karp_max_flow(g, s, t);
    auto gRes = detail::residual_graph(g, residual_capacity);
//    path_augmentation_from_residual(gRes, s, t);
    cycle_cancelation_from_residual(gRes);
    std::iota(pred.begin(), pred.end(), 0);

    std::cout << residual_capacity[edge(0,1, g).first] << std::endl;
    std::cout << residual_capacity[edge(0,2, g).first] << std::endl;
    std::cout << residual_capacity[edge(1,0, g).first] << std::endl;
    std::cout << residual_capacity[edge(2,0, g).first] << std::endl;

    //std::cout << cost << std::endl;
}
