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

#include "paal/data_structures/components/components.hpp"
#include "paal/data_structures/components/components_swap.hpp"

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

struct A {
    explicit
    A(int x) : a(x) {}
    A()  = delete;
    bool operator==(A aa) const {
        return a== aa.a;
    }

    template <typename ostream>
    friend ostream & operator<<(ostream & o, A a) {
        o << a.a;
        return o;
    }

    int a;
};

struct B {
    explicit
    B(int x) : b(x) {}
    B()  = delete;

    int b;
};

struct C {
    explicit
    C(int x) : c(x) {}
    C()  = delete;

    int c;
};

template <typename... Args>
using CompsWithDefaults = typename  ds::Components<
        names::A, names::B, ds::NameWithDefault<names::C, A>>::type<Args...> ;

BOOST_AUTO_TEST_CASE(ComponentsTestDefaultParameters) {
    typedef ds::detail_set_defaults::SetDefaults<ds::TypesVector<names::A, names::B, ds::NameWithDefault<names::C, int>>, 
                                ds::TypesVector<int, double, int>> SetD;
    CompsWithDefaults<int, double, float> comps;
    CompsWithDefaults<int, double> comps2(1, 2, 3);
    BOOST_CHECK_EQUAL(comps2.get<names::C>(), A(3));
    BOOST_CHECK_EQUAL(comps.get<names::C>(), 0);
    
    //This won't compile
    //CompsWithDefaults<int> comps3;
}

template <typename... Args>
using CompsToSwap = typename  ds::Components<
        names::A, names::B>::type<Args...> ;


BOOST_AUTO_TEST_CASE(ComponentsSwapNotDefConstructible) {
    A a(1);
    B b(2);
    C c(3);

    CompsToSwap<A, B> comps(a, b);
    BOOST_CHECK_EQUAL(comps.get<names::A>().a, 1);
    auto s = ds::swap<names::A>(c, comps);
    BOOST_CHECK_EQUAL(s.get<names::A>().c, 3);
    
    auto s2 = ds::swap<names::B>(c, comps);
    BOOST_CHECK_EQUAL(s2.get<names::B>().c, 3);
}
