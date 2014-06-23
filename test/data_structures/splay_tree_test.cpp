
#include "paal/data_structures/splay_tree.hpp"

#include <boost/test/unit_test.hpp>

#include <climits>
#include <algorithm>
#include <utility>
#include <vector>
#include <random>


using paal::data_structures::splay_tree::splay_tree;

template <typename T, typename I> bool check_content(T &t, I begin, I end) {
    if (!std::equal(begin, end, t.begin())) {
        return false;
    }
    std::reverse(begin, end);
    if (!std::equal(begin, end, t.rbegin())) {
        return false;
    }
    std::reverse(begin, end);
    for (int i = 0; begin != end; i++, begin++) {
        if (*begin != t[i]) {
            return false;
        }
    }
    return true;
}

template <typename T, typename I>
bool check_random_splay(T &t, I begin, I end, const std::size_t k_m) {
    const std::size_t k_n = end - begin;
    for (std::size_t i = 0; i < k_m; i++) {
        int el = rand() % k_n; // NOLINT
        if (*(begin + el) != *(t.splay(el))) {
            return false;
        }
    }
    if (!check_content(t, begin, end)) {
        return false;
    }
    return true;
}

BOOST_AUTO_TEST_SUITE(splay_treeTest)

BOOST_AUTO_TEST_CASE(splay_treeCreateEmpty) {
    splay_tree<int> t;
    BOOST_CHECK_EQUAL(std::size_t(0), t.size());
    BOOST_CHECK(t.empty());
    int *input = nullptr;
    BOOST_CHECK(check_content(t, input, input));
}

BOOST_AUTO_TEST_CASE(splay_tree_CreateFromRange) {
    const int k_n = 101;
    std::vector<int> input(k_n);
    std::iota(input.begin(), input.end(), 0);
    splay_tree<int> t(input.begin(), input.end());
    splay_tree<int> t1(input);
    BOOST_CHECK(check_content(t, input.begin(), input.end()));
    BOOST_CHECK(check_content(t1, input.begin(), input.end()));
}

BOOST_AUTO_TEST_CASE(splay_tree_Find) {
    const int k_n = 1001;
    int input[k_n];
    std::iota(input, input + k_n, 0);
    splay_tree<int> t(input, input + k_n);
    BOOST_CHECK(check_content(t, input, input + k_n));
    for (int i = 0; i < k_n; i++) {
        BOOST_CHECK_EQUAL(input[i], t[i]);
    }
}

BOOST_AUTO_TEST_CASE(splay_tree_splay) {
    srand(123433);
    const int k_n = 131, k_m = 1701;
    int input[k_n];
    std::iota(input, input + k_n, 0);
    splay_tree<int> t(input, input + k_n);
    BOOST_CHECK(check_content(t, input, input + k_n));
    BOOST_CHECK(check_random_splay(t, input, input + k_n, k_m));
}

BOOST_AUTO_TEST_CASE(splay_tree_split_higher) {
    srand(123433);
    const int k_n = 10435, kL = 47;
    int input[k_n];
    std::iota(input, input + k_n, 0);
    splay_tree<int> t(input, input + k_n);
    BOOST_CHECK(check_content(t, input, input + k_n));
    const int k1 = k_n / 2, k2 = k_n / 4, k3 = k_n / 8;
    splay_tree<int> t1 = t.split_higher(k1);
    BOOST_CHECK(check_random_splay(t, input, input + k1 + 1, kL));
    BOOST_CHECK(check_random_splay(t1, input + k1 + 1, input + k_n, kL));
    splay_tree<int> t2 = t.split_higher(k2);
    BOOST_CHECK(check_random_splay(t, input, input + k2 + 1, kL));
    BOOST_CHECK(check_random_splay(t2, input + k2 + 1, input + k1 + 1, kL));
    splay_tree<int> t3 = t.split_higher(k3);
    BOOST_CHECK(check_random_splay(t, input, input + k3 + 1, kL));
    BOOST_CHECK(check_random_splay(t3, input + k3 + 1, input + k2 + 1, kL));
}

BOOST_AUTO_TEST_CASE(splay_tree_split_lower) {
    srand(123433);
    const int k_n = 10435, kL = 47;
    int input[k_n];
    std::iota(input, input + k_n, 0);
    splay_tree<int> t(input, input + k_n);
    BOOST_CHECK(check_content(t, input, input + k_n));
    const int k1 = k_n / 2, k2 = k_n / 4, k3 = k_n / 8;
    splay_tree<int> t1 = t.split_lower(k1);
    BOOST_CHECK(check_random_splay(t, input + k1, input + k_n, kL));
    BOOST_CHECK(check_random_splay(t1, input, input + k1, kL));
    splay_tree<int> t2 = t.split_lower(k2);
    BOOST_CHECK(check_random_splay(t, input + k1 + k2, input + k_n, kL));
    BOOST_CHECK(check_random_splay(t2, input + k1, input + k1 + k2, kL));
    splay_tree<int> t3 = t.split_lower(k3);
    BOOST_CHECK(check_random_splay(t, input + k1 + k2 + k3, input + k_n, kL));
    BOOST_CHECK(
        check_random_splay(t3, input + k1 + k2, input + k1 + k2 + k3, kL));
}

BOOST_AUTO_TEST_CASE(splay_tree_Merge) {
    srand(123433);
    const int k_n = 10382, k_m = 3875, kL = 51;
    BOOST_CHECK(k_n > k_m);
    int input[k_n];
    std::iota(input, input + k_n, 0);
    splay_tree<int> t1(input, input + k_m);
    BOOST_CHECK(check_content(t1, input, input + k_m));
    splay_tree<int> t2(input + k_m, input + k_n);
    BOOST_CHECK(check_content(t2, input + k_m, input + k_n));
    t1.merge_right(t2);
    BOOST_CHECK(check_random_splay(t1, input, input + k_n, kL));
}

BOOST_AUTO_TEST_CASE(splay_tree_Reverse) {
    srand(123433);
    const int k_n = 231, k_m = 2000, kL = 47;
    int input[k_n];
    std::iota(input, input + k_n, 0);
    splay_tree<int> t(input, input + k_n);
    BOOST_CHECK(check_content(t, input, input + k_n));
    for (int i = 0; i < k_m; i++) {
        int a = rand() % k_n;                       // NOLINT
        int b = (a < k_n) ? rand() % (k_n - a) : 0; // NOLINT
        std::reverse(input + a, input + a + b + 1);
        t.reverse(a, a + b);
        BOOST_CHECK(check_random_splay(t, input, input + k_n, kL));
    }
}

class splay_tree_performance {
  public:
    static const int k_n = 104729, k_m = 26669;
    static const uint32_t seed = 331u;

    std::mt19937 gen;
    std::uniform_int_distribution<int> dist;
    int input[k_n];
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
        auto end = input + k_n;
        std::iota(input, end, 0);
        tree = splay_tree<int>(input, end);
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
