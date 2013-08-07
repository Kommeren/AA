/**
 * @file paal/local_search/search_traits.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-19
 */
#ifndef SEARCH_TRAITS_HPP
#define SEARCH_TRAITS_HPP

#include "paal/data_structures/components/component_traits.hpp"
#include "paal/local_search/search_components.hpp"

namespace paal {
namespace local_search {

template <typename SearchComponents> 
struct SearchComponentsTraits {
    typedef typename data_structures::ComponentTraits<SearchComponents>::template type<GetMoves>::type GetMovesT; 
    typedef typename data_structures::ComponentTraits<SearchComponents>::template type<Gain>::type GainT; 
    typedef typename data_structures::ComponentTraits<SearchComponents>::template type<Commit>::type CommitT; 
    typedef typename data_structures::ComponentTraits<SearchComponents>::template type<StopCondition>::type StopConditionT; 
};

template <typename SearchComponents, typename Solution> 
class Move {
    typedef typename SearchComponentsTraits<
                SearchComponents>::GetMovesT NG;
    typedef decltype(std::declval<NG>()(
                                std::declval<Solution &>()
                                ).first) MoveIterator;
public:
   typedef typename utils::IterToElem<MoveIterator>::type type;

};

template <typename SearchComponents, typename Solution> 
class MultiMove {
    typedef typename SearchComponentsTraits<
                SearchComponents>::GetMovesT NG;
    typedef typename utils::SolToElem<Solution>::type Element;
    typedef decltype(std::declval<NG>()(
                                std::declval<Solution &>(),
                                std::declval<Element &>()
                                ).first) MoveIterator;
public:
   typedef typename utils::IterToElem<MoveIterator>::type type;

};

template <typename SearchComponents, typename Solution> 
class Fitness {
    typedef typename SearchComponentsTraits<
                SearchComponents>::GainT Gain;
    typedef typename Move<SearchComponents, Solution>::type Move;
public:
    typedef decltype(std::declval<Gain>()(
                        std::declval<Solution &>(), std::declval<Move &>())
                    ) type;
};


template <typename SearchComponents, typename Solution> 
class MultiFitness {
    typedef typename SearchComponentsTraits<
                SearchComponents>::GainT Gain;
    typedef typename MultiMove<SearchComponents, Solution>::type Move;
    typedef typename utils::SolToElem<Solution>::type SolutionElement;
public:
    typedef decltype(std::declval<Gain>()(
                        std::declval<Solution &>(), std::declval<SolutionElement>(), std::declval<Move &>())
                    ) type;
};

} // local_search
} // paal
#endif /* SEARCH_TRAITS_HPP */
