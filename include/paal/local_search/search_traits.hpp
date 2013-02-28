/**
 * @file paal/local_search/search_traits.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-19
 */
#ifndef SEARCH_TRAITS_HPP
#define SEARCH_TRAITS_HPP 

namespace paal {
namespace local_search {

template <typename SearchComponents> 
struct SearchComponentsTraits {
    typedef puretype(std::declval<SearchComponents>().getNeighborhood()) GetNeighborhood; 
    typedef puretype(std::declval<SearchComponents>().gain()) Gain; 
    typedef puretype(std::declval<SearchComponents>().updateSolution()) UpdateSolution; 
    typedef puretype(std::declval<SearchComponents>().stopCondition()) StopCondition; 
};

template <typename SearchComponents, typename Solution> 
class Update {
    typedef typename SearchComponentsTraits<
                SearchComponents>::GetNeighborhood NG;
    typedef decltype(std::declval<NG>()(
                                std::declval<Solution &>()
                                ).first) UpdateIterator;
public:
   typedef typename utils::IterToElem<UpdateIterator>::type type;

};

template <typename SearchComponents, typename Solution> 
class MultiUpdate {
    typedef typename SearchComponentsTraits<
                SearchComponents>::GetNeighborhood NG;
    typedef typename utils::SolToElem<Solution>::type Element;
    typedef decltype(std::declval<NG>()(
                                std::declval<Solution &>(),
                                std::declval<Element &>()
                                ).first) UpdateIterator;
public:
   typedef typename utils::IterToElem<UpdateIterator>::type type;

};

} // local_search
} // paal
#endif /* SEARCH_TRAITS_HPP */
