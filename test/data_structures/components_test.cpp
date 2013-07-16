/**
 * @file components_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */
#define BOOST_TEST_MODULE components

#include <boost/test/unit_test.hpp>
#include "paal/data_structures/components.hpp"

namespace comps {
    struct CompA;
    struct CompB;
    struct CompC;
}

int f(int i) {
    return i;
}

template <typename... Args>
using Comps = typename  paal::data_structures::Components<
        comps::CompA, comps::CompB, comps::CompC>::type<Args...> ;

        
namespace bm = boost::mpl;
namespace bf = boost::fusion;

BOOST_AUTO_TEST_CASE(ComponentsTest) {

    //default
    Comps<int, double, int> comps;
    comps.get<comps::CompA>();
    comps.set<comps::CompA>(7);
    BOOST_CHECK_EQUAL(comps.get<comps::CompA>(), 7);
    
    Comps<int, double, int> comps2(5,4,3);
    BOOST_CHECK_EQUAL(comps2.get<comps::CompA>(), 5);

    //default value for 3rd argument
    Comps<int, double, int> comps3(5,4);
    BOOST_CHECK_EQUAL(comps3.get<comps::CompA>(), 5);
    
    
    Comps<int(*)(int), double, int> comps4;

    Comps<int(*)(int), double, int> comps5(f);
    BOOST_CHECK_EQUAL(comps5.call<comps::CompA>(2), 2);
}
