/**
 * @file functors.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-04
 */
#ifndef FUNCTORS_HPP
#define FUNCTORS_HPP 
#include <cassert>

namespace paal {
namespace utils {

struct SkipFunctor {
    template <typename ... Args > 
    void  operator()(Args&&... args) const {}
};

template <typename T, T t>
struct ReturnSomethingFunctor {
    template <typename ... Args > 
    T  operator()(Args&&... args) const {
        return t;
    } 
};

struct IdentityFunctor {
    template <typename Arg> 
    auto  operator()(Arg&& arg) const ->
    Arg
    { 
        return std::forward<Arg>(arg);
    }
};

struct ReturnFalseFunctor : 
    public ReturnSomethingFunctor<bool, false> {};

struct ReturnTrueFunctor : 
    public ReturnSomethingFunctor<bool, true> {};

struct ReturnZeroFunctor :
    public ReturnSomethingFunctor<int, 0> {};

struct AssertFunctor {
    template <typename ... Args > 
    void  operator()(Args&&... args) const {
        assert(false);
    } 
};

/**    
 * @brief Adapts array as function, providing operator()().      
 *       
 * @tparam Array input array type 
*/

template <typename Array> 
class ArrayToFunctor{
public:
    ArrayToFunctor(const Array & array, int offset = 0) : m_array(array), m_offset(offset) {}
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

struct Greater{
    template<class T>bool operator() (const T& x,const T& y) const{return x>y;};
};

struct Less{
    template<class T>bool operator() (const T& x,const T& y) const{return x<y;};
};

template <typename Functor,typename Compare=Less>
struct FunctorToComparator {
    FunctorToComparator(Functor f,Compare c=Compare()) : m_f(f),m_c(c){}

    template <typename T>
    bool operator()(const T & left, const T & right) /*const*/ {
        return m_c(m_f(left), m_f(right));
    }

private:
    Functor m_f;
    Compare m_c;
};


template <typename Functor,typename Compare = Less>
FunctorToComparator<Functor,Compare>
make_FunctorToComparator(Functor functor,Compare compare=Compare()) {
    return FunctorToComparator<Functor,Compare>(functor,compare);
};
template <typename Functor>
struct FunctorToOutputIterator {
    FunctorToOutputIterator(Functor functor= Functor()) :
        m_functor(functor) {}
    
    template <typename Arg> 
    decltype(std::declval<Functor>()(std::declval<Arg>()))
    operator=(Arg&& arg) {
        return m_functor(std::forward<Arg>(arg));
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
    return FunctorToOutputIterator<Functor>(functor);
}



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
    return NotFunctor<Functor>(functor);
}

template <typename FunctorLeft, typename FunctorRight>
struct OrFunctor {
    OrFunctor(FunctorLeft left = FunctorLeft(), FunctorRight right = FunctorRight()) :
        m_left(left), m_right(right) {}
    
    template <typename ... Args> 
    bool  operator()(Args&&... args) const {
        return m_left(std::forward<Args>(args)...) || m_right(std::forward<Args>(args)...);
    }

private:
    FunctorLeft m_left;
    FunctorRight m_right;
};

template <typename FunctorLeft, typename FunctorRight>
OrFunctor<FunctorLeft, FunctorRight>
make_OrFunctor(FunctorLeft left, FunctorRight right) {
    return OrFunctor<FunctorLeft, FunctorRight>(left, right);
}

template <typename FunctorLeft, typename FunctorRight>
struct AndFunctor {
    AndFunctor(FunctorLeft left = FunctorLeft(), FunctorRight right = FunctorRight()) :
        m_left(left), m_right(right) {}
    
    template <typename ... Args> 
    bool  operator()(Args&&... args) const {
        return m_left(std::forward<Args>(args)...) && m_right(std::forward<Args>(args)...);
    }

private:
    FunctorLeft m_left;
    FunctorRight m_right;
};

template <typename FunctorLeft, typename FunctorRight>
AndFunctor<FunctorLeft, FunctorRight>
make_AndFunctor(FunctorLeft left, FunctorRight right) {
    return AndFunctor<FunctorLeft, FunctorRight>(left, right);
}

template <typename FunctorLeft, typename FunctorRight>
struct XorFunctor {
    XorFunctor(FunctorLeft left = FunctorLeft(), FunctorRight right = FunctorRight()) :
        m_left(left), m_right(right) {}
    
    template <typename ... Args> 
    bool  operator()(Args&&... args) const {
        return m_left(std::forward<Args>(args)...) != m_right(std::forward<Args>(args)...);
    }

private:
    FunctorLeft m_left;
    FunctorRight m_right;
};

template <typename FunctorLeft, typename FunctorRight>
XorFunctor<FunctorLeft, FunctorRight>
make_XorFunctor(FunctorLeft left, FunctorRight right) {
    return XorFunctor<FunctorLeft, FunctorRight>(left, right);
}




} //utils
} //paal
#endif /* FUNCTORS_HPP */
