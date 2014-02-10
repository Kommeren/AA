/**
 * @file local_search_multi_solution.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef LOCAL_SEARCH_MULTI_SOLUTION_HPP
#define LOCAL_SEARCH_MULTI_SOLUTION_HPP

#include <utility>
#include <algorithm>
#include <functional>

#include "local_search_multi_solution_concepts.hpp"
#include "paal/local_search/local_search.hpp"
#include "paal/local_search/search_traits.hpp"
#include "paal/utils/type_functions.hpp"

namespace paal {
namespace local_search {

namespace search_strategies {
    /**
     * @brief In this strategy, we iterate through moves until we find the move with positive gain.
     */
    class ChooseFirstBetter;
    /**
     * @brief In this strategy, we iterate through all moves and chose one with the largest gain. 
     */
    class SteepestSlope;
}

namespace detail {

template <typename Solution>
class LocalSearchStepMultiSolutionBase {
protected:    
    typedef typename utils::CollectionToElem<Solution>::type SolutionElement;

    LocalSearchStepMultiSolutionBase(Solution & solution) : 
        m_solution(solution) {}

public:
    Solution & getSolution() {
        return m_solution;
    }

protected:

    Solution & m_solution;
};

template <typename Solution,
          typename SearchStrategy,
          typename... MultiSearchComponentsRest>
class LocalSearchStepMultiSolution; 

template <typename Solution,
          typename MultiSearchComponents,
          typename... MultiSearchComponentsRest>
class LocalSearchStepMultiSolution<Solution, search_strategies::ChooseFirstBetter, MultiSearchComponents, MultiSearchComponentsRest...> : 
        public LocalSearchStepMultiSolution<Solution, search_strategies::ChooseFirstBetter, MultiSearchComponentsRest...> {
    BOOST_CONCEPT_ASSERT((local_search_concepts::MultiSearchComponents<MultiSearchComponents, Solution>));
    typedef typename MultiMove<MultiSearchComponents, Solution>::reference MoveRef;
    
    typedef LocalSearchStepMultiSolution<Solution, search_strategies::ChooseFirstBetter, MultiSearchComponentsRest...> base;
    typedef typename std::iterator_traits<typename utils::CollectionToIter<Solution>::type>::reference SolElementRef;
public:

    LocalSearchStepMultiSolution(
            Solution & solution, 
            MultiSearchComponents sc, 
            MultiSearchComponentsRest... rest) : 
        base(solution, std::move(rest)...), m_searchComponents(std::move(sc)) {}
    
    LocalSearchStepMultiSolution(
            Solution & solution) : 
        base(solution), m_searchComponents(MultiSearchComponents()) {}

    typedef typename base::SolutionElement SolutionElement;
    using base::m_solution;
    
    MultiSearchComponents & getSearchComponents() {
        return m_searchComponents;
    }

    /**
     * @brief tries to improve the solution using local search 
     *
     * @return true if the solution is improved false otherwise 
     */
    bool search() {

        for(SolElementRef r : m_solution) {
            auto adjustmentSet = call<GetMoves>(m_solution, r);
            for(MoveRef move : boost::make_iterator_range(adjustmentSet)) {
                if(call<Gain>(m_solution, r, move) > 0) {
                    call<Commit>(m_solution, r, move);
                    return true;
                }
            }
        }
          
        return base::search();
    }
          
private:
    template <typename Action, typename... Args> 
    auto call(Args&&... args) -> 
    decltype(std::declval<MultiSearchComponents>().template call<Action>(std::forward<Args>(args)...)){
        return m_searchComponents.template call<Action>(std::forward<Args>(args)...);
    }
    
    MultiSearchComponents m_searchComponents;
};

template <typename Solution>
class LocalSearchStepMultiSolution<Solution, search_strategies::ChooseFirstBetter> : 
        public LocalSearchStepMultiSolutionBase<Solution> {
      
    typedef LocalSearchStepMultiSolutionBase<Solution> base;
    typedef typename std::iterator_traits<typename utils::CollectionToIter<Solution>::type>::reference SolElementRef;
    template <typename Sol, typename SS, typename... MSC>
    friend class LocalSearchStepMultiSolution;
//TODO change after redesign
protected:
    LocalSearchStepMultiSolution(Solution & solution) : 
        base(solution) {}
public:
    using base::getSolution;
    bool search() {
        return false;
    }
};


template <typename Solution, 
          typename MultiSearchComponents, 
          typename... MultiSearchComponentsRest> 
class LocalSearchStepMultiSolution<
        Solution, 
        search_strategies::SteepestSlope, 
        MultiSearchComponents, 
        MultiSearchComponentsRest...> : 
      public LocalSearchStepMultiSolution<Solution, search_strategies::SteepestSlope, MultiSearchComponentsRest...> { 

    BOOST_CONCEPT_ASSERT((local_search_concepts::MultiSearchComponents<MultiSearchComponents, Solution>));
    typedef typename MultiMove<MultiSearchComponents, Solution>::reference MoveRef;
    typedef typename MultiMove<MultiSearchComponents, Solution>::type Move;
    typedef LocalSearchStepMultiSolution<Solution, search_strategies::SteepestSlope, MultiSearchComponentsRest...> base;
public:

    LocalSearchStepMultiSolution(Solution & solution) : 
        base(solution) {}
    
    LocalSearchStepMultiSolution(Solution & solution, MultiSearchComponents sc,  MultiSearchComponentsRest... rest) : 
        base(solution, rest...), m_searchComponents(std::move(sc)) {}

    typedef typename base::SolutionElement SolutionElement;
    typedef typename utils::CollectionToIter<Solution>::type SolutionIterator;
    typedef typename MultiFitness<MultiSearchComponents, Solution>::type Fitness;
    using base::m_solution;
    
    MultiSearchComponents & getSearchComponents() {
        return m_searchComponents;
    }

    /**
     * @brief tries to improve the solution using local search 
     *
     * @return true if the solution is improved false otherwise 
     */

    bool search() {
        return base::searchAndPassBest(best());
    }

protected:
    template <typename Best>
    bool searchAndPassBest(Best & b) {
            auto b2 = best();
            if(std::get<0>(b) > std::get<0>(b2)) {
            return searchAndPassBest(b);
        } else {
            return searchAndPassBest(b2);
        }
    }
private:
    
    typedef std::tuple<Fitness, MultiSearchComponents &, SolutionIterator, Move> BestDesc;

    BestDesc best() {
        Fitness max = Fitness();
        bool init = false;
        auto currSE = m_solution.begin();
        auto endSE =  m_solution.end();
        auto bestSE = currSE;
        Move bestMove = Move();
        for(;currSE != endSE; ++currSE) {
            auto adjustmentSet = call<GetMoves>(m_solution, *currSE);
            auto currMove = adjustmentSet.first;
            for(;currMove !=  adjustmentSet.second; ++currMove) {
                Fitness gain = call<Gain>(m_solution, *currSE, *currMove); 
                
                if(!init || gain > max) {
                    init = true;
                    bestSE = currSE;
                    bestMove = *currMove;
                    max = gain;
                } 
            }
        }
          
        return BestDesc(max, m_searchComponents, bestSE, bestMove);
    }
    
    template <typename Action, typename... Args> 
    auto call(Args&&... args) -> 
    decltype(std::declval<MultiSearchComponents>().template call<Action>(args...)){
        return m_searchComponents.template call<Action>(args...);
    }

    MultiSearchComponents m_searchComponents;
};

template <typename Solution>
class LocalSearchStepMultiSolution<Solution, search_strategies::SteepestSlope> : 
        public LocalSearchStepMultiSolutionBase<Solution> {
      
    typedef LocalSearchStepMultiSolutionBase<Solution> base;
    typedef typename std::iterator_traits<typename utils::CollectionToIter<Solution>::type>::reference SolElementRef;
    template <typename Sol, typename SS, typename... MSC>
    friend class LocalSearchStepMultiSolution;

    LocalSearchStepMultiSolution(Solution & solution) : 
        base(solution) {}
public:
    using base::getSolution;
    using base::m_solution;

    template <typename Best>
    bool searchAndPassBest(Best b) {
        if(std::get<0>(b) > 0) {
            std::get<1>(b).template call<Commit>(m_solution, *std::get<2>(b), std::get<3>(b));
        }
        return std::get<0>(b) > 0;
    }
};

}//! detail


