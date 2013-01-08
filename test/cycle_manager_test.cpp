#define BOOST_TEST_MODULE cycle_manager

#include <boost/test/unit_test.hpp>
#include "cycle_manager.hpp"

#include <vector>
#include <string>

using std::string;
using std::vector;

class CheckSize {
public:
    CheckSize(CycleManager<string> cm, size_t size) : m_cm(cm), m_size(size) {}
    void operator()(const string & s) {
        auto r = m_cm.getEdgeRange(s);    
        BOOST_CHECK_EQUAL(m_size, size_t(std::distance(r.first, r.second)));        
    }
private:
    CycleManager<string> m_cm;
    size_t m_size;

};

class CheckAllSizes {
        typedef typename vector<string>::iterator iter;
    public:
        CheckAllSizes(iter begin) : m_begin(begin)  {}
        void operator()(iter end) {
            CycleManager<string> cm(m_begin, end);
            std::for_each(m_begin, end, CheckSize(cm, end - m_begin));
        }

    private:
        iter m_begin;
};

BOOST_AUTO_TEST_CASE(iterator_size) 
{
    vector<string> v = {"1","2","3","4","5","6","7","8","9","10"};
    typename vector<string>::iterator i = v.begin(); 
    typename vector<string>::iterator end = v.end(); 
    
    CheckAllSizes c(v.begin());
    for(; i!= end; ++i ) {
        c(i);
    }



}
