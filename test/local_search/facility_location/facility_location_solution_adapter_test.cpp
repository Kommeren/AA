#include "utils/sample_graph.hpp"

#include "paal/data_structures/facility_location/facility_location_solution.hpp"
#include "paal/data_structures/voronoi/voronoi.hpp"
#include "paal/local_search/facility_location/facility_location_solution_adapter.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/sort.hpp>


using namespace paal::data_structures;
using namespace paal::local_search::facility_location;

BOOST_AUTO_TEST_CASE(facility_location_solution_adapterTest) {
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();
    std::vector<int> fcosts{7,8};
    auto cost = [&](int i){ return fcosts[i];};

    typedef voronoi<decltype(gm)> VorType;
    typedef typename VorType::GeneratorsSet FSet;
    VorType voronoi(FSet{}, FSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E} , gm);

    typedef facility_location_solution<decltype(cost), VorType> Sol;

    Sol sol(std::move(voronoi), FSet{SGM::A,SGM::B}, cost);

    facility_location_solution_adapter<Sol> sa(sol);
    std::vector<int> saSorted;

    BOOST_CHECK_EQUAL(boost::distance(sa.getChosenCopy()), 0);

    boost::copy(sa.getUnchosenCopy(), std::back_inserter(saSorted));
    boost::sort(saSorted);
    auto b = saSorted.begin();
    auto e = saSorted.end();
    auto A = *b;
    BOOST_CHECK_EQUAL(*b, SGM::A);
    BOOST_CHECK(b != e);
    BOOST_CHECK_EQUAL(*(++b), SGM::B);
    auto B = *b;
    BOOST_CHECK(b != e);
    BOOST_CHECK(++b == e);

    sa.add_facility(A);
    BOOST_CHECK_EQUAL(boost::distance(sa.getChosenCopy()), 1);
    BOOST_CHECK_EQUAL(boost::distance(sa.getUnchosenCopy()), 1);
    BOOST_CHECK_EQUAL(*std::begin(sa.getChosenCopy()), A);
    BOOST_CHECK_EQUAL(*std::begin(sa.getUnchosenCopy()), B);
}
