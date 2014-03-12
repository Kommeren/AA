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
#include "paal/data_structures/static_lazy_join.hpp"
#include "paal/data_structures/components/component_traits.hpp"

namespace paal {
namespace local_search {

/**
 * @brief this predicates returns true if there is a move with positive gain
 *        and the commit was succesfull
 */
struct FindPositivePredicate {

    /**
     * @brief operator()
     *
     * @tparam ComponentsAndSolution
     * @param compsAndSol
     *
     * @return
     */
    template <typename ComponentsAndSolution>
    bool operator()(ComponentsAndSolution & compsAndSol) const {
        auto & solution = compsAndSol.second;
        auto & comps = compsAndSol.first;

        using MoveRef = typename MoveType<puretype(comps), puretype(solution)>::reference;

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
 * @brief This strategy uses FindPositivePredicate as stop condition
 */
struct ChooseFirstBetterStrategy {
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
    FindPositivePredicate m_pred;
    data_structures::Satisfy m_satisfy;
};


/**
 * @brief functor used in fold on StaticLazyJoin in order to find the most improving move.
 */
struct MaxFunctor {

    /**
     * @brief operator()
     *
     * @tparam ComponentsAndSolution
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
    template <typename ComponentsAndSolution,
              typename AccumulatorFunctor,
              typename AccumulatorData,
              typename Continuation>
                 bool operator()(
                        ComponentsAndSolution & compsAndSol,
                        AccumulatorFunctor accumulatorFunctor,
                        AccumulatorData accumulatorData,
                        Continuation continuation
                    ) const {
        auto & comps = compsAndSol.first;
        auto  & solution = compsAndSol.second;

        using Move = typename MoveType<puretype(comps), puretype(solution)>::value_type;
        using MoveRef = typename MoveType<puretype(comps), puretype(solution)>::reference;

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
struct SteepestSlopeStrategy {
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
                , utils::ReturnFalseFunctor{}
                , 0
                , join
                );
    }
private:
    MaxFunctor m_fun;
    data_structures::PolymorficFold m_fold;
};

namespace detail {

template <typename Solution,
          typename... SearchComponentsPack>
struct LocalSearchConsepts;

template <typename Solution,
          typename SearchComponents, typename... SearchComponentsPack>
struct LocalSearchConsepts<Solution, SearchComponents, SearchComponentsPack...> :
    public LocalSearchConsepts<Solution, SearchComponentsPack...>{
    BOOST_CONCEPT_ASSERT((concepts::SearchComponents<SearchComponents, Solution>));
};

template <typename Solution>
struct LocalSearchConsepts<Solution>{

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
          typename PostSearchAction,
          typename GlobalStopCondition,
          typename Solution,
          typename... Components>
bool local_search(
            Solution & solution
          , SearchStrategy searchStrategy
          , PostSearchAction psa
          , GlobalStopCondition gsc
          , Components... components) {
    detail::LocalSearchConsepts<Solution, Components...> concepts;
    concepts.use();


    using SearchComponentsVector = boost::fusion::vector<std::pair<Components, Solution &>...>;

    SearchComponentsVector searchComponentsVector(
            std::pair<Components, Solution &>(std::move(components), solution)...);

    if(!searchStrategy(searchComponentsVector)) {
        return false;
    }

    if(!gsc(solution)) {
        psa(solution);
        while(searchStrategy(searchComponentsVector) && !gsc(solution)) {
            psa(solution);
        }
    }
    return true;
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
          typename... Components>
bool local_search_simple(Solution & solution, Components... components) {
    return local_search(solution, ChooseFirstBetterStrategy{},
            utils::SkipFunctor(), utils::ReturnFalseFunctor(), std::move(components)...);
}




} // local_search
} // paal

#endif // LOCAL_SEARCH_SINGLE_SOLUTION_HPP
