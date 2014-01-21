/**
 * @file test_result_check.hpp
 * @brief
 * @author Piotr Smulewicz, Robert Rosolek
 * @version 1.0
 * @date 2014-01-29
 */
#ifndef TEST_RESULT_CHECK_HPP
#define TEST_RESULT_CHECK_HPP

#include <boost/test/unit_test.hpp>

#include "utils/logger.hpp"

#include "paal/utils/functors.hpp"

#include <iomanip>

template <typename T> auto to_double(T t) -> puretype(t + 0.0) { return t; }

template<
   typename Result,
   typename Bound,
   typename Ratio,
   typename Comparator=paal::utils::less_equal,
   typename Eps = double
>
void check_result_compare_to_bound(
   Result result,
   Bound bound_for_optimal,
   Ratio approximation_ratio,
   Comparator comp = Comparator{},
   Eps eps = 0,
   std::string bound = "Solution is not worse than: ",
   std::string got_ratio = "Approximation ratio is not better than: ",
   std::string want_ratio = "Want approximation: "
) {
   LOGLN("result: " << std::setprecision(20) << result);
   LOGLN(bound << bound_for_optimal);
   LOGLN(got_ratio << to_double(result) / bound_for_optimal);
   LOGLN(want_ratio << approximation_ratio);
   // +eps is for comp = less_equal() , -eps if for comp = greater_equal().
   BOOST_CHECK(comp(to_double(result) + eps, to_double(bound_for_optimal) * approximation_ratio) ||
         comp(to_double(result) - eps, to_double(bound_for_optimal) * approximation_ratio));
};

template<
   typename Result,
   typename Optimal,
   typename Ratio,
   typename Comparator=paal::utils::less_equal,
   typename Eps = double
>
void check_result(
   Result result,
   Optimal optimal,
   Ratio approximation_ratio,
   Comparator comp = Comparator{},
   Eps eps = 0,
   std::string opt = "Optimal: ",
   std::string got_ratio = "Got approximation ratio: ",
   std::string want_ratio = "Want approximation: "
) {
    check_result_compare_to_bound(result, optimal, approximation_ratio, comp, eps, opt, got_ratio, want_ratio);
    // +eps is for comp = less_equal() , -eps if for comp = greater_equal().
    BOOST_CHECK(comp(optimal, result + eps) || comp(optimal, result - eps));
};

#endif /* TEST_RESULT_CHECK_HPP */
