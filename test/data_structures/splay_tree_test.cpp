//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include "paal/data_structures/splay_tree.hpp"

#include <boost/test/unit_test.hpp>

#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/adaptor/sliced.hpp>
#include <boost/range/adaptor/indexed.hpp>


#include <algorithm>
#include <array>
#include <climits>
#include <utility>
#include <vector>
#include <random>


using paal::data_structures::splay_tree;


template <typename T, typename I> bool check_content(T &t, I const & input) {
    if (!boost::equal(input, boost::make_iterator_range(t.begin(), t.end()))) {
        return false;
    }

    if (!boost::equal(input | boost::adaptors::reversed,
                      boost::make_iterator_range(t.rbegin(), t.rend()))) {
        return false;
    }

    //checking operator[]
    for (auto v : input | boost::adaptors::indexed()) {
        if (v.value() != t[v.index()]) {
            return false;
        }
    }
    return true;
}

template <typename T, typename I> bool check_content(T &t, I begin, I end) {
    return check_content(t, boost::make_iterator_range(begin, end));
}


template <typename T, typename I>
bool check_random_splay(T &t, I const & input, const std::size_t k_m) {
    const std::size_t k_n = boost::size(input);
    for (std::size_t i = 0; i < k_m; i++) {
        int el = rand() % k_n; // NOLINT
        if (input[el] != *(t.splay(el))) {
            return false;
        }
    }
    if (!check_content(t, input)) {
        return false;
    }
    return true;
}

template <typename T, typename I> bool check_random_splay(T &t, I begin, I end, const std::size_t k_m) {
    return check_random_splay(t, boost::make_iterator_range(begin, end), k_m);
}

BOOST_AUTO_TEST_SUITE(splay_treeTest)

BOOST_AUTO_TEST_CASE(splay_treeCreateEmpty) {
    splay_tree<int> t;
    BOOST_CHECK_EQUAL(std::size_t(0), t.size());
    BOOST_CHECK(t.empty());
    std::vector<int> input(0);
    BOOST_CHECK(check_content(t, input));
}

BOOST_AUTO_TEST_CASE(splay_tree_CreateFromRange) {
    const int k_n = 101;
    std::array<int, k_n> input;
    boost::iota(input, 0);
    splay_tree<int> t(input.begin(), input.end());
    splay_tree<int> t1(input);
    BOOST_CHECK(check_content(t, input));
    BOOST_CHECK(check_content(t1, input));
}

BOOST_AUTO_TEST_CASE(splay_tree_Find) {
    const int k_n = 1001;
    std::array<int, k_n> input;
    boost::iota(input, 0);
    splay_tree<int> t(input);
    BOOST_CHECK(check_content(t, input));
    BOOST_CHECK_EQUAL_COLLECTIONS(input.begin(), input.end(),
        t.begin(), t.end());
}

BOOST_AUTO_TEST_CASE(splay_tree_splay) {
    srand(123433);
    const int k_n = 131, k_m = 1701;
    std::array<int, k_n> input;
    boost::iota(input, 0);
    splay_tree<int> t(input);
    BOOST_CHECK(check_content(t, input));
    BOOST_CHECK(check_random_splay(t, input, k_m));
}

BOOST_AUTO_TEST_CASE(splay_tree_split_higher) {
    srand(123433);
    const int k_n = 10435, kL = 47;
    std::array<int, k_n> input;
    boost::iota(input, 0);
    splay_tree<int> t(input);
    BOOST_CHECK(check_content(t, input));
    const int k1 = k_n / 2, k2 = k_n / 4, k3 = k_n / 8;
    splay_tree<int> t1 = t.split_higher(k1);
    auto b = input.begin();
    BOOST_CHECK(check_random_splay(t, b, b + k1 + 1, kL));
    BOOST_CHECK(check_random_splay(t1, b + k1 + 1, b + k_n, kL));
    splay_tree<int> t2 = t.split_higher(k2);
    BOOST_CHECK(check_random_splay(t, b, b + k2 + 1, kL));
    BOOST_CHECK(check_random_splay(t2, b + k2 + 1, b + k1 + 1, kL));
    splay_tree<int> t3 = t.split_higher(k3);
    BOOST_CHECK(check_random_splay(t, b, b + k3 + 1, kL));
    BOOST_CHECK(check_random_splay(t3, b + k3 + 1, b + k2 + 1, kL));
}

