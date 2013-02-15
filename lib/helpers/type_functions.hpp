/**
 * @file type_functions.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef __TYPE_FUNCITONS__
#define __TYPE_FUNCITONS__

#include <type_traits>
#include <utility>

namespace paal {
namespace helpers {

#define HAS_MEMBER_CLASS(member) \
template <typename T> \
class has_##member {  \
    private: \
        template <typename U, U> \
            class check \
            { }; \
\
        template <typename C> \
            static char f(check<decltype(std::declval<C>().member()) (C::*)() const, &C::member>*); \
        \
        template <typename C> \
            static char f(check< decltype(std::declval<C>().member()) (C::*)(), &C::member>*); \
\
        template <typename C> \
            static long f(...); \
 \
    public: \
        static  const bool value = (sizeof(f<typename std::decay<T>::type>(0)) == sizeof(char));\
}; 

//HAS_MEMBER_CLASS(begin)
//HAS_MEMBER_CLASS(cbegin)
HAS_MEMBER_CLASS(get)

template <typename Iter> struct IterToElem {
    typedef typename std::decay<decltype(*std::declval<Iter>())>::type type; 
};

/*template <typename Solution, typename Enable = void> struct SolToIter;


template <typename Solution> struct SolToIter<Solution, 
         typename std::enable_if< has_begin<Solution>::value >::type > {
    typedef decltype(std::declval<Solution>().begin()) type; 
};

template <typename Solution> struct SolToIter<Solution, 
         typename std::enable_if< has_cbegin<Solution>::value && ! has_begin<Solution>::value >::type > {
  typedef decltype(std::declval<Solution>().cbegin()) type; 
  };*/

template <typename Solution> struct SolToIter {
    typedef decltype(std::declval<Solution>().begin()) type; 
};

template <typename Solution> struct SolToConstIter {
    typedef decltype(std::declval<const Solution>().begin()) type; 
};


template <typename Solution> struct SolToElem {
  typedef typename IterToElem<typename SolToIter<Solution>::type>::type type; 
  };


template <typename T, int k> struct kTuple {
    typedef decltype(std::tuple_cat(std::declval<std::tuple<T>>(), 
                                    std::declval<typename  kTuple<T, k-1>::type>())) type; 
};

template <typename T> struct kTuple<T, 1> {
    typedef std::tuple<T> type;
};

} //helpers
} //paal

#endif //__TYPE_FUNCITONS__
