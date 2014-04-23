/**
 * @file simulated_annealing_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-02-04
 */

#include <boost/test/unit_test.hpp>

#include "paal/local_search/simulated_annealing.hpp"
#include "paal/local_search/custom_components.hpp"
#include "paal/data_structures/components/components_replace.hpp"

#include "utils/simple_single_local_search_components.hpp"
#include "utils/logger.hpp"


BOOST_AUTO_TEST_SUITE( simulated_annealing )

namespace ls = paal::local_search;
using namespace paal;

    BOOST_AUTO_TEST_CASE(simulated_annealing_gain_adaptorTest) {
        int currentSolution(0);
        int best(0);
        auto saGain = ls::make_simulated_annealing_gain_adaptor(Gain(),
                            ls::make_exponential_cooling_schema_dependant_on_time(std::chrono::seconds(1), 10, 0.1));

        auto recordSolutionCommit =
                ls::make_record_solution_commit_adapter(
                        best,
                        Commit(),
                        paal::utils::make_functor_to_comparator(f));

        ls::local_search_simple(currentSolution, ls::make_search_components(get_moves(), saGain, recordSolutionCommit));
        BOOST_CHECK_EQUAL(best, 6);
        LOGLN("solution " << best);
    }


BOOST_AUTO_TEST_SUITE_END()