/**
 * @brief local_search_multi_solution function 
 *
 * @tparam SearchStrategy
 * @tparam PostSearchAction
 * @tparam GlobalStopCondition
 * @tparam Solution
 * @tparam Components
 * @param solution
 * @param psa
 * @param gsc
 * @param components
 *
 * @return 
 */
template <typename SearchStrategy = search_strategies::ChooseFirstBetter,
          typename PostSearchAction,
          typename GlobalStopCondition,
          typename Solution,
          typename... Components>
bool local_search_multi_solution(
            Solution & solution,
            PostSearchAction psa,
            GlobalStopCondition gsc,
            Components... components) {
    detail::LocalSearchStepMultiSolution<Solution, SearchStrategy, Components...> lss(solution, std::move(components)...);
    return search(lss, psa, gsc);
}

/**
 * @brief simple version of the local_search_multi_solution
 *
 * @tparam SearchStrategy
 * @tparam Solution
 * @tparam Components
 * @param solution
 * @param components
 *
 * @return 
 */
template <typename SearchStrategy = search_strategies::ChooseFirstBetter, 
          typename Solution, 
          typename... Components>
bool local_search_multi_solution_simple(Solution & solution, Components... components) {
    return local_search_multi_solution<SearchStrategy>(solution, utils::SkipFunctor(), utils::ReturnFalseFunctor(), std::move(components)...);
}


} // local_search
} // paal

#endif // LOCAL_SEARCH_MULTI_SOLUTION_HPP
