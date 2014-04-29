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

    BOOST_AUTO_TEST_CASE(conditional_gain_adaptor_test) {
        int solution(0);

        auto condGain =  ls::make_conditional_gain_adaptor(search_comps().get<ls::Gain>(),
                [](int sol, int move){return sol + move <= 3;});

        ls::first_improving(solution, paal::data_structures::replace<ls::Gain>(condGain, search_comps()));
        BOOST_CHECK_EQUAL(solution, 3);
        LOGLN("solution " << solution);
    }

    BOOST_AUTO_TEST_CASE(tabu_gain_adaptor_test) {
        int currentSolution(0);
        int best(0);

        auto condGain =  ls::make_tabu_gain_adaptor(
                data_structures::tabu_list_remember_solution_and_move<int, int>(20),
                utils::always_true(),
                [](int s, int m){ return s + m <= 3. && s + m >= 0.;});

        auto recordSolutionCommit =
                ls::make_record_solution_commit_adapter(
                        best,
                        commit(),
                        paal::utils::make_functor_to_comparator(f));

        ls::first_improving(currentSolution,
                ls::make_search_components(get_moves(), condGain, recordSolutionCommit));

        BOOST_CHECK_EQUAL(best, 3);
        LOGLN("solution " << best);
    }


BOOST_AUTO_TEST_SUITE_END()
