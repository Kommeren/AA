#include <boost/test/unit_test.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <climits>
#include <algorithm>
#include <utility>
#include <vector>

#include "paal/data_structures/splay_tree.hpp"

using paal::data_structures::splay_tree::SplayTree;
using paal::data_structures::splay_tree::SplayImplEnum;

template<typename T, typename I> bool check_content(T &t, I begin, I end) {
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

template<typename T, typename I> bool check_random_splay(T &t, I begin,
    I end, const size_t kM) {
  const size_t kN = end - begin;
  for (size_t i = 0; i < kM; i++) {
    int el = rand() % kN; // NOLINT
    if (*(begin + el) != t.splay(el)->val_) {
      return false;
    }
  }
  if (!check_content(t, begin, end)) {
    return false;
  }
  return true;
}

template<typename I> void fill_range(I begin, I end) {
  int i = 0;
  for (I iter = begin; iter != end; iter++) {
    *iter = i;
    i++;
  }
}

BOOST_AUTO_TEST_CASE(SplayTreeCreateEmpty) {
  SplayTree<int> t;
  BOOST_CHECK_EQUAL(size_t(0), t.size());
  BOOST_CHECK(t.empty());
  int *input = NULL;
  BOOST_CHECK(check_content(t, input, input));
}

BOOST_AUTO_TEST_CASE(SplayTree_CreateFromRange) {
  const int kN = 101;
  std::vector<int> input(kN);
  fill_range(input.begin(), input.end());
  SplayTree<int> t(input.begin(), input.end());
  SplayTree<int> t1(input);
  BOOST_CHECK(check_content(t, input.begin(), input.end()));
  BOOST_CHECK(check_content(t1, input.begin(), input.end()));
}

BOOST_AUTO_TEST_CASE(SplayTree_Find) {
  const int kN = 1001;
  int input[kN];
  fill_range(input, input + kN);
  SplayTree<int> t(input, input + kN);
  BOOST_CHECK(check_content(t, input, input + kN));
  for (int i = 0; i < kN; i++) {
    BOOST_CHECK_EQUAL(input[i], t[i]);
  }
}

BOOST_AUTO_TEST_CASE(SplayTree_Splay) {
  srand(123433);
  const int kN = 131, kM = 1701;
  int input[kN];
  fill_range(input, input + kN);
  SplayTree<int> t(input, input + kN);
  BOOST_CHECK(check_content(t, input, input + kN));
  BOOST_CHECK(check_random_splay(t, input, input + kN, kM));
}

BOOST_AUTO_TEST_CASE(SplayTree_SplitHigher) {
  srand(123433);
  const int kN = 10435, kL = 47;
  int input[kN];
  fill_range(input, input + kN);
  SplayTree<int> t(input, input + kN);
  BOOST_CHECK(check_content(t, input, input + kN));
  const int k1 = kN / 2, k2 = kN / 4, k3 = kN / 8;
  SplayTree<int> t1 = t.split_higher(k1);
  BOOST_CHECK(check_random_splay(t, input, input + k1 + 1, kL));
  BOOST_CHECK(check_random_splay(t1, input + k1 + 1, input + kN, kL));
  SplayTree<int> t2 = t.split_higher(k2);
  BOOST_CHECK(check_random_splay(t, input, input + k2 + 1, kL));
  BOOST_CHECK(check_random_splay(t2, input + k2 + 1, input + k1 + 1, kL));
  SplayTree<int> t3 = t.split_higher(k3);
  BOOST_CHECK(check_random_splay(t, input, input + k3 + 1, kL));
  BOOST_CHECK(check_random_splay(t3, input + k3 + 1, input + k2 + 1, kL));
}

BOOST_AUTO_TEST_CASE(SplayTree_SplitLower) {
  srand(123433);
  const int kN = 10435, kL = 47;
  int input[kN];
  fill_range(input, input + kN);
  SplayTree<int> t(input, input + kN);
  BOOST_CHECK(check_content(t, input, input + kN));
  const int k1 = kN / 2, k2 = kN / 4, k3 = kN / 8;
  SplayTree<int> t1 = t.split_lower(k1);
  BOOST_CHECK(check_random_splay(t, input + k1, input + kN, kL));
  BOOST_CHECK(check_random_splay(t1, input, input + k1, kL));
  SplayTree<int> t2 = t.split_lower(k2);
  BOOST_CHECK(check_random_splay(t, input + k1 + k2, input + kN, kL));
  BOOST_CHECK(check_random_splay(t2, input + k1, input + k1 + k2, kL));
  SplayTree<int> t3 = t.split_lower(k3);
  BOOST_CHECK(check_random_splay(t, input + k1 + k2 + k3, input + kN, kL));
  BOOST_CHECK(check_random_splay(t3, input + k1 + k2,
      input + k1 + k2 + k3, kL));
}

BOOST_AUTO_TEST_CASE(SplayTree_Merge) {
  srand(123433);
  const int kN = 10382, kM = 3875, kL = 51;
  BOOST_CHECK(kN > kM);
  int input[kN];
  fill_range(input, input + kN);
  SplayTree<int> t1(input, input + kM);
  BOOST_CHECK(check_content(t1, input, input + kM));
  SplayTree<int> t2(input + kM, input + kN);
  BOOST_CHECK(check_content(t2, input + kM, input + kN));
  t1.merge_right(t2);
  BOOST_CHECK(check_random_splay(t1, input, input + kN, kL));
}

BOOST_AUTO_TEST_CASE(SplayTree_Reverse) {
  srand(123433);
  const int kN = 231, kM = 2000, kL = 47;
  int input[kN];
  fill_range(input, input + kN);
  SplayTree<int> t(input, input + kN);
  BOOST_CHECK(check_content(t, input, input + kN));
  for (int i = 0; i < kM; i++) {
    int a = rand() % kN; // NOLINT
    int b = (a < kN) ? rand() % (kN - a) : 0; // NOLINT
    std::reverse(input + a, input + a + b + 1);
    t.reverse(a, a + b);
    BOOST_CHECK(check_random_splay(t, input, input + kN, kL));
  }
}

class SplayTreePerformance {
  public:
    static const enum SplayImplEnum splay_impl = paal::data_structures::splay_tree::kTopDownUnbalanced;

    static const int kN = 104729, kM = 26669;
    static const uint32_t seed = 331u;

    boost::random::mt19937 gen;
    boost::random::uniform_int_distribution<int> dist;
    int input[kN];
    SplayTree<int, splay_impl> *tree;

    SplayTreePerformance() :  
      dist(boost::random::uniform_int_distribution<int>(0, kN - 1)), tree(NULL){
    }

    void test() {
        SetUp();
        for (int i = 0; i < kM; i++) {
        auto p = rand_reverse();
        tree->reverse(p.first, p.second);
      }
    }

  protected:
    std::pair<int, int> rand_reverse() {
      int a = dist(gen);
      int b = (a < kN) ? dist(gen) % (kN - a) : 0;
      return std::make_pair(a, a + b);
    }
    virtual void SetUp() {
      gen.seed(seed);
      auto end = input + kN;
      fill_range(input, end);
      tree = new SplayTree<int, splay_impl>(input, end);
    }
};

const uint32_t SplayTreePerformance::seed;

/*BOOST_AUTO_TEST_CASE(SplayTreePerformance_ReverseVector) {
  for (int i = 0; i < kM; i++) {
    auto p = rand_reverse();
    std::reverse(input + p.first, input + p.second + 1);
  }
}*/

BOOST_AUTO_TEST_CASE(SplayTreePerformance_ReverseSplay) {
    SplayTreePerformance().test();
}
