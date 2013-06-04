#define BOOST_TEST_MODULE bimap

#include <vector>
#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>
#include "paal/data_structures/bimap.hpp"
#include "utils/logger.hpp"

static const int s  = 10000000;
static const int nr = 10000000;
std::vector<int> vec(s);
std::vector<int> ids(nr);

template <typename Bimap>
void testGetVal(const Bimap & b) {
    int t(0);
    for(int j  : boost::irange(0,50)) {
        std::ignore = j; //removes warning
        for(int i : ids) {
            t+=b.getVal(i); //has to do addition, because without it compiler removes the whole loop
        }
    }
    LOG(t);
}

template <typename Bimap>
void testGetIdx(const Bimap & b) {
    for(int i : ids) {
        b.getIdx(vec[i]);
    }
}

template <typename Bimap>
void test() {
    ON_LOG(time_t t1 = time(NULL));
    Bimap b(vec.begin(), vec.end());
    ON_LOG(time_t t2 = time(NULL));
    LOG("construction time " << t2 - t1);
    ON_LOG(t1 = t2);
    testGetVal(b);
    ON_LOG(t2 = time(NULL));
    LOG("getval time " << t2 - t1);
    
    ON_LOG(t1 = t2);
    testGetIdx(b);
    ON_LOG(t2 = time(NULL));
    LOG("getidx time " << t2 - t1);
}

BOOST_AUTO_TEST_CASE(bimap) {

    std::iota(vec.begin(), vec.end(), 0);
    std::random_shuffle(vec.begin(), vec.end());
    
    for(int & i : ids) {
        i = rand() % s;
    }

    LOG("boost::mulit_index Bimap");
    test<paal::data_structures::BiMapMIC<int>>();
    LOG("paal Bimap");
    test<paal::data_structures::BiMap<int>>();
}
