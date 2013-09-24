/**
 * @file functors.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-04
 */
#ifndef FUNCTORS_HPP
#define FUNCTORS_HPP 
namespace paal {
namespace utils {

struct DoNothingFunctor {
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

struct ReturnSameFunctor {
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

template <typename Array> 
class Array2Functor{
public:
    Array2Functor(const Array & array, int offset = 0) : m_array(array), m_offset(offset) {}
    typedef decltype(std::declval<const Array>()[0]) Value;
    Value operator()(int a) const {return m_array[a + m_offset];}
private:
    const Array & m_array;
    int m_offset;
};

template <typename Array>
Array2Functor<Array> make_Array2Functor(const Array &a, int offset = 0) {
    return Array2Functor<Array>(a, offset);
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
