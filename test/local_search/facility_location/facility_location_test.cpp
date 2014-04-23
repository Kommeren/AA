#include <boost/test/unit_test.hpp>
#include "paal/local_search/facility_location/facility_location.hpp"
#include "paal/local_search/custom_components.hpp"
#include "paal/utils/functors.hpp"
#include "utils/sample_graph.hpp"
#include "utils/logger.hpp"

using namespace paal::local_search::facility_location;


BOOST_AUTO_TEST_CASE(FacilityLocationTest) {

    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();
    std::vector<int> fcosts{7,8};
    auto cost = paal::utils::make_array_to_functor(fcosts);

    typedef paal::data_structures::voronoi<decltype(gm)> VorType;
    typedef paal::data_structures::facility_location_solution
        <decltype(cost), VorType> Sol;
    typedef typename VorType::GeneratorsSet FSet;
    VorType voronoi(FSet{}, FSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);
    Sol sol(std::move(voronoi),FSet{SGM::A, SGM::B}, cost );
    default_remove_fl_components::type rem;
    default_add_fl_components::type    add;
    default_swap_fl_components::type   swap;
    paal::utils::always_true nop;
    //this search can and in one or 2 rounds depending on
    //implementation of unordered_set in facility_location_solution
    //if the first facility to add is A, then the search will be finished
    //in  one round. The 2 round are needed otherwise
    paal::local_search::stop_condition_count_limit oneRoundSearch(2);

    ON_LOG(auto const & ch = sol.get_chosen_facilities());
    LOGLN("Solution before the first search");
    LOG_COPY_RANGE_DEL(ch, ",");
    LOGLN("");

    BOOST_CHECK(facility_location_local_search(
                    sol, paal::local_search::choose_first_better_strategy{}, nop, oneRoundSearch, rem, add, swap));

    LOGLN("Solution after the first search");
    LOG_COPY_RANGE_DEL(ch, ",");
    LOGLN("");
    BOOST_CHECK(!facility_location_local_search_simple(sol, rem, add ,swap));
}
