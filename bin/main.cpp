/*
 *  * main.cpp
 *  *
 *  *  Created on: 03-01-2013
 *  *      Author: Piotr Wygocki
 *  */

#include <iostream>
#include <iterator>
#include <algorithm>

#include <boost/iterator/transform_iterator.hpp>

#include "paal/local_search/2_local_search/2_local_search.hpp"
#include "paal/data_structures/metric/graph_metrics.hpp"
#include "paal/data_structures/facility_location/facility_location_solution.hpp"
#include "paal/data_structures/cycle/simple_cycle.hpp"
#include "paal/data_structures/cycle/cycle_algo.hpp"
#include "paal/local_search/facility_location/facility_location_solution_adapter.hpp"
#include "paal/local_search/facility_location/facility_location_neighbor_getter.hpp"
#include "paal/utils/type_functions.hpp"

std::ostream& operator<< (std::ostream &o, const std::pair<int, int> &p) {
    return o << p.first << ',' << p.second;
}

using namespace paal;
using namespace paal::local_search;
using namespace paal::local_search::facility_location;
using namespace paal::local_search::two_local_search;
using namespace paal::data_structures;
using namespace paal::utils;
using namespace boost;

struct XXX {
    bool operator()(const std::pair<int, int> & p) const {
        return p != std::pair<int, int>(4,5);
    }
};

void fn(std::vector<int> v,  std::pair<int, int> i) {
    typedef std::pair<int, int> SolutionElement;
    typedef data_structures::SimpleCycle<int> CM;
    CM cm(v.begin(), v.end());

    typename  CM::EdgeIterator  begin, end;
    std::cout << typeid(*begin).name() << std::endl;
    tie(begin, end) = cm.getEdgeRange();
    begin != end;

    XXX bi;
    boost::filter_iterator<XXX, typename  CM::EdgeIterator >   ou(bi, begin, end ) ;
    *ou;
}

int main() {

    std::vector<int> v = {1,2,3,4,5, 6, 7, 8, 9, 10};
    auto vExl = TrivialNeigborGetter()(v, 4);
    vExl.first != vExl.second;
    fn(v, std::make_pair(4, 5));
    std::cout << (vExl.first == vExl.second) << std::endl << std::flush;
    std::cout << "t1" << std::endl << std::flush;
    std::copy(vExl.first, vExl.second, std::ostream_iterator<int>(std::cout, "\n"));

    SimpleCycle<int> cm(vExl.first, vExl.second);
    auto r = cm.getEdgeRange(2);
    for(;r.first != r.second; ++r.first) {
        std::cout << *r.first << std::endl;
    }
    std::cout << "xxx" << std::endl;

    typedef std::pair<int, int> PT;
    cm.flip(2, 7);

    r = cm.getEdgeRange(2);


    for(;r.first != r.second; ++r.first) {
        std::cout << *r.first << std::endl;
    }
    std::cout << "xxx" << std::endl;


    typedef adjacency_list <vecS, vecS, undirectedS, no_property, property < edge_weight_t, int > > graph_t;
    typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
    typedef graph_traits < graph_t >::edge_descriptor edge_descriptor;
    typedef std::pair<int, int> Edge;


    const int num_nodes = 5;
    enum nodes { A, B, C, D, E };
    Edge edge_array[] = { Edge(A, C), Edge(B, B), Edge(B, D), Edge(B, E),
        Edge(C, B), Edge(C, D), Edge(D, E), Edge(E, A), Edge(E, B)
    };
    int weights[] = { 1, 2, 1, 2, 7, 3, 1, 1, 1 };
    int num_arcs = sizeof(edge_array) / sizeof(Edge);

    graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);
    //    property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);

    typedef GraphMetric<graph_t, int> GraphMT;
    GraphMT gm(g);

    std::cout << "odl: " << gm(A,B) << std::endl;
    std::cout << "odl: " << gm(C,B) << std::endl;

    std::vector<int> ver = {A, B, C, D, E};

    std::random_shuffle(ver.begin(), ver.end());
    std::random_shuffle(ver.begin(), ver.end());
    std::copy(ver.begin(), ver.end(), std::ostream_iterator<int>(std::cout, ","));
    std::cout << std::endl;
    Gain2Opt<GraphMT> checker(gm);
    auto transformChosen = [](int v){ return std::make_pair(v,1);};
    auto transformUnchosen = [](int v){ return std::make_pair(v,2);};
    auto transformChosen2 = [](int v){ return std::make_pair(v,1);};
    std::cout<< typeid(transformUnchosen).name() << std::endl;
    std::cout<< typeid(transformChosen).name() << std::endl;
    std::cout<< typeid(transformChosen2).name() << std::endl;

    typedef typename SolToElem<const std::set<int>>::type t;
    std::set<int> set{7,8,9,1};
    typedef typename IterToElem<decltype(set.begin())>::type T;
    typedef typename IterToElem<decltype(std::declval<std::set<int>>().begin())>::type type; 

//    static char f(check<R (C::*)(), &C::begin>*);
    std::cout << typeid(&std::set<int>::begin).name() << std::endl;
    typedef decltype(std::declval<std::set<int>>().begin()) Ret;
    Ret(std::set<int>::*f)() const;
    std::cout << typeid(f).name() << std::endl;

//    std::cout << "HAS_MEMBER" << HAS_MEMBER(std::set<int>, begin) << std::endl;
    std::cout  << "HAS_MEMBER" <<  "bu" <<std::endl;

    typename kTuple<int, 10>::type ktup;
     

}
