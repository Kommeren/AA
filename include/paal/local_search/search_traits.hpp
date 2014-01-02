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
#include "paal/utils/type_functions.hpp"

namespace paal {
namespace local_search {

/**
 * @brief Traits class for SearchComponents
 *
 * @tparam SearchComponents
 */
template <typename SearchComponents> 
struct SearchComponentsTraits {
    typedef typename data_structures::ComponentTraits<SearchComponents>::template type<GetMoves>::type GetMovesT; 
    typedef typename data_structures::ComponentTraits<SearchComponents>::template type<Gain>::type GainT; 
    typedef typename data_structures::ComponentTraits<SearchComponents>::template type<Commit>::type CommitT; 
    typedef typename data_structures::ComponentTraits<SearchComponents>::template type<StepStopCondition>::type StepStopConditionT; 
};

/**
 * @brief metafunction returns move type in single_solution case
 *
 * @tparam SearchComponents
 * @tparam Solution
 */
template <typename SearchComponents, typename Solution> 
class Move {
    typedef typename SearchComponentsTraits<
                SearchComponents>::GetMovesT NG;
    typedef typename std::result_of<NG(Solution &)>::type MovesRange;
    typedef decltype(std::declval<MovesRange>().first) MoveIterator;
public:
   typedef typename std::iterator_traits<MoveIterator>::value_type type;
   typedef typename std::iterator_traits<MoveIterator>::reference reference;
};

/**
 * @brief metafunction returns move type in multi_solution case
 *
 * @tparam SearchComponents
 * @tparam Solution
 */
template <typename SearchComponents, typename Solution> 
class MultiMove {
    typedef typename SearchComponentsTraits<
                SearchComponents>::GetMovesT NG;
    typedef typename utils::CollectionToElem<Solution>::type Element;
    typedef typename std::result_of<NG(Solution &, Element &)>::type MovesRange;
    typedef decltype(std::declval<MovesRange>().first) MoveIterator;
public:
   typedef typename std::iterator_traits<MoveIterator>::value_type type;
   typedef typename std::iterator_traits<MoveIterator>::reference reference;
};

/**
 * @brief metafunction returns Fitness type in single_solution case
 *
 * @tparam SearchComponents
 * @tparam Solution
 */
template <typename SearchComponents, typename Solution> 
class Fitness {
    typedef typename SearchComponentsTraits<
                SearchComponents>::GainT Gain;
    typedef typename Move<SearchComponents, Solution>::type Move;
public:
    typedef typename utils::PureResultOf<Gain(Solution &, Move &)>::type type;
};


/**
 * @brief metafunction returns Fitness type in multi_solution case
 *
 * @tparam SearchComponents
 * @tparam Solution
 */
template <typename SearchComponents, typename Solution> 
class MultiFitness {
    typedef typename SearchComponentsTraits<
                SearchComponents>::GainT Gain;
    typedef typename MultiMove<SearchComponents, Solution>::type Move;
    typedef typename utils::CollectionToElem<Solution>::type SolutionElement;
public:
    typedef typename utils::PureResultOf<Gain(Solution &, SolutionElement &, Move &)>::type type;
};

} // local_search
} // paal
#endif /* SEARCH_TRAITS_HPP */
