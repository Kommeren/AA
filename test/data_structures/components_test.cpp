/**
 * @file components_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */
#define BOOST_TEST_MODULE components

#include <boost/test/unit_test.hpp>

#include <type_traits>
#include <boost/mpl/equal.hpp>

#include "paal/data_structures/components.hpp"



namespace names {
    struct A;
    struct B;
    struct C;
}

int f(int i) {
    return i;
}

template <typename... Args>
using Comps = typename  paal::data_structures::Components<
        names::A, names::B, names::C>::type<Args...> ;

        
namespace bm = boost::mpl;
namespace bf = boost::fusion;

BOOST_AUTO_TEST_CASE(ComponentsTest) {

    //default
    Comps<int, double, int> comps;
    comps.get<names::A>();
    comps.set<names::A>(7);
    BOOST_CHECK_EQUAL(comps.get<names::A>(), 7);
    
    Comps<int, double, int> comps2(5,4,3);
    BOOST_CHECK_EQUAL(comps2.get<names::A>(), 5);

    //default value for 3rd argument
    Comps<int, double, int> comps3(5,4);
    BOOST_CHECK_EQUAL(comps3.get<names::A>(), 5);
    
    typedef Comps<int(*)(int), double, int> CompsF;
    
    CompsF comps4;

    CompsF comps5(f, 2, 17);

    BOOST_CHECK_EQUAL(comps5.call<names::A>(2), 2);

    typedef paal::data_structures::SwapType<names::A, std::pair<int, int>, CompsF>::type Swapped;

    //This is unfortunately not true
    //typedef Comps<std::pair<int, int>, double, int> SwappedCheck;
//    static_assert(std::is_same<Swapped, SwappedCheck>::value, "Invalid swapped type");

    Swapped swap = paal::data_structures::swap<names::A>(std::make_pair(11, 12), comps5);
    
    auto p = swap.get<names::A>();
    BOOST_CHECK_EQUAL(p.first, 11);
    BOOST_CHECK_EQUAL(p.second, 12);
    BOOST_CHECK_EQUAL(swap.get<names::B>(), 2);
    BOOST_CHECK_EQUAL(swap.get<names::C>(), 17);

}
