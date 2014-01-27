//=======================================================================
// Copyright 2013 University of Warsaw.
// Authors: Piotr Wygocki
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
//
//This file contains set of simple useful functors or functor adapters.

#ifndef BOOST_FUNCTORS_HPP
#define BOOST_FUNCTORS_HPP

#define BOOST_RESULT_OF_USE_DECLTYPE

#include <cassert>
#include <utility>

namespace paal {
namespace utils {

/**
 * @brief Functor does nothing
 */
struct SkipFunctor {
    /**
     * @brief operator
     *
     * @tparam Args
     * @param args
     */
    template <typename ... Args >
        void  operator()(Args&&... args) const {}
};

/**
 * @brief Functor returns always the same number. The number has to be known at compile time
 *
 * @tparam T type of returned value
 * @tparam t return value
 */
template <typename T, T t>
    struct ReturnSomethingFunctor {
        /**
         * @brief operator
         *
         * @tparam Args
         * @param args
         *
         * @return
         */
        template <typename ... Args >
            T  operator()(Args&&... args) const {
                return t;
            }
    };

/**
 * @brief Functor returns always the same number (dynamic version).
 *
 * @tparam T type of returned value
 */
template <typename T>
    struct DynamicReturnSomethingFunctor {
        /**
         * @brief constructor
         *
         * @param t
         */
        DynamicReturnSomethingFunctor(T t) : m_t(t) {}

        /**
         * @brief operator
         *
         * @tparam Args
         * @param args
         *
         * @return
         */
        template <typename ... Args >
            T  operator()(Args&&... args) const {
                return m_t;
            }
    private:
        T m_t;
    };



/**
 * @brief make function for DynamicReturnSomethingFunctor
 *
 * @tparam T
 * @param t
 *
 * @return
 */
template <typename T>
DynamicReturnSomethingFunctor<T>
make_DynamicReturnSomethingFunctor(T t) {
    return DynamicReturnSomethingFunctor<T>(t);
}


/**
 * @brief functor returns its argument
 */
struct IdentityFunctor {
    template <typename Arg>
        auto  operator()(Arg&& arg) const ->
        Arg
        {
            return std::forward<Arg>(arg);
        }
};


/**
 * @brief functor return false
 */
struct ReturnFalseFunctor :
    public ReturnSomethingFunctor<bool, false> {};


/**
 * @brief functor return true
 */
struct ReturnTrueFunctor :
    public ReturnSomethingFunctor<bool, true> {};


/**
 * @brief functor returns 0
 */
struct ReturnZeroFunctor :
    public ReturnSomethingFunctor<int, 0> {};


/**
 * @brief functor returns 1
 */
struct ReturnOneFunctor :
    public ReturnSomethingFunctor<int, 1> {};

/**
 * @brief functors calls assert(false).
 */
struct AssertFunctor {
    template <typename ... Args >
        void  operator()(Args&&... args) const {
            assert(false);
        }
};

/**
 * @brief removes reference
 */
struct RemoveReference {
    template <typename T>
    T operator()(const T & t) const {
        return t;
    }
};

/**
 * @brief Counts numner of calls
 *
 * @tparam Functor
 * @tparam CounterType
 */
template <typename Functor, typename CounterType = int>
class CountingFunctorAdaptor {
public:
    /**
     * @brief Constructor
     *
     * @param cnt count reference
     * @param f functor
     */
    CountingFunctorAdaptor(Functor f, CounterType & cnt) : m_cnt(&cnt), m_functor(std::move(f)) {}

    /**
     * @brief increment the counter and checks if the given limit is reached. 
     *
     * @tparam Args
     *
     * @return 
     */
    template <typename... Args> 
    auto operator()(Args&&... args) -> decltype(std::declval<Functor>()(std::forward<Args>(args)...)) {
        ++(*m_cnt);
        return m_functor(std::forward<Args>(args)...);
    }

private:
    CounterType * m_cnt;
    Functor m_functor;
};

/**
 * @brief make function for CountingFunctorAdaptor
 *
 * @tparam CounterType
 * @tparam Functor
 * @param f
 * @param cnt
 *
 * @return 
 */
template <typename CounterType = int, typename Functor>
CountingFunctorAdaptor<Functor, CounterType>
make_CountingFunctorAdaptor(Functor f, CounterType & cnt) {
    return CountingFunctorAdaptor<Functor, CounterType>(std::move(f), cnt);
}



/**
 * @brief Adapts array as function, providing operator()().
 *
 * @tparam Array
 */
template <typename Array>
    class ArrayToFunctor{
        public:
            /**
             * @brief constructor
             *
             * @param array
             * @param offset
             */
            ArrayToFunctor(const Array & array, int offset = 0) :
                m_array(&array), m_offset(offset) {}

            typedef decltype(std::declval<const Array>()[0]) Value;

            /**
             * @brief operator
             *
             * @param a
             *
             * @return 
             */
            Value operator()(int a) const {
                return (*m_array)[a + m_offset];
            }

        private:
            const Array * m_array;
            int m_offset;
    };

/**
 * @brief make function for ArrayToFunctor
 *
 * @tparam Array
 * @param a
 * @param offset
 *
 * @return
 */
template <typename Array>
    ArrayToFunctor<Array> make_ArrayToFunctor(const Array &a, int offset = 0) {
        return ArrayToFunctor<Array>(a, offset);
    }


/**
 * @brief  Wrapper around a functor which adds assigmnent operator as well as default constructor.
 * Note, this struct might be dangerous. Using this struct correctly requires the underlying
 * functor to live at least as long as this wrapper.
 *
 * @tparam Functor
 */
template <typename Functor>
struct AssignableFunctor {
    /**
     * @brief constructor
     *
     * @param f
     */
   AssignableFunctor(Functor& f) : m_f(&f) {}
   AssignableFunctor() = default;
 
