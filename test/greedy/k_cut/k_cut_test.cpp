/**
 * @file k_cut_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-20
 */

#include "utils/logger.hpp"
#include "utils/test_result_check.hpp"

#include "paal/greedy/k_cut/k_cut.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/adjacency_list.hpp>


BOOST_AUTO_TEST_CASE(KCut) {
    std::vector<std::pair<int,int> > edgesP{{1,2},{1,5},{2,3},{2,5},{2,6},{3,4},{3,7},{4,7},{4,0},{5,6},{6,7},{7,0}};
    std::vector<int> costs{2,3,3,2,2,4,2,2,2,3,1,3};
    boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS,
                    boost::no_property,
                    boost::property<boost::edge_index_t,std::size_t>
                    >graph(8);
    for(std::size_t i=0;i<edgesP.size();i++){
        add_edge(edgesP[i].first,edgesP[i].second,i,graph);
    }

    auto edgeId = get(boost::edge_index, graph);

    std::vector<int> optimal={4,9,13,16,21,25,29};

    auto weight=make_iterator_property_map(costs.begin(), edgeId);

    for(auto i:boost::irange(2,9)){
        std::vector<std::pair<int,int> > verticesParts;
        int costCut=paal::greedy::kCut(graph,i,back_inserter(verticesParts),boost::weight_map(weight));
        LOGLN("cost cut: "<<costCut);
        std::vector<int> verticesToParts;
        verticesToParts.resize(verticesParts.size());
        for(auto i:verticesParts){
            LOG(i.first<<"("<<i.second<<"), ");
            verticesToParts[i.first]=i.second;
        }
        LOGLN("");

        int costCutVerification=0;
        for(auto i:boost::make_iterator_range(edges(graph))){
            if(verticesToParts[source(i,graph)]!=verticesToParts[target(i,graph)])
                costCutVerification+=weight[i];
        }
        BOOST_CHECK_EQUAL(costCut,costCutVerification);
        check_result(costCutVerification,optimal[i-2],2.0-2.0/double(i));
    }

}
