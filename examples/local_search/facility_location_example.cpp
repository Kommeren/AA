/**
 * @file facility_location_example.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include "test/utils/sample_graph.hpp"

#include "paal/local_search/facility_location/facility_location.hpp"
#include "paal/utils/functors.hpp"

#include <iostream>

using namespace paal::local_search::facility_location;

int main() {
    //! [FL Search Example]
    // sample data
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();
    std::vector<int> fcosts{ 7, 8 };
    typedef paal::utils::array_to_functor<std::vector<int>> Cost;

    // define voronoi and solution
    typedef paal::data_structures::voronoi<decltype(gm)> VorType;
    typedef paal::data_structures::facility_location_solution<Cost, VorType>
        Sol;
    typedef paal::data_structures::voronoi_traits<VorType> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    typedef typename Sol::UnchosenFacilitiesSet USet;

    // create voronoi and solution
    VorType voronoi(GSet{}, VSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E }, gm);
    Sol sol(std::move(voronoi), USet{ SGM::A, SGM::B },
            paal::utils::make_array_to_functor(fcosts));

    // create facility location local search components
    default_remove_fl_components::type rem;
    default_add_fl_components::type add;
    default_swap_fl_components::type swap;

    // search
    facility_location_first_improving(sol, rem, add, swap);

    // print result
    auto const &ch = sol.get_chosen_facilities();
    std::copy(ch.begin(), ch.end(), std::ostream_iterator<int>(std::cout, ","));
    std::cout << std::endl;
    //! [FL Search Example]

    return true;
}
