#include "utils/logger.hpp"

#include "paal/data_structures/vertex_to_edge_iterator.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

using std::string;
using std::vector;

using namespace paal::data_structures;

struct T {
    typedef typename vector<string>::iterator iter;
    typedef std::pair<string, string> P;
};

class check_all_sizes : public T {
  public:
    check_all_sizes(iter begin) : m_begin(begin) {}
    void operator()(iter end) {
        vertex_to_edge_iterator<iter> b(m_begin, end);
        vertex_to_edge_iterator<iter> e(end, end);
        BOOST_CHECK_EQUAL(std::distance(b, e), std::distance(m_begin, end));
    }

  private:
    iter m_begin;
};

template <typename I> void pe(I b, I e) {
    LOGLN("Edges:");

    for (; b != e; ++b) {
        LOGLN(b->first << "," << b->second);
    }
}

template <typename I1, typename I2>
bool vec_equals(I1 b1, I1 e1, I2 b2, I2 e2) {
    if (std::distance(b1, e1) != std::distance(b2, e2)) {
        return false;
    }
    return std::equal(b1, e1, b2);
}

namespace {
vector<string> v = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
}

BOOST_AUTO_TEST_CASE(iterator_size) {
    auto i = v.begin();
    auto end = v.end();

    check_all_sizes c(v.begin());
    for (; i != end; ++i) {
        c(i);
    }
}

BOOST_AUTO_TEST_CASE(iterator_valid) {
    std::vector<T::P> sol = { T::P("1", "2"), T::P("2", "3"), T::P("3", "1") };
    auto end = v.begin() + 3;
    BOOST_CHECK(vec_equals(sol.begin(), sol.end(),
                           vertex_to_edge_iterator<T::iter>(v.begin(), end),
                           vertex_to_edge_iterator<T::iter>(end, end)));
}
