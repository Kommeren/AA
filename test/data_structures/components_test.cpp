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

#include "paal/data_structures/components/components.hpp"
#include "paal/data_structures/components/components_replace.hpp"

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
    BOOST_CHECK_EQUAL(comps2.get<names::B>(), 4);
    BOOST_CHECK_EQUAL(comps2.get<names::C>(), 3);

    //default value for 3rd argument
    Comps<int, double, int> comps3(5,4);
    BOOST_CHECK_EQUAL(comps3.get<names::A>(), 5);
    BOOST_CHECK_EQUAL(comps3.get<names::B>(), 4);
    BOOST_CHECK_EQUAL(comps3.get<names::C>(), 0);
    
    
    typedef Comps<int(*)(int), double, int> CompsF;
    
    CompsF comps4;
    const CompsF & alias = comps4;

    comps4.get<names::B>();
    alias.get<names::B>();

    CompsF comps5(f, 2, 17);

    BOOST_CHECK_EQUAL(comps5.call<names::A>(2), 2);

    typedef ds::ReplacedType<names::A, std::pair<int, int>, CompsF>::type Replaced;

    //This was not true when using mpl::vector
    typedef Comps<std::pair<int, int>, double, int> ReplacedCheck;
    static_assert(std::is_same<Replaced, ReplacedCheck>::value, "Invalid replaceped type");

    Replaced replace = ds::replace<names::A>(std::make_pair(11, 12), comps5);
    
    auto p = replace.get<names::A>();
    BOOST_CHECK_EQUAL(p.first, 11);
    BOOST_CHECK_EQUAL(p.second, 12);
    BOOST_CHECK_EQUAL(replace.get<names::B>(), 2);
    BOOST_CHECK_EQUAL(replace.get<names::C>(), 17);
}

    struct X {
        explicit
        X(int _x) : x(_x) {}
        X()  = delete;
        bool operator==(X xx) const {
            return x == xx.x;
        }

        template <typename ostream>
        friend ostream & operator<<(ostream & o, X x) {
            o << x.x;
        return o;
        }

        int x;
    };

    struct Y {
        explicit
        Y(int _y) : y(_y) {}
        Y()  = delete;

        int y;
    };

    struct Z {
        explicit
        Z(int _z) : z(_z) {}
        Z()  = delete;

        int z;
    };

template <typename... Args>
using CompsWithDefaults = typename  ds::Components<
        names::A, names::B, ds::NameWithDefault<names::C, X>>::type<Args...> ;

BOOST_AUTO_TEST_CASE(ComponentsTestDefaultParameters) {
    CompsWithDefaults<int, double, float> comps;
    CompsWithDefaults<int, double> comps2(1, 2, 3);
    BOOST_CHECK_EQUAL(comps2.get<names::C>(), X(3));
    BOOST_CHECK_EQUAL(comps.get<names::C>(), 0);
    
    //This won't compile
    //CompsWithDefaults<int> comps3;
}

template <typename... Args>
using CompsToReplace = typename  ds::Components<
        names::A, names::B>::type<Args...> ;


BOOST_AUTO_TEST_CASE(ComponentsReplaceNotDefConstructible) {
    X x(1);
    Y y(2);
    Z z(3);

    CompsToReplace<X, Y> comps(x, y);
    BOOST_CHECK_EQUAL(comps.get<names::A>().x, 1);
    auto s = ds::replace<names::A>(z, comps);
    BOOST_CHECK_EQUAL(s.get<names::A>().z, 3);
    
    auto s2 = ds::replace<names::B>(z, comps);
    BOOST_CHECK_EQUAL(s2.get<names::B>().z, 3);

    //typedef typename  ds::detail_set_defaults::SetDefaults<ds::TypesVector<names::A, ds::NameWithDefault<names::B, X>, names::C>, ds::TypesVector<int ,int ,int>>::type t;
}


//this shouldn't compile
//template <typename... Args>
//using CompsWithDefaultsIncorrect = typename  ds::Components<
//        names::A, ds::NameWithDefault<names::B, X>, names::C>::type<Args...> ;

