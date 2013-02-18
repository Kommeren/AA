#define BOOST_TEST_MODULE cycle_manager

#include <boost/test/unit_test.hpp>
#include "paal/data_structures/simple_cycle_manager.hpp"
#include "utils/logger.hpp"

#include <vector>
#include <string>

using std::string;
using std::vector;

using namespace paal::data_structures;

struct T {
    typedef typename vector<string>::iterator iter;
    typedef std::pair<string, string> P;
};

class CheckSize {
public:
    CheckSize(SimpleCycle<string> cm, size_t size) : m_cm(cm), m_size(size) {}
    void operator()(const string & s) {
        auto r = m_cm.getEdgeRange(s);    
        BOOST_CHECK_EQUAL(m_size, size_t(std::distance(r.first, r.second)));        
    }
private:
    SimpleCycle<string> m_cm;
    size_t m_size;

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
    LOG("Edges:");

    for(;b != e; ++b) {
        LOG(b->first << "," << b->second);
    }

}


template <typename I1, typename I2> bool vecEquals(I1 b1, I1 e1, I2 b2, I2 e2) {
    if(std::distance(b1, e1) != std::distance(b2, e2)) {
        return false;
    }
    return std::equal(b1, e1 , b2);
}

template <typename El, typename Sol> 
void checkSwap(T::iter b, T::iter e, const El & p1, const  El & p2, const El & start, const Sol & sol) {
    SimpleCycle<string> cm(b, e);
    cm.flip(p1, p2);
    auto r = cm.getEdgeRange(start);
    BOOST_CHECK(vecEquals(sol.begin(), sol.end(), r.first, r.second));
}

namespace {
    vector<string> v = {"1","2","3","4","5","6","7","8","9","10"};
}


BOOST_AUTO_TEST_CASE(iterator_size) {
    T::iter i = v.begin(); 
    T::iter end = v.end(); 
    
    CheckAllSizes c(v.begin());
    for(; i!= end; ++i ) {
        c(i);
    }
}

BOOST_AUTO_TEST_CASE(swap_edges_3) {
    std::vector<T::P> sol = {T::P("1","2"), T::P("2", "3"), T::P("3","1")};
    checkSwap(v.begin(), v.begin() + 3, "2", "2", "1", sol);
}

BOOST_AUTO_TEST_CASE(swap_edges_4) {
    std::vector<T::P> sol = {T::P("1","3"), T::P("3", "2"), T::P("2","4"), T::P("4", "1")};
    checkSwap(v.begin(), v.begin() + 4, "2", "3", "1", sol);
    
}
