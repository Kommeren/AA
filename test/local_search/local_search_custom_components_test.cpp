/**
 * @file local_search_custom_components_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-02-04
 */

#include <boost/test/unit_test.hpp>


#include "paal/local_search/custom_components.hpp"
#include "paal/data_structures/components/components_replace.hpp"
#include "paal/data_structures/tabu_list/tabu_list.hpp"

#include "utils/simple_single_local_search_components.hpp"
#include "utils/logger.hpp"

BOOST_AUTO_TEST_SUITE( local_search_custom_components )

namespace ls = paal::local_search;
using namespace paal;

    BOOST_AUTO_TEST_CASE(ConditionalGainAdaptorTest) {
        int solution(0);

        auto condGain =  ls::make_ConditionalGainAdaptor(SearchComp().get<ls::Gain>(),
                [](int sol, int move){return sol + move <= 3;});

        ls::local_search_simple(solution, paal::data_structures::replace<ls::Gain>(condGain, SearchComp()));
        BOOST_CHECK_EQUAL(solution, 3);
        LOGLN("solution " << solution);
    }

    BOOST_AUTO_TEST_CASE(TabuGainAdaptorTest) {
        int currentSolution(0);
        int best(0);

        auto condGain =  ls::make_TabuGainAdaptor(
                data_structures::TabuListRememberSolutionAndMove<int, int>(20),
                utils::ReturnTrueFunctor(),
                [](int s, int m){return s + m <= 3. && s + m >= 0.;});

        auto recordSolutionCommit =
                ls::make_RecordSolutionCommitAdapter(
                        best,
                        Commit(),
                        paal::utils::make_FunctorToComparator(f, utils::Greater()));

        ls::local_search_simple(currentSolution, ls::make_SearchComponents(GetMoves(), condGain, recordSolutionCommit));

        BOOST_CHECK_EQUAL(best, 3);
        LOGLN("solution " << best);
    }


BOOST_AUTO_TEST_SUITE_END()
