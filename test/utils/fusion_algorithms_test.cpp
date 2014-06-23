/**
 * @file fusion_alogrithms_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include "paal/utils/fusion_algorithms.hpp"
#include "paal/utils/functors.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/fusion/include/vector.hpp>

namespace ds = paal::data_structures;

struct Sum {
    template <typename Number, typename AccumulatorFun,
              typename AccumulatorData, typename Callback>
    auto operator()(Number n, AccumulatorFun aFun, AccumulatorData aData,
                    Callback call) const->decltype(call(aFun, aData + n)) {
        return call(aFun, aData + n);
    }
};

struct strange_weighted_sum {

    template <typename AccumulatorFun, typename AccumulatorData,
              typename Callback>
    auto operator()(int n, AccumulatorFun aFun, AccumulatorData aData,
                    Callback call) const->decltype(call(aFun, aData + n)) {
        return call(aFun, aData + n);
    }

    template <typename AccumulatorFun, typename AccumulatorData,
              typename Callback>
    auto operator()(float n, AccumulatorFun aFun, AccumulatorData aData,
                    Callback call) const->decltype(call(aFun, aData + n / 2)) {
        return call(aFun, aData + n / 2);
    }

    template <typename AccumulatorFun, typename AccumulatorData,
              typename Callback>
    auto operator()(long long n, AccumulatorFun aFun, AccumulatorData aData,
                    Callback call) const->decltype(call(aFun, aData + n / 3)) {
        return call(aFun, aData + n / 3);
    }
};

struct Find12 {
    template <typename Number> auto operator()(Number n) const->bool {
        return n == 12;
    }
};

struct Find10 {
    template <typename Number> auto operator()(Number n) const->bool {
        return n == 10;
    }
};

BOOST_AUTO_TEST_CASE(polymorfic_fold) {
    boost::fusion::vector<int, float, long long> v(12, 5.5f, 1ll);

    ds::polymorfic_fold fold{};
    paal::utils::identity_functor id{};
    BOOST_CHECK_EQUAL(12 + 5.5f + 1ll, fold(Sum{}, id, 0, v));
    BOOST_CHECK_EQUAL(12 + 5.5f / 2 + 1ll / 3,
                      fold(strange_weighted_sum{}, id, 0, v));

    ds::Satisfy satisfy{};
    BOOST_CHECK(satisfy(Find12{}, v));
    BOOST_CHECK(!satisfy(Find10{}, v));
}