   /**
    * @brief assign operator
    *
    * @param f
    *
    * @return
    */
   AssignableFunctor& operator=(Functor& f) { m_f = f; }

   /**
    * @brief operator()
    *
    * @tparam Args
    *
    * @return 
    */
   template<typename ... Args>
   auto operator()(Args&& ... args) const ->
      decltype(std::declval<Functor>()(std::forward<Args>(args)...)) {
      return (*m_f)(std::forward<Args>(args)...);
   }

   private:
      const Functor* m_f;
};

/**
 * @brief make function for AssignableFunctor
 *
 * @tparam Functor
 * @param f
 *
 * @return
 */
template <typename Functor>
AssignableFunctor<Functor>
make_AssignableFunctor(Functor& f) {
   return AssignableFunctor<Functor>(f);
}

/**
 * @brief For given functor f, LiftIteratorFunctor provides operator()(Iterator iterator)
 * which returns f(*iter).
 *
 * @tparam Functor
 */
template <typename Functor>
struct LiftIteratorFunctor {
   LiftIteratorFunctor(Functor f) : m_f(std::move(f)) {}

   template <typename Iterator>
   auto operator()(Iterator iter) const -> decltype(std::declval<Functor>()(*iter)) {
      return m_f(*iter);
   }
   private:
      Functor m_f;
};

/**
 * @brief make function for LiftIteratorFunctor
 *
 * @tparam Functor
 * @param f
 *
 * @return
 */
template <typename Functor>
LiftIteratorFunctor<Functor>
make_LiftIteratorFunctor(Functor f) {
   return LiftIteratorFunctor<Functor>(f);
}

//************ The set of comparison functors *******************
//functors are equivalent to corresponding std functors (e.g. std::less) but are not templated

/**
 * @brief Greater functor
 */
struct Greater {
    template<class T>
        auto operator() (const T& x, const T& y) const ->
            decltype(x > y) {
            return x > y;
        };
};

/**
 * @brief Less functor
 */
struct Less {
    template<class T>
        auto operator() (const T& x, const T& y) const ->
            decltype(x < y){
            return x < y;
        };
};

/**
 * @brief GreaterEqual functor
 */
struct GreaterEqual {
    template<class T>
        auto operator() (const T& x, const T& y) const ->
            decltype(x >= y){
            return x >= y;
        };
};

/**
 * @brief LessEqual functor
 */
struct LessEqual {
    template<class T>
        auto operator() (const T& x, const T& y) const ->
            decltype(x <= y){
            return x <= y;
        };
};

/**
 * @brief EqualTo functor
 */
struct EqualTo {
    template<class T>
        auto operator() (const T& x, const T& y) const ->
            decltype(x == y){
            return x == y;
        };
};

/**
 * @brief NotEqualTo functor
 */
struct NotEqualTo {
    template<class T>
        auto operator() (const T& x, const T& y) const ->
            decltype(x != y){
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
            auto operator()(const T & left, const T & right) const ->
                decltype(std::declval<Compare>()(
                            std::declval<Functor>()(left),
                            std::declval<Functor>()(right)
                            )){
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

//Functor that scales another functor
template <typename Functor, typename ScaleType, typename ReturnType = ScaleType>
struct ScaleFunctor {

    ScaleFunctor(Functor f, ScaleType s) :
        m_f(std::move(f)), m_s(s) {}

    template <typename Arg>
    ReturnType operator()(Arg && arg) const {
        return m_s * m_f(std::forward<Arg>(arg));
    }

private:
    Functor m_f;
    ScaleType m_s;
};

template <typename ScaleType, typename ReturnType = ScaleType, typename Functor>
ScaleFunctor<Functor, ScaleType, ReturnType>
make_ScaleFunctor(Functor f, ScaleType s) {
    return ScaleFunctor<Functor, ScaleType, ReturnType>(f, s);
}

//****************************** This is set of functors representing standard boolean operation
//that is !, &&, ||. These are equivalent to standard std:: structs but are not templated
//(only operator() is templated)
struct Not {
    template <typename T>
    auto operator()(const T & b ) const -> decltype(!b) {
        return !b;
    }
};

struct Or {
    template <typename T>
    auto operator()(const T & left, const T & right) const ->
    decltype(left || right) {
        return left || right;
    }
};

struct And {
    template <typename T>
    auto operator()(const T & left, const T & right) const ->
        decltype(left && right){
        return left && right;
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
            auto  operator()(Args&&... args) const ->
                decltype(std::declval<Operator>()(
                            std::declval<FunctorLeft>()(std::forward<Args>(args)...),
                            std::declval<FunctorRight>()(std::forward<Args>(args)...)
                            )){
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
//allowing two compose functors  using
//standard logical operators


//Not
template <typename Functor>
    struct NotFunctor {
        NotFunctor(Functor functor= Functor()) :
            m_functor(functor) {}

        template <typename ... Args>
            auto operator()(Args&&... args) const ->
                decltype(std::declval<Functor>()(std::forward<Args>(args)...)) {
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
            public LiftBinaryOperatorFunctor<FunctorLeft, FunctorRight, NotEqualTo> {
        typedef LiftBinaryOperatorFunctor<FunctorLeft, FunctorRight, NotEqualTo> base;

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
#endif /* BOOST_FUNCTORS_HPP */
