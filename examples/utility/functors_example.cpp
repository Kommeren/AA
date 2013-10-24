#include <vector>

#include "paal/utils/functors.hpp"



using namespace paal::utils;

void functors_example() {
    //skip
    SkipFunctor skip;

    skip(2, 2.1, "asda");

    //identity
    IdentityFunctor id;

    assert(id("asd") == "asd");
    assert(id(7) == 7);

    //return something
    ReturnTrueFunctor retTrue;
    ReturnFalseFunctor retFalse;
    ReturnZeroFunctor retZero;

    assert(retTrue(2, 2.3, "abc"));
    assert(!retFalse(2, 2.3, "abc"));
    assert(retZero(2, 2.3, "abc") == 0);

    //assert
    //AssertFunctor assertFun;
    //assertFun(); //aborts
    

    //array to functor
    std::vector<int> vec{1,2,3};
    auto vecFun = make_ArrayToFunctor(vec);
    assert(vecFun(1) == 2);
    
    auto vecFunWithOffset = make_ArrayToFunctor(vec, 1);
    assert(vecFunWithOffset(1) == 3);
};

void compare_functors() {
    Greater g;
    GreaterEqual ge;
    Less l;
    LessEqual le;
    EqualTo e;
    NotEqualTo ne;

    assert(!g(1,2));
    assert(!g(1,1));
    assert( g(2,1));
    
    assert(!ge(1,2));
    assert( ge(1,1));
    assert( ge(2,1));
    
    assert( l(1,2));
    assert(!l(1,1));
    assert(!l(2,1));
    
    assert( le(1,2));
    assert( le(1,1));
    assert(!le(2,1));
    
    assert(!e(1,2));
    assert( e(1,1));
    assert(!e(2,1));
    
    assert( ne(1,2));
    assert(!ne(1,1));
    assert( ne(2,1));
}

void comparator_functor() {
    auto getFirst = [](std::pair<int, int> p){return p.first;};
    auto compareFirst = make_FunctorToComparator(getFirst);

    assert(!compareFirst(std::make_pair(1,2), std::make_pair(0,1)));
    
    auto compareFirstDesc = make_FunctorToComparator(getFirst, Greater());
    
    assert(compareFirstDesc(std::make_pair(1,2), std::make_pair(0,1)));
}

void boolean_functors() {
    Not notFun;
    Or orFun;
    And andFun;

    assert(!notFun(true));
    assert( notFun(false));

    assert(!orFun(false, false));
    assert( orFun(true , false));
    assert( orFun(false, true));
    assert( orFun(true , true));
    
    assert(!andFun(false, false));
    assert(!andFun(true , false));
    assert(!andFun(false, true));
    assert( andFun(true , true));
}

void lift_operator_functor() {
    auto oper = [](int a, int b) {return a + b > 0;};
    ReturnZeroFunctor zero;
    ReturnSomethingFunctor<int, 5> five;

    auto f =  make_LiftBinaryOperatorFunctor(zero, five, oper);

    assert(f(1, 2, 41243.2, "dada"));
}

void boolean_functors_on_functors() {
    ReturnTrueFunctor retTrue;
    ReturnFalseFunctor retFalse;

    {
        auto trueFunctor = make_NotFunctor(retFalse);
        assert(trueFunctor(1.2, "xada", 3));
    }
    
    {
        auto falseFunctor = make_NotFunctor(retTrue);
        assert(!falseFunctor(1.2, "xada", 3));
    }

    {
        auto trueFunctor = make_OrFunctor(retTrue, retFalse);
        assert(trueFunctor(1.2, "xada", 3));
    }
    
    {
        auto falseFunctor = make_AndFunctor(retTrue, retFalse);
        assert(!falseFunctor(1.2, "xada", 3));
    }
    
    {
        auto trueFunctor = make_XorFunctor(retTrue, retFalse);
        assert(trueFunctor(1.2, "xada", 3));
    }
}


int main() {
    functors_example();
    boolean_functors();
    comparator_functor();
    boolean_functors();
    lift_operator_functor();
    boolean_functors_on_functors();

    return 0;
}
