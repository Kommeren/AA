/**
 * @file search_traits.hpp
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
    typedef puretype(std::declval<SearchComponents>().getNeighborhoodGetter()) NeighborhoodGetter; 
    typedef puretype(std::declval<SearchComponents>().getImproveChecker()) ImproveChecker; 
    typedef puretype(std::declval<SearchComponents>().getSolutionUpdater()) SolutionUpdater; 
    typedef puretype(std::declval<SearchComponents>().getStopCondition()) StopCondition; 
};

template <typename SearchComponents, typename Solution> 
class Update {
    typedef typename SearchComponentsTraits<
                SearchComponents>::NeighborhoodGetter NG;
    typedef decltype(std::declval<NG>().get(
                                std::declval<Solution &>()
                                ).first) UpdateIterator;
public:
   typedef typename helpers::IterToElem<UpdateIterator>::type type;

};

template <typename SearchComponents, typename Solution> 
class MultiUpdate {
    typedef typename SearchComponentsTraits<
                SearchComponents>::NeighborhoodGetter NG;
    typedef typename helpers::SolToElem<Solution>::type Element;
    typedef decltype(std::declval<NG>().get(
                                std::declval<Solution &>(),
                                std::declval<Element &>()
                                ).first) UpdateIterator;
public:
   typedef typename helpers::IterToElem<UpdateIterator>::type type;

};

} // local_search
} // paal
#endif /* SEARCH_TRAITS_HPP */
