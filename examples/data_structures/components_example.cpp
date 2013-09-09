/**
 * @file components_example.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */


#include <type_traits>
#include <cassert>

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
    
struct X {
    explicit
    X(int _x) : x(_x) {}
    X()  = delete;
    bool operator==(X xx) const {
        return x == xx.x;
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

namespace ds = paal::data_structures;

//definition of Comps with names names::A, names::B, names::C
template <typename... Args>
using Comps = typename  ds::Components<
    names::A, names::B, names::C>::type<Args...> ;


//comnponts with defaults
template <typename... Args>
using CompsWithDefaults = typename  ds::Components<
    names::A, names::B, ds::NameWithDefault<names::C, X>>::type<Args...> ;

    
//definition of Comps with names names::A, names::B
template <typename... Args>
using CompsToReplace = typename  ds::Components<
    names::A, names::B>::type<Args...> ;

//this shouldn't compile
//template <typename... Args>
//using CompsWithDefaultsIncorrect = typename  ds::Components<
//        names::A, ds::NameWithDefault<names::B, X>, names::C>::type<Args...> ;


int main() {
    //constructor has default agruments
    Comps<int, double, int> comps;
    //getter
    comps.get<names::A>();

    //setter
    comps.set<names::A>(7);
    assert(comps.get<names::A>() == 7);
   
    //constructor with parameters 
    Comps<int, double, int> comps2(5,4,3);
    assert(comps2.get<names::A>() == 5);
    assert(comps2.get<names::B>() == 4);
    assert(comps2.get<names::C>() == 3);

    //default value for 3rd argument
    Comps<int, double, int> comps3(5,4);
    assert(comps2.get<names::A>() == 5);
    assert(comps2.get<names::B>() == 4);
    
    //declaration components with function 
    typedef Comps<int(*)(int), double, int> CompsF;
    
    CompsF comps4;
    const CompsF & constAlias = comps4;

    // non const version of get
    comps4.get<names::B>();
    // const version of get
    constAlias.get<names::B>();

    //definition
    CompsF comps5(f, 2, 17);
    
    //call the first argument
    assert(comps5.call<names::A>(2) == 2);

    //components with replaceped type
    typedef ds::ReplacedType<names::A, std::pair<int, int>, CompsF>::type Replaced;
    typedef Comps<std::pair<int, int>, double, int> ReplacedCheck;
    static_assert(std::is_same<Replaced, ReplacedCheck>::value, "Invalid replaceped type");


//    CompsF comps777(comps5, ds::detail::WrapToConstructable<ds::detail::Movable>());

    //replace components
    Replaced replace = ds::replace<names::A>(std::make_pair(11, 12), comps5);
    
    auto p = replace.get<names::A>();
    assert(p.first == 11);
    assert(p.second == 12);
    assert(replace.get<names::B>() == 2);
    assert(replace.get<names::C>() == 17);


    //normal definition
    CompsWithDefaults<int, double, float> compsDef;
    compsDef = compsDef;

    //definition with default 3rd template parameter
    CompsWithDefaults<int, double> compsDef2(1, 2, 3);
    assert(compsDef2.get<names::C>() == X(3));
    
    //This won't compile
    //CompsWithDefaults<int> comps3;


    X x(1);
    Y y(2);
    Z z(3);

    CompsToReplace<X, Y> compsToReplace(x, y);
    //replace, X, Y, Z doesn't have default constructors
    auto s = ds::replace<names::A>(z, compsToReplace);
    assert(s.get<names::A>().z == 3);
    
    auto s2 = ds::replace<names::B>(z, compsToReplace);
    assert(s2.get<names::B>().z == 3);

    //constructing objects providing names for objects
    typedef Comps<int, double, float> SomeComps;
    SomeComps someComps = SomeComps::make<names::A, names::C>(1,2);
    assert(someComps.get<names::A>() == 1);
    assert(someComps.get<names::C>() == 2);
    
    
    SomeComps someComps2 = SomeComps::make<names::C, names::A>(1,2);
    assert(someComps2.get<names::C>() == 1);
    assert(someComps2.get<names::A>() == 2);
    
    SomeComps someComps4(CompsToReplace<int, int>(1,2), ds::CopyTag());
    assert(someComps4.get<names::A>() == 1);
    assert(someComps4.get<names::B>() == 2);
    
    typedef Comps<int(*)(int), int(*)(int), int> CompsFF;

    CompsFF ff(f,f);
    ff.call<names::A>(2);
    ff.call<names::B>(2);


    // references works also
    int a;
    typedef Comps<int, const int &, int &> CompsWithRefs;
    CompsWithRefs compsWithRefs(a, a, a);

    CompsWithRefs::make<names::B, names::C>(a, a);
    
}
