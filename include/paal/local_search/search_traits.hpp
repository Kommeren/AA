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
 * @brief Traits class for search_components
 *
 * @tparam search_components
 */
template <typename search_components>
struct search_components_traits {
    typedef typename data_structures::component_traits<search_components>::template type<get_moves>::type get_movesT;
    typedef typename data_structures::component_traits<search_components>::template type<Gain>::type GainT;
    typedef typename data_structures::component_traits<search_components>::template type<Commit>::type CommitT;
};

/**
 * @brief metafunction returns move type in single_solution case
 *
 * @tparam search_components
 * @tparam Solution
 */
template <typename search_components, typename Solution>
class move_type {
    typedef typename search_components_traits<
                search_components>::get_movesT NG;
    typedef typename std::remove_reference<
        typename std::result_of<NG(Solution &)>::type>::type MovesRange;
    typedef typename boost::range_iterator<MovesRange>::type MoveIterator;
public:
   typedef typename std::iterator_traits<MoveIterator>::value_type value_type;
   typedef typename std::iterator_traits<MoveIterator>::reference reference;
};

/**
 * @brief metafunction returns move type in multi_solution case
 *
 * @tparam search_components
 * @tparam Solution
 */
template <typename search_components, typename Solution>
class multi_move {
    typedef typename search_components_traits<
                search_components>::get_movesT NG;
    typedef typename utils::collection_to_elem<Solution>::type Element;
    typedef typename std::result_of<NG(Solution &, Element &)>::type MovesRange;
    typedef typename boost::range_iterator<MovesRange>::type MoveIterator;
public:
   typedef typename std::iterator_traits<MoveIterator>::value_type type;
   typedef typename std::iterator_traits<MoveIterator>::reference reference;
};

/**
 * @brief metafunction returns Fitness type in single_solution case
 *
 * @tparam search_components
 * @tparam Solution
 */
template <typename search_components, typename Solution>
class Fitness {
    typedef typename search_components_traits<
                search_components>::GainT Gain;
    typedef typename move_type<search_components, Solution>::type Move;
public:
    typedef typename utils::pure_result_of<Gain(Solution &, Move &)>::type type;
};


/**
 * @brief metafunction returns Fitness type in multi_solution case
 *
 * @tparam search_components
 * @tparam Solution
 */
template <typename search_components, typename Solution>
class multi_fitness {
    typedef typename search_components_traits<
                search_components>::GainT Gain;
    typedef typename multi_move<search_components, Solution>::type Move;
    typedef typename utils::collection_to_elem<Solution>::type SolutionElement;
public:
    typedef typename utils::pure_result_of<Gain(Solution &, SolutionElement &, Move &)>::type type;
};

} // local_search
} // paal
#endif /* SEARCH_TRAITS_HPP */
