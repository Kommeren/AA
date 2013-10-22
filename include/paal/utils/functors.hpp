/**
 * @file functors.hpp
 * @brief This file contains set of simple useful functors or functor adapters. 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-04
 */
#ifndef FUNCTORS_HPP
#define FUNCTORS_HPP 
#include <cassert>
#include <utility>

namespace paal {
namespace utils {


//Functor does nothing
struct SkipFunctor {
    template <typename ... Args > 
        void  operator()(Args&&... args) const {}
};

//Functor returns always the same number. 
//The number has to be known at compile time
template <typename T, T t>
    struct ReturnSomethingFunctor {
        template <typename ... Args > 
            T  operator()(Args&&... args) const {
                return t;
            } 
    };

//functor returns its argument
struct IdentityFunctor {
    template <typename Arg> 
        auto  operator()(Arg&& arg) const ->
        Arg
        { 
            return std::forward<Arg>(arg);
        }
};

//functor return false
struct ReturnFalseFunctor : 
    public ReturnSomethingFunctor<bool, false> {};

//functor return true
struct ReturnTrueFunctor : 
    public ReturnSomethingFunctor<bool, true> {};

//functor returns 0
struct ReturnZeroFunctor :
    public ReturnSomethingFunctor<int, 0> {};

//functors calls assert(false). 
struct AssertFunctor {
    template <typename ... Args > 
        void  operator()(Args&&... args) const {
            assert(false);
        } 
};


// Adapts array as function, providing operator()().     
template <typename Array> 
    class ArrayToFunctor{
        public:
            ArrayToFunctor(const Array & array, int offset = 0) : 
                m_array(array), m_offset(offset) {}

            typedef decltype(std::declval<const Array>()[0]) Value;
            Value operator()(int a) const {return m_array[a + m_offset];}

        private:
            const Array & m_array;
            int m_offset;
    };

template <typename Array>
    ArrayToFunctor<Array> make_ArrayToFunctor(const Array &a, int offset = 0) {
        return ArrayToFunctor<Array>(a, offset);
    }

//************ The set of comparison functors *******************
//functors are equivalent to corresponding std functors (e.g. std::less) but are not templated

struct Greater {
    template<class T>
        bool operator() (const T& x, const T& y) const {
            return x > y;
        };
};

struct Less {
    template<class T>
        bool operator() (const T& x, const T& y) const {
            return x < y;
        };
};

struct GreaterEqual {
    template<class T>
        bool operator() (const T& x, const T& y) const {
            return x >= y;
        };
};

struct LessEqual {
    template<class T>
        bool operator() (const T& x, const T& y) const {
            return x <= y;
        };
};

struct EqualTo {
    template<class T>
        bool operator() (const T& x, const T& y) const {
            return x == y;
        };
};

struct NotEqualTo {
    template<class T>
        bool operator() (const T& x, const T& y) const {
            return x != y;
        };
};


//This comparator  takes functor "f" and comparator "c"
//and for elements(x,y) returns c(f(x), f(y))
//c is Less by default
template <typename Functor,typename Compare=Less>
    struct FunctorToComparator {
        FunctorToComparator(Functor f,Compare c=Compare()) : m_f(f),m_c(c){}

        template <typename T>
            bool operator()(const T & left, const T & right) const {
                return m_c(m_f(left), m_f(right));
            }

    private:
        Functor m_f;
        Compare m_c;
    };


template <typename Functor,typename Compare = Less>
    FunctorToComparator<Functor,Compare>
    make_FunctorToComparator(Functor functor,Compare compare=Compare()) {
        return FunctorToComparator<Functor,Compare>(std::move(functor), std::move(compare));
    };

//The output Iterator takes functor and
//each time the operator= is called the given function is called
template <typename Functor>
    struct FunctorToOutputIterator {
        typedef std::output_iterator_tag iterator_category;
        typedef FunctorToOutputIterator   value_type;
        typedef void                      difference_type;
        typedef FunctorToOutputIterator * pointer;
        typedef FunctorToOutputIterator & reference;


        FunctorToOutputIterator(Functor functor= Functor()) :
            m_functor(functor) {}

        template <typename Arg> 
            FunctorToOutputIterator & operator=(Arg&& arg) {
                m_functor(std::forward<Arg>(arg));
                return *this;
            }

        FunctorToOutputIterator & operator++() {
            return *this;
        }

        FunctorToOutputIterator & operator*() {
            return *this;
        }

