/**
 * @file simple_cycle_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */


#include "utils/logger.hpp"
#include "cycle.hpp"

using std::string;

using namespace paal::data_structures;


namespace {
struct types {
    using iter  = std::vector<string>::iterator;
    using edges = std::vector<std::pair<string, string>>;
};

class check_size {
  public:
    check_size(simple_cycle<string> cm, std::size_t size)
        : m_cm(std::move(cm)), m_size(size) {}
    void operator()(const string &s) {
        auto re = m_cm.get_edge_range(s);
        auto rv = m_cm.get_vertices_range(s);
        BOOST_CHECK_EQUAL(m_size, boost::distance(re));
        BOOST_CHECK_EQUAL(m_size, boost::distance(rv));
    }

  private:
    simple_cycle<string> m_cm;
    std::size_t m_size;
};

class check_all_sizes : public types {
  public:
    check_all_sizes(iter begin) : m_begin(begin) {}
    void operator()(iter end) {
        simple_cycle<string> cm(m_begin, end);
        std::for_each(m_begin, end, check_size(cm, end - m_begin));
    }

  private:
    iter m_begin;
};

template <typename El, typename Sol>
void check_swap_edge(types::iter b, types::iter e, const El &p1, const El &p2,
                     const El &start, const Sol &sol) {
    simple_cycle<string> cm(b, e);
    cm.flip(p1, p2);
    auto && r = cm.get_edge_range(start);
    BOOST_CHECK(boost::equal(sol, r));
}

} //! anonymous

BOOST_AUTO_TEST_SUITE(simple_cycle_suite)

BOOST_AUTO_TEST_CASE(iterator_size) {
    auto i = v.begin();
    auto end = v.end();

    check_all_sizes c(v.begin());
    for (; i != end; ++i) {
        c(i);
    }
}

template <typename T> struct simpl_cycle_temp : public simple_cycle<T> {
    template <typename Iter>
    simpl_cycle_temp(Iter b, Iter e)
        : simple_cycle<T>(b, e) {}
};

BOOST_AUTO_TEST_CASE(swap_edges_3_test) {
    types::edges sol = { {"1", "2"}, {"2", "3"}, {"3", "1"} };
    check_swap_edge(v.begin(), v.begin() + 3, "2", "2", "1", sol);
    swap_edges_3<simpl_cycle_temp>();
}

BOOST_AUTO_TEST_CASE(swap_edges_4_test) {
    types::edges sol = { {"1", "3"}, {"3", "2"}, {"2", "4"}, {"4", "1"} };
    check_swap_edge(v.begin(), v.begin() + 4, "2", "3", "1", sol);

    swap_edges_4<simpl_cycle_temp>();
}
BOOST_AUTO_TEST_SUITE_END()
