//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include "test_utils/sample_graph.hpp"
#include "test_utils/logger.hpp"

#include "paal/local_search/facility_location/facility_location.hpp"
#include "paal/local_search/custom_components.hpp"
#include "paal/utils/functors.hpp"

#include <boost/test/unit_test.hpp>

using namespace paal::local_search;

BOOST_AUTO_TEST_CASE(FacilityLocationTest) {

    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();
    std::vector<int> fcosts{ 7, 8 };
    auto cost = paal::utils::make_array_to_functor(fcosts);

    typedef paal::data_structures::voronoi<decltype(gm)> VorType;
    typedef paal::data_structures::facility_location_solution<decltype(cost),
                                                              VorType> Sol;
    typedef typename VorType::GeneratorsSet FSet;
    VorType voronoi(FSet{}, FSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E }, gm);
    Sol sol(std::move(voronoi), FSet{ SGM::A, SGM::B }, cost);
    default_remove_fl_components rem;
    default_add_fl_components add;
    default_swap_fl_components swap;
    paal::utils::always_true nop;
    // this search can and in one or 2 rounds depending on
    // implementation of unordered_set in facility_location_solution
    // if the first facility to add is A, then the search will be finished
    // in  one round. The 2 round are needed otherwise
    stop_condition_count_limit oneRoundSearch(2);

    ON_LOG(auto const &ch = sol.get_chosen_facilities());
    LOGLN("Solution before the first search");
    LOG_COPY_RANGE_DEL(ch, ",");
    LOGLN("");

    BOOST_CHECK(facility_location_local_search(
        sol, first_improving_strategy{}, nop,
        oneRoundSearch, rem, add, swap));

    LOGLN("Solution after the first search");
    LOG_COPY_RANGE_DEL(ch, ",");
    LOGLN("");
    BOOST_CHECK(!facility_location_first_improving(sol, rem, add, swap));
    //best improving strategy
    BOOST_CHECK(!facility_location_local_search(sol,
                best_improving_strategy{}, paal::utils::always_true{},
                paal::utils::always_false{}, rem, add, swap));
}
