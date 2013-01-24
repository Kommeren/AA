#ifndef __TYPE_FUNCITONS__
#define __TYPE_FUNCITONS__

#include <type_traits>
#include <utility>

template <typename T>
class has_begin
{
    private:
        template <typename U, U>
            class check
            { };

        template <typename C>
            static char f(check<decltype(std::declval<C>().begin()) (C::*)() const, &C::begin>*);
        
        template <typename C>
            static char f(check<decltype(std::declval<C>().begin()) (C::*)(), &C::begin>*);

        template <typename C>
            static long f(...);

    public:
        static const bool value = (sizeof(f<typename std::decay<T>::type>(0)) == sizeof(char));
};

template <typename T>
class has_cbegin
{
    private:
        template <typename U, U>
            class check
            { };

        template <typename C>
            static char f(check<decltype(std::declval<C>().cbegin()) (C::*)() const, &C::cbegin>*);
        
        template <typename C>
            static char f(check<decltype(std::declval<C>().cbegin()) (C::*)(), &C::cbegin>*);

        template <typename C>
            static long f(...);

    public:
        static const bool value = (sizeof(f<typename std::decay<T>::type>(0)) == sizeof(char));
};

#define HAS_MEMBER(member) \\
template <typename T> \\
class has_#member {  \\
    private: \\
        template <typename U, U> \\
            class check \\
            { }; \\
\\
        template <typename C> \\
            static char f(check<decltype(std::declval<C>().#member()) (C::*)() const, &C::#member>*); \\
        \\
        template <typename C> \\
            static char f(check< decltype(std::declval<C>().#member()) (C::*)(), &C::#member>*); \\
\\
        template <typename C> \\
            static long f(...); \\
 \\
    public: \\
        static  const bool value = (sizeof(f<typename std::decay<T>::type>(0)) == sizeof(char));\\
}; \\





template <typename Iter> struct IterToElem {
    typedef typename std::decay<decltype(*std::declval<Iter>())>::type type; 
};

template <typename Solution, typename Enable = void> struct SolToIter;


template <typename Solution> struct SolToIter<Solution, 
         typename std::enable_if< has_begin<Solution>::value >::type > {
    typedef decltype(std::declval<Solution>().begin()) type; 
};

template <typename Solution> class SolToIter<Solution, 
         typename std::enable_if< has_cbegin<Solution>::value && ! has_begin<Solution>::value >::type > {
  typedef decltype(std::declval<Solution>().cbegin()) type; 
  };


template <typename Solution> struct SolToElem {
  typedef typename IterToElem<typename SolToIter<Solution>::type>::type type; 
  };

#endif //__TYPE_FUNCITONS__
