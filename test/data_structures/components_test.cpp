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

namespace ds = paal::data_structures;


template <typename... Args>
using Comps = typename  ds::Components<
        names::A, names::B, names::C>::type<Args...> ;

        
BOOST_AUTO_TEST_CASE(ComponentsTest) {
    //default arguments
    Comps<int, double, int> comps;
    comps.get<names::A>();
    comps.set<names::A>(7);
    BOOST_CHECK_EQUAL(comps.get<names::A>(), 7);
   

    Comps<int, double, int> comps2(5,4,3);
    static_assert(ds::detail::HasTemplateGet<Comps<int, double,int>, names::A>::value, "HasGet doesn't work");
    BOOST_CHECK_EQUAL(comps2.get<names::A>(), 5);

    //default value for 3rd argument
    Comps<int, double, int> comps3(5,4);
    BOOST_CHECK_EQUAL(comps3.get<names::A>(), 5);
    
    
    typedef Comps<int(*)(int), double, int> CompsF;
    
    CompsF comps4;
    const CompsF & alias = comps4;

    comps4.get<names::B>();
    alias.get<names::B>();

    CompsF comps5(f, 2, 17);

    BOOST_CHECK_EQUAL(comps5.call<names::A>(2), 2);

    typedef ds::SwapType<names::A, std::pair<int, int>, CompsF>::type Swapped;

    //This was not true when using mpl::vector
    typedef Comps<std::pair<int, int>, double, int> SwappedCheck;
    static_assert(std::is_same<Swapped, SwappedCheck>::value, "Invalid swapped type");

    Swapped swap = ds::swap<names::A>(std::make_pair(11, 12), comps5);
    
    auto p = swap.get<names::A>();
    BOOST_CHECK_EQUAL(p.first, 11);
    BOOST_CHECK_EQUAL(p.second, 12);
    BOOST_CHECK_EQUAL(swap.get<names::B>(), 2);
    BOOST_CHECK_EQUAL(swap.get<names::C>(), 17);
}

template <typename... Args>
using CompsWithDefaults = typename  ds::Components<
        names::A, names::B, ds::NameWithDefault<names::C, int>>::type<Args...> ;

BOOST_AUTO_TEST_CASE(ComponentsTestDefaultParameters) {
    typedef ds::detail::SetDefaults<ds::TypesVector<names::A, names::B, ds::NameWithDefault<names::C, int>>, 
                                ds::TypesVector<int, double, float>> SetD;
    CompsWithDefaults<int, double, float> comps;
    CompsWithDefaults<int, double> comps2(1,2, 3.5);
    BOOST_CHECK_EQUAL(comps2.get<names::C>(), 3);
    BOOST_CHECK_EQUAL(comps.get<names::C>(), 0);
    
    //This won't compile
    //CompsWithDefaults<int> comps3;
}
