//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file simulated_annealing_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-02-04
 */

#include "test_utils/simple_single_local_search_components.hpp"
#include "test_utils/logger.hpp"

#include "paal/local_search/simulated_annealing.hpp"
#include "paal/local_search/custom_components.hpp"
#include "paal/data_structures/components/components_replace.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(simulated_annealing)

namespace ls = paal::local_search;
using namespace paal;

BOOST_AUTO_TEST_CASE(simulated_annealing_gain_adaptor_test) {
    int currentSolution{};
    int best{};
    auto sa_gain = ls::make_simulated_annealing_gain_adaptor(
        gain{}, ls::make_exponential_cooling_schema_dependant_on_time(
                    std::chrono::seconds(1), 10, 0.1));

    auto record_solution_commit = ls::make_record_solution_commit_adapter(
        best, commit(), paal::utils::make_functor_to_comparator(f));

    ls::first_improving(currentSolution,
                        ls::make_search_components(get_moves{}, sa_gain,
                                                   record_solution_commit));
    BOOST_CHECK_EQUAL(best, 6);
    LOGLN("solution " << best);
}

BOOST_AUTO_TEST_CASE(simulated_annealing_commit_adaptor_test) {
    int currentSolution{};
    int best{};
    auto sa_commit = ls::make_simulated_annealing_commit_adaptor(
        commit{}, gain{},
        ls::exponential_cooling_schema_dependant_on_iteration(1e3, 1-1e-3));

    auto record_solution_commit = ls::make_record_solution_commit_adapter(
        best, std::move(sa_commit), paal::utils::make_functor_to_comparator(f));

    ls::stop_condition_count_limit stop_cond_count(10);
    auto stop_cond = [&](int) { return !stop_cond_count(); };

    ls::local_search(
        currentSolution, ls::best_strategy{}, stop_cond, stop_cond,
        ls::make_search_components(get_moves{}, gain{},
                                   std::move(record_solution_commit)));
    BOOST_CHECK_EQUAL(best, 6);
    LOGLN("solution " << best);
}

BOOST_AUTO_TEST_CASE(start_temperature_test) {
    double t;

    int solution{};
    auto cooling = [&]() { return t; };
    auto set_temperature = [&](double _t) { t = _t; };
    auto sa_gain = ls::make_simulated_annealing_gain_adaptor(gain{}, cooling);

    auto get_success_rate = [&](double temp, int repeats_number = 1e3) {
        set_temperature(temp);
        get_moves gm{};

        int number_of_success{};
        int total_moves{};
        for (int i = 0; i < repeats_number; ++i) {
            for (auto move : gm(solution)) {
                ++total_moves;
                if (paal::local_search::detail::positive_delta(sa_gain(solution, move))) {
                    ++number_of_success;
                }
            }
        }
        return double(number_of_success) / total_moves;
    };

    // 0.5 success_rate is guarantied for this model
    auto temp =
        ls::start_temperature(solution, sa_gain, get_moves{}, set_temperature);
    BOOST_CHECK_EQUAL(temp, 0.);

    for (auto ratio : { 0.6, 0.7, 0.8, 0.9, 0.95, 0.99 }) {
        int repeat = 1e3;
        temp = ls::start_temperature(solution, sa_gain, get_moves{},
                                     set_temperature, ratio, repeat);
        auto succ_rate = get_success_rate(temp, repeat);
        BOOST_CHECK(std::abs(succ_rate - ratio) < 0.1);
        LOGLN("temp = " << temp << "; success rate = " << succ_rate
                        << "; expected = " << ratio
                        << "; error = " << std::abs(succ_rate - ratio));
    }
}

BOOST_AUTO_TEST_SUITE_END()
