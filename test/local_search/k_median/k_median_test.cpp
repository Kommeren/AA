//=======================================================================
// Copyright (c) 2014 Piotr Smulewicz
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include "test_utils/sample_graph.hpp"
#include "test_utils/logger.hpp"

#include "paal/local_search/k_median/k_median.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(kmedian_test) {
    // sample data
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();

    // define voronoi and solution
    const int k = 2;
    typedef paal::data_structures::voronoi<decltype(gm)> VorType;
    typedef paal::data_structures::k_median_solution<VorType> Sol;
    typedef paal::data_structures::voronoi_traits<VorType> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    typedef typename Sol::UnchosenFacilitiesSet USet;

    // create voronoi and solution
    VorType voronoi(GSet{ SGM::B, SGM::D },
                    VSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E }, gm);
    Sol sol(std::move(voronoi), USet{ SGM::A, SGM::C }, k);

    // create facility location local search components
    paal::local_search::default_k_median_components swap;

    // search
    paal::local_search::facility_location_first_improving(sol, swap);

    // print result
    ON_LOG(auto const &ch = ) sol.get_chosen_facilities();
    LOGLN("Solution:");
    LOG_COPY_RANGE_DEL(ch, ",");
}
