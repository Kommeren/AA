/**
 * @file paal/local_search/search_traits.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-19
 */
#ifndef SEARCH_TRAITS_HPP
#define SEARCH_TRAITS_HPP

#include "paal/data_structures/component_traits.hpp"
#include "paal/local_search/search_components.hpp"

namespace paal {
namespace local_search {

template <typename SearchComponents> 
struct SearchComponentsTraits {
    typedef typename data_structures::ComponentTraits<SearchComponents>::template type<GetNeighborhood>::type GetNeighborhoodT; 
    typedef typename data_structures::ComponentTraits<SearchComponents>::template type<Gain>::type GainT; 
    typedef typename data_structures::ComponentTraits<SearchComponents>::template type<UpdateSolution>::type UpdateSolutionT; 
    typedef typename data_structures::ComponentTraits<SearchComponents>::template type<StopCondition>::type StopConditionT; 
};

template <typename SearchComponents, typename Solution> 
class Update {
    typedef typename SearchComponentsTraits<
                SearchComponents>::GetNeighborhoodT NG;
    typedef decltype(std::declval<NG>()(
                                std::declval<Solution &>()
                                ).first) UpdateIterator;
public:
   typedef typename utils::IterToElem<UpdateIterator>::type type;

};

template <typename SearchComponents, typename Solution> 
class MultiUpdate {
    typedef typename SearchComponentsTraits<
                SearchComponents>::GetNeighborhoodT NG;
    typedef typename utils::SolToElem<Solution>::type Element;
    typedef decltype(std::declval<NG>()(
                                std::declval<Solution &>(),
                                std::declval<Element &>()
                                ).first) UpdateIterator;
public:
   typedef typename utils::IterToElem<UpdateIterator>::type type;

};

template <typename SearchComponents, typename Solution> 
class Fitness {
    typedef typename SearchComponentsTraits<
                SearchComponents>::GainT Gain;
    typedef typename Update<SearchComponents, Solution>::type Update;
public:
    typedef decltype(std::declval<Gain>()(
                        std::declval<Solution &>(), std::declval<Update &>())
                    ) type;
};


template <typename SearchComponents, typename Solution> 
class MultiFitness {
    typedef typename SearchComponentsTraits<
                SearchComponents>::GainT Gain;
    typedef typename MultiUpdate<SearchComponents, Solution>::type Update;
    typedef typename utils::SolToElem<Solution>::type SolutionElement;
public:
    typedef decltype(std::declval<Gain>()(
                        std::declval<Solution &>(), std::declval<SolutionElement>(), std::declval<Update &>())
                    ) type;
};

} // local_search
} // paal
#endif /* SEARCH_TRAITS_HPP */
