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
#include "paal/data_structures/components/components_swap.hpp"

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
using CompsToSwap = typename  ds::Components<
    names::A, names::B>::type<Args...> ;

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
    assert(comps2.get<names::C>() == 0);
    
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

    //components with swapped type
    typedef ds::SwapType<names::A, std::pair<int, int>, CompsF>::type Swapped;
    typedef Comps<std::pair<int, int>, double, int> SwappedCheck;
    static_assert(std::is_same<Swapped, SwappedCheck>::value, "Invalid swapped type");

    //swap components
    Swapped swap = ds::swap<names::A>(std::make_pair(11, 12), comps5);
    
    auto p = swap.get<names::A>();
    assert(p.first == 11);
    assert(p.second == 12);
    assert(swap.get<names::B>() == 2);
    assert(swap.get<names::C>() == 17);


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

    CompsToSwap<X, Y> compsToSwap(x, y);
    //swap, X, Y, Z doesn't have default constructors
    auto s = ds::swap<names::A>(z, compsToSwap);
    assert(s.get<names::A>().z == 3);
    
    auto s2 = ds::swap<names::B>(z, compsToSwap);
    assert(s2.get<names::B>().z == 3);
}
