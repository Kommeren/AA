/**
 * @file local_search.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef LOCAL_SEARCH_SINGLE_SOLUTION_HPP
#define LOCAL_SEARCH_SINGLE_SOLUTION_HPP

#include <utility>
#include <algorithm>
#include <functional>

#include <boost/fusion/include/vector.hpp>
#include <boost/range/algorithm/max_element.hpp>

#include "local_search_concepts.hpp"
#include "paal/utils/fusion_algorithms.hpp"
#include "paal/data_structures/components/component_traits.hpp"

namespace paal {
namespace local_search {

/**
 * @brief this predicates returns true if there is a move with positive gain
 *        and the commit was successful
 */
struct find_positive_predicate {

    /**
     * @brief operator()
     *
     * @tparam componentsAndSolution
     * @param compsAndSol
     *
     * @return
     */
    template <typename componentsAndSolution>
    bool operator()(componentsAndSolution & compsAndSol) const {
        auto & solution = compsAndSol.second;
        auto & comps = compsAndSol.first;

        using MoveRef = typename move_type<puretype(comps), puretype(solution)>::reference;
//        using Delta = typename fitness<puretype(comps), puretype(solution)>::type;
//        TODO use above in comparison

        decltype(comps.template call<GetMoves>(solution))
            adjustmentSet = comps.template call<GetMoves>(solution);

        for(MoveRef move : boost::make_iterator_range(adjustmentSet)) {
            if(comps.template call<Gain>(solution, move) > 0) {
                if(comps.template call<Commit>(solution, move)) {
                    return true;
                }
            }
        }

        return false;
    }
};



/**
 * @brief This strategy uses find_positive_predicate as stop condition
 */
struct choose_first_better_strategy {
    /**
     * @brief operator()
     *
     * @tparam SearchJoin
     * @param join
     *
     * @return
     */
    template <typename SearchJoin>
        bool operator()(SearchJoin & join) const {
            return m_satisfy(m_pred, join);
        }
    private:
    find_positive_predicate m_pred;
    data_structures::Satisfy m_satisfy;
};


/**
 * @brief functor used in fold on StaticLazyJoin in order to find the most improving move.
 */
struct max_functor {

    /**
     * @brief operator()
     *
     * @tparam componentsAndSolution
     * @tparam AccumulatorFunctor
     * @tparam AccumulatorData
     * @tparam Continuation
     * @param compsAndSol
     * @param accumulatorFunctor
     * @param accumulatorData
     * @param continuation
     *
     * @return
     */
    template <typename componentsAndSolution,
              typename AccumulatorFunctor,
              typename AccumulatorData,
              typename Continuation>
                 bool operator()(
                        componentsAndSolution & compsAndSol,
                        AccumulatorFunctor accumulatorFunctor,
                        AccumulatorData accumulatorData,
                        Continuation continuation
                    ) const {
        auto & comps = compsAndSol.first;
        auto  & solution = compsAndSol.second;

        using Move = typename move_type<puretype(comps), puretype(solution)>::value_type;
        using MoveRef = typename move_type<puretype(comps), puretype(solution)>::reference;

        decltype(comps.template call<GetMoves>(solution))
            adjustmentSet = comps.template call<GetMoves>(solution);

        if(boost::empty(adjustmentSet)) {
            return continuation(accumulatorFunctor, accumulatorData);
        }

        Move maxMove = *std::begin(adjustmentSet);
        auto maxGain = comps.template call<Gain>(solution, maxMove);


        for(MoveRef move : boost::make_iterator_range(++std::begin(adjustmentSet), std::end(adjustmentSet))) {
            auto gain = comps.template call<Gain>(solution, move);
            if(gain > maxGain) {
                maxMove = move;
                maxGain = gain;
            }
        }

        if(maxGain > accumulatorData) {
            auto commit = std::bind(std::ref(comps.template get<Commit>()), std::ref(solution), maxMove);
            return continuation(commit, maxGain);
        } else {
            return continuation(accumulatorFunctor, accumulatorData);
        }

    }

};

/**
 * @brief This strategy chooses the best possible move and applies it to the solution
 */
struct steepest_slope_strategy {
    /**
     * @brief operator()
     *
     * @tparam SearchJoin
     * @param join
     *
     * @return
     */
    template <typename SearchJoin>
    bool operator()(SearchJoin & join) const {
        return m_fold(m_fun
                , utils::always_false{}
                , 0
                , join
                );
    }
private:
    max_functor m_fun;
    data_structures::polymorfic_fold m_fold;
};

namespace detail {

template <typename Solution,
          typename... search_componentsPack>
struct local_search_consepts;

template <typename Solution,
          typename search_components, typename... search_componentsPack>
struct local_search_consepts<Solution, search_components, search_componentsPack...> :
    public local_search_consepts<Solution, search_componentsPack...>{
    BOOST_CONCEPT_ASSERT((concepts::search_components<search_components, Solution>));
};

template <typename Solution>
struct local_search_consepts<Solution>{

    /**
     * @brief dummy member to avoid warnings
     */
    void use() const {}
    };

} //!detail

/**
 * @brief local search simple solution
 *
 * @param solution the initial solution which going to be possibly improved by local_search
 * @param psa post search action
 * @param gsc global stop condition
 * @param components
 *
 * @return true if the solution is improved
 */
template <typename SearchStrategy,
          typename ContinueOnSuccess,
          typename ContinueOnFail,
          typename Solution,
          typename... components>
bool local_search(
            Solution & solution
          , SearchStrategy searchStrategy
          , ContinueOnSuccess succ
          , ContinueOnFail fail
          , components... comps) {
    detail::local_search_consepts<Solution, components...> concepts;
    concepts.use();


    using search_components_v = boost::fusion::vector<std::pair<components, Solution &>...>;

    search_components_v search_comps(
            std::pair<components, Solution &>(std::move(comps), solution)...);

    bool success{false}, ret{false};

    while((success = searchStrategy(search_comps)) || fail(solution)) {
        ret |= success;
        if(success && !succ(solution)) {
            break;
        }
    }
    return success | ret;
}

/**
 * @brief
 *
 * @param solution the initial solution which going to be possibly improved by local_search
 * @param components
 *
 * @return true if the solution is improved
 */
template <typename Solution,
          typename... components>
bool local_search_simple(Solution & solution, components... comps) {
    return local_search(solution, choose_first_better_strategy{},
            utils::always_true{}, utils::always_false{}, std::move(comps)...);
}




} // local_search
} // paal

#endif // LOCAL_SEARCH_SINGLE_SOLUTION_HPP
