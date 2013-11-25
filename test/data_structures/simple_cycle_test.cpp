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

struct T {
    typedef typename std::vector<string>::iterator iter;
    typedef std::pair<string, string> P;
};

class CheckSize {
public:
    CheckSize(SimpleCycle<string> cm, std::size_t size) : m_cm(cm), m_size(size) {}
    void operator()(const string & s) {
        auto re = m_cm.getEdgeRange(s);    
        auto rv = m_cm.getVerticesRange(s);    
        BOOST_CHECK_EQUAL(m_size, std::size_t(std::distance(re.first, re.second)));        
        BOOST_CHECK_EQUAL(m_size, std::size_t(std::distance(rv.first, rv.second)));        
    }
private:
    SimpleCycle<string> m_cm;
    std::size_t m_size;

};

class CheckAllSizes : public T {
    public:
        CheckAllSizes(iter begin) : m_begin(begin)  {}
        void operator()(iter end) {
            SimpleCycle<string> cm(m_begin, end);
            std::for_each(m_begin, end, CheckSize(cm, end - m_begin));
        }

    private:
        iter m_begin;
};

template  <typename I> void  pe(I b, I e) {
    LOGLN("Edges:");

    for(;b != e; ++b) {
        LOGLN(b->first << "," << b->second);
    }

}


template <typename El, typename Sol> 
void checkSwapEdge(T::iter b, T::iter e, const El & p1, const  El & p2, const El & start, const Sol & sol) {
    SimpleCycle<string> cm(b, e);
    cm.flip(p1, p2);
    auto r = cm.getEdgeRange(start);
    BOOST_CHECK(vecEquals(sol.begin(), sol.end(), r.first, r.second));
}

BOOST_AUTO_TEST_SUITE(simple_cycle)
    
BOOST_AUTO_TEST_CASE(iterator_size) {
    T::iter i = v.begin(); 
    T::iter end = v.end(); 
    
    CheckAllSizes c(v.begin());
    for(; i!= end; ++i ) {
        c(i);
    }
}

template <typename T>
struct SimplCycleTemp : public SimpleCycle<T> {
    template <typename Iter> SimplCycleTemp(Iter b, Iter e) : SimpleCycle<T>(b, e) {}
};

BOOST_AUTO_TEST_CASE(swap_edges_3_test) {
    std::vector<T::P> sol = {T::P("1","2"), T::P("2", "3"), T::P("3","1")};
    checkSwapEdge(v.begin(), v.begin() + 3, "2", "2", "1", sol);
    swap_edges_3<SimplCycleTemp>();
}

BOOST_AUTO_TEST_CASE(swap_edges_4_test) {
    std::vector<T::P> solE = {T::P("1","3"), T::P("3", "2"), T::P("2","4"), T::P("4", "1")};
    checkSwapEdge(v.begin(), v.begin() + 4, "2", "3", "1", solE);

    swap_edges_4<SimplCycleTemp>();
}
BOOST_AUTO_TEST_SUITE_END()
