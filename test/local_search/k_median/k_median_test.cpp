#define BOOST_TEST_MODULE k_median

#include <boost/test/unit_test.hpp>

#include "paal/local_search/k_median/k_median.hpp"
#include "utils/sample_graph.hpp"
#include "utils/logger.hpp"


BOOST_AUTO_TEST_CASE(test_1) {
    // sample data
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
    
    //define voronoi and solution
    const int k = 2;
    typedef paal::data_structures::Voronoi<decltype(gm)> VorType;
    typedef paal::local_search::k_median::KMedianSolution
        <k, VorType> Sol;
    typedef paal::data_structures::ObjectWithCopy<Sol> SolOcjWithCopy;
    typedef paal::data_structures::VoronoiTraits<VorType> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    typedef typename Sol::UnchosenFacilitiesSet USet;


    //create voronoi and solution
    VorType voronoi(GSet{SGM::B, SGM::D}, VSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);
    Sol sol(std::move(voronoi), USet{SGM::A, SGM::C});

    //create facility location local search step
    paal::local_search::k_median::KMedianLocalSearchStep<k, VorType>  
        ls(std::move(sol));

    //search 
    search(ls);

    //print result
    SolOcjWithCopy & s = ls.getSolution();
    auto const & ch = s->getChosenFacilities();
    LOG("Solution:");
    LOG_COPY_DEL(ch.begin(), ch.end(), ",");
}