    private:
        Functor m_functor;
    };


template <typename Functor>
    FunctorToOutputIterator<Functor>
    make_FunctorToOutputIterator(Functor functor) {
        return FunctorToOutputIterator<Functor>(std::move(functor));
    }


//****************************** This is set of functors representing standard boolean operation
//that is !, &&, ||, !=(xor).
struct Not {
    bool operator()(bool b) const {
        return !b;
    }
};

struct Or {
    bool operator()(bool left, bool right) const {
        return left || right;
    }
};

struct And {
    bool operator()(bool left, bool right) const {
        return left && right;
    }
};

struct Xor {
    bool operator()(bool left, bool right) const {
        return left != right;
    }
};

//Functor stores binary operator "o" and two functors "f" and "g"
//for given "args" returns o(f(args), g(args))
template <typename FunctorLeft, typename FunctorRight, typename Operator>
    struct LiftBinaryOperatorFunctor {
        LiftBinaryOperatorFunctor(FunctorLeft left = FunctorLeft(),
                                  FunctorRight right = FunctorRight(),
                                  Operator op = Operator()) :
            m_left(std::move(left)), m_right(std::move(right)), 
            m_operator(std::move(op)) {}

        template <typename ... Args> 
            bool  operator()(Args&&... args) const {
                return m_operator(m_left(std::forward<Args>(args)...), 
                                  m_right(std::forward<Args>(args)...));
            }

    private:
        FunctorLeft m_left;
        FunctorRight m_right;
        Operator m_operator;
    };

template <typename FunctorLeft, typename FunctorRight, typename Operator>
    LiftBinaryOperatorFunctor<FunctorLeft, FunctorRight, Operator>
    make_LiftBinaryOperatorFunctor(
            FunctorLeft left, FunctorRight right, Operator op) {
        return LiftBinaryOperatorFunctor
                <FunctorLeft, FunctorRight, Operator>(
                    std::move(left), std::move(right), std::move(op));
    }


//******************** this is set of functors 
//allowing two compose functors which returns bool using
//standard bool operators


//Not
template <typename Functor>
    struct NotFunctor {
        NotFunctor(Functor functor= Functor()) :
            m_functor(functor) {}

        template <typename ... Args> 
            bool  operator()(Args&&... args) const {
                return !m_functor(std::forward<Args>(args)...);
            }

    private:
        Functor m_functor;
    };


template <typename Functor>
    NotFunctor<Functor>
    make_NotFunctor(Functor functor) {
        return NotFunctor<Functor>(std::move(functor));
    }


//Or
template <typename FunctorLeft, typename FunctorRight>
    class OrFunctor : 
            public LiftBinaryOperatorFunctor<FunctorLeft, FunctorRight, Or> {
        typedef LiftBinaryOperatorFunctor<FunctorLeft, FunctorRight, Or> base;

    public:
        OrFunctor(FunctorLeft left = FunctorLeft(), 
                  FunctorRight right = FunctorRight()) :
            base(std::move(left), std::move(right)) {}
    };

template <typename FunctorLeft, typename FunctorRight>
    OrFunctor<FunctorLeft, FunctorRight>
    make_OrFunctor(FunctorLeft left, FunctorRight right) {
        return OrFunctor<FunctorLeft, FunctorRight>(std::move(left), std::move(right));
    }

//And
template <typename FunctorLeft, typename FunctorRight>
    class AndFunctor :
            public LiftBinaryOperatorFunctor<FunctorLeft, FunctorRight, And> {
        typedef LiftBinaryOperatorFunctor<FunctorLeft, FunctorRight, And> base;

    public:

        AndFunctor(FunctorLeft left = FunctorLeft(), FunctorRight right = FunctorRight()) :
            base(std::move(left), std::move(right)) {}
    };

//Xor
template <typename FunctorLeft, typename FunctorRight>
    AndFunctor<FunctorLeft, FunctorRight>
    make_AndFunctor(FunctorLeft left, FunctorRight right) {
        return AndFunctor<FunctorLeft, FunctorRight>(std::move(left), std::move(right));
    }

template <typename FunctorLeft, typename FunctorRight>
    class XorFunctor :
            public LiftBinaryOperatorFunctor<FunctorLeft, FunctorRight, Xor> {
        typedef LiftBinaryOperatorFunctor<FunctorLeft, FunctorRight, Xor> base;

    public:
        XorFunctor(FunctorLeft left = FunctorLeft(), FunctorRight right = FunctorRight()) :
            base(std::move(left), std::move(right)) {}

    };

template <typename FunctorLeft, typename FunctorRight>
    XorFunctor<FunctorLeft, FunctorRight>
    make_XorFunctor(FunctorLeft left, FunctorRight right) {
        return XorFunctor<FunctorLeft, FunctorRight>(std::move(left), std::move(right));
    }

} //utils
} //paal
#endif /* FUNCTORS_HPP */
