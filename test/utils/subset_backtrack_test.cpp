/**
 * @file subset_backtrack_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-08-14
 */

#include "paal/utils/algorithms/subset_backtrack.hpp"
#include "paal/utils/functors.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/algorithm/remove.hpp>

#include <vector>
#include <random>

BOOST_AUTO_TEST_CASE(subset_backtrack) {
    const int N = 10, WANT_SUM = (1 << (N - 1)) * (N * (N + 1) / 2);
    std::vector<int> numbers(N);
    boost::iota(numbers, 1);
    int sum = 0, selected_sum = 0;
    auto backtrack = paal::make_subset_backtrack(numbers);
    backtrack.solve([&](int k) {
                        selected_sum += k;
                        sum += selected_sum;
                        return true;
                    },
                    [&](int k) { selected_sum -= k; });
    BOOST_CHECK_EQUAL(selected_sum, 0);
    BOOST_CHECK_EQUAL(sum, WANT_SUM);
}

BOOST_AUTO_TEST_CASE(subset_backtrack_empty_set_solved_two_times){
    auto backtrack=paal::make_subset_backtrack(std::vector<int>{});
    auto f=[](int k){BOOST_CHECK(false);return true;};
    backtrack.solve(f,f);
    backtrack.solve(f,f);
}

BOOST_AUTO_TEST_CASE(subset_backtrack_on_element_set_solved_two_times){
    auto backtrack=paal::make_subset_backtrack(std::vector<int>{0});
    auto t=paal::utils::always_true{};
    backtrack.solve(t,t);
    backtrack.solve(t,t);
}

BOOST_AUTO_TEST_CASE(subset_backtrack_max_one_even) {
    const int N = 20;
    assert(N % 4 == 0);
    const int WANT_SUM =
        (1 << (N / 2))         // number of sets of odd numbers
        * (N / 2 + 1)          // one or 0 even numbers
        * (N * N / 8 + N / 2); // average sum of odd and even numbers
    std::vector<int> numbers(N);
    boost::iota(numbers, 1);
    int sum = 0, even = 0, selected_sum = 0;
    auto backtrack = paal::make_subset_backtrack(numbers);
    auto is_even = [](int k) { return k % 2 == 0; };
    backtrack.solve([&](int k) {
                        if (even + is_even(k) > 1) return false;
                        even += is_even(k);
                        selected_sum += k;
                        sum += selected_sum;
                        return true;
                    },
                    [&](int k) {
                        even -= is_even(k);
                        selected_sum -= k;
                    },
                    [&](boost::iterator_range<std::vector<int>::iterator>
                            remaining_nums) {
                        if (even == 1) return boost::remove(remaining_nums, 8);
                        std::shuffle(remaining_nums.begin(), remaining_nums.end(),
                                    std::default_random_engine());
                        return remaining_nums.end();
                    });
    BOOST_CHECK_EQUAL(even, 0);
    BOOST_CHECK_EQUAL(selected_sum, 0);
    BOOST_CHECK_EQUAL(sum, WANT_SUM);
}