BOOST_AUTO_TEST_CASE(splay_tree_split_lower) {
    srand(123433);
    const int k_n = 10435, kL = 47;
    std::array<int, k_n> input;
    boost::iota(input, 0);
    splay_tree<int> t(input);
    BOOST_CHECK(check_content(t, input));
    const int k1 = k_n / 2, k2 = k_n / 4, k3 = k_n / 8;
    splay_tree<int> t1 = t.split_lower(k1);
    auto b = input.begin();
    BOOST_CHECK(check_random_splay(t, b + k1, b + k_n, kL));
    BOOST_CHECK(check_random_splay(t1, b, b + k1, kL));
    splay_tree<int> t2 = t.split_lower(k2);
    BOOST_CHECK(check_random_splay(t, b + k1 + k2, b + k_n, kL));
    BOOST_CHECK(check_random_splay(t2, b + k1, b + k1 + k2, kL));
    splay_tree<int> t3 = t.split_lower(k3);
    BOOST_CHECK(check_random_splay(t, b + k1 + k2 + k3, b + k_n, kL));
    BOOST_CHECK(
    check_random_splay(t3, b + k1 + k2, b + k1 + k2 + k3, kL));
}

BOOST_AUTO_TEST_CASE(splay_tree_Merge) {
    srand(123433);
    const int k_n = 10382, k_m = 3875, kL = 51;
    BOOST_CHECK(k_n > k_m);
    std::array<int, k_n> input;
    boost::iota(input, 0);
    auto front = input | boost::adaptors::sliced(0, k_m);
    splay_tree<int> t1(front);
    BOOST_CHECK(check_content(t1, front));
    auto b = input.begin();
    splay_tree<int> t2(b + k_m, b + k_n);
    BOOST_CHECK(check_content(t2, b + k_m, b + k_n));
    t1.merge_right(t2);
    BOOST_CHECK(check_random_splay(t1, input, kL));
}

BOOST_AUTO_TEST_CASE(splay_tree_Reverse) {
    srand(123433);
    const int k_n = 231, k_m = 2000, kL = 47;
    std::array<int, k_n> input;
    boost::iota(input, 0);
    splay_tree<int> t(input);
    BOOST_CHECK(check_content(t, input));
    for (int i = 0; i < k_m; i++) {
        int a = rand() % k_n;                       // NOLINT
        int b = (a < k_n) ? rand() % (k_n - a) : 0; // NOLINT
        auto begin = input.begin();
        std::reverse(begin + a, begin + a + b + 1);
        t.reverse(a, a + b);
        BOOST_CHECK(check_random_splay(t, input, kL));
    }
}

class splay_tree_performance {
  public:
    static const int k_n = 104729, k_m = 26669;
    static const uint32_t seed = 331u;

    std::mt19937 gen;
    std::uniform_int_distribution<int> dist;
    std::array<int, k_n> input;
    splay_tree<int> tree;

    splay_tree_performance()
        : dist(std::uniform_int_distribution<int>(0, k_n - 1)) {}

    void test() {
        set_up();
        for (int i = 0; i < k_m; i++) {
            auto p = rand_reverse();
            tree.reverse(p.first, p.second);
        }
    }

  protected:
    std::pair<int, int> rand_reverse() {
        int a = dist(gen);
        int b = (a < k_n) ? dist(gen) % (k_n - a) : 0;
        return std::make_pair(a, a + b);
    }

    virtual void set_up() {
        gen.seed(seed);
        boost::iota(input, 0);
        tree = splay_tree<int>(input);
    }
};

const uint32_t splay_tree_performance::seed;

/*BOOST_AUTO_TEST_CASE(splay_tree_performance_ReverseVector) {
  for (int i = 0; i < k_m; i++) {
    auto p = rand_reverse();
    std::reverse(input + p.first, input + p.second + 1);
  }
}*/

BOOST_AUTO_TEST_CASE(splay_tree_performance_reverse_splay) {
    splay_tree_performance().test();
}

BOOST_AUTO_TEST_SUITE_END()
