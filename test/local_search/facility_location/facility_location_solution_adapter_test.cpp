#include <boost/test/unit_test.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/sort.hpp>

#include "paal/data_structures/facility_location/facility_location_solution.hpp"
#include "paal/data_structures/voronoi/voronoi.hpp"
#include "paal/local_search/facility_location/facility_location_solution_adapter.hpp"

#include "utils/sample_graph.hpp"

using namespace paal::data_structures;
using namespace paal::local_search::facility_location;

BOOST_AUTO_TEST_CASE(FacilityLocationSolutionAdapterTest) {
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
    std::vector<int> fcosts{7,8};
    auto cost = [&](int i){ return fcosts[i];};

    typedef Voronoi<decltype(gm)> VorType;
    typedef typename VorType::GeneratorsSet FSet;
    VorType voronoi(FSet{}, FSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E} , gm);

    typedef FacilityLocationSolution<decltype(cost), VorType> Sol;

    Sol sol(std::move(voronoi), FSet{SGM::A,SGM::B}, cost);

    FacilityLocationSolutionAdapter<Sol> sa(sol);
    std::vector<Facility<int>> saSorted;
    std::copy(sa.begin(), sa.end(), std::back_inserter(saSorted));
    boost::sort(saSorted);
    auto b = saSorted.begin();
    auto e = saSorted.end();
    auto A = *b;
    BOOST_CHECK_EQUAL(*b, Facility<int>(UNCHOSEN, SGM::A));
    BOOST_CHECK(b != e);
    BOOST_CHECK_EQUAL(*(++b), Facility<int>(UNCHOSEN, SGM::B));
    auto B = *b;
    BOOST_CHECK(b != e);
    BOOST_CHECK(++b == e);

    sa.addFacility(A);

    saSorted.clear();
    std::copy(sa.begin(), sa.end(), std::back_inserter(saSorted));
    boost::sort(saSorted);

    b = saSorted.begin();
    e = saSorted.end();
    BOOST_CHECK_EQUAL(*b, A);
    BOOST_CHECK(b != e);
    BOOST_CHECK_EQUAL(*(++b), B);
    BOOST_CHECK(b != e);
    BOOST_CHECK(++b == e);
}
