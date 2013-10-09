/**
 * @file local_search_single_solution.hpp
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

#include <boost/range/iterator_range.hpp>

#include "paal/local_search/local_search.hpp"
#include "local_search_concepts.hpp"

namespace paal {
namespace local_search {

namespace search_strategies {
    class ChooseFirstBetter;
    class SteepestSlope;
}


template <typename Solution> 
class LocalSearchStepBase {
protected:
    LocalSearchStepBase(Solution & solution) :
        m_solution(solution) {}
   
    Solution & m_solution;

public:
    
    Solution & getSolution() {
        return m_solution;
    }
};

template <typename Solution,
          typename SearchStrategy, 
          typename... SearchComponents>

class LocalSearchStep  {
    static_assert(std::is_same<SearchStrategy, search_strategies::ChooseFirstBetter>::value || 
                  std::is_same<SearchStrategy, search_strategies::SteepestSlope>::value, "Wrong search strategy");
    LocalSearchStep() = delete;
};

template <typename Solution,
          typename SearchComponents,
          typename... SearchComponentsRest>

class LocalSearchStep<Solution, search_strategies::ChooseFirstBetter, SearchComponents, SearchComponentsRest...> : 
    public LocalSearchStep<Solution, search_strategies::ChooseFirstBetter, SearchComponentsRest...> {
private:
    BOOST_CONCEPT_ASSERT((concepts::SearchComponents<SearchComponents, Solution>));
    
    typedef LocalSearchStep<Solution, search_strategies::ChooseFirstBetter, SearchComponentsRest...> base;
    using base::m_solution;
    typedef typename  Move<SearchComponents, Solution>::type Move;

public:
    LocalSearchStep(Solution & solution) :
        base(solution) {}

    LocalSearchStep(Solution & solution, SearchComponents searchComponents, SearchComponentsRest... rest) :
        base(solution, std::move(rest)...), m_searchComponents(std::move(searchComponents)) {}

    bool search() {
        auto adjustmentSet = call<GetMoves>(m_solution);

        for(const Move & move : boost::make_iterator_range(adjustmentSet)) {
            if(call<Gain>(m_solution, move) > 0) {
                call<Commit>(m_solution, move);
                return true;
            } else {
                if(call<StopCondition>(m_solution, move)) {
                    break;
                }
            }
        }
        return base::search();
    }
protected:
    template <typename Action, typename... Args> 
    auto call(Args&&... args) -> 
    decltype(std::declval<SearchComponents>().template call<Action>(args...)){
        return m_searchComponents.template call<Action>(args...);
    }

    SearchComponents m_searchComponents;
};

template <typename Solution>
class LocalSearchStep<Solution, search_strategies::ChooseFirstBetter> : 
    public LocalSearchStepBase<Solution> {
public:
    typedef LocalSearchStepBase<Solution> base;
    LocalSearchStep(Solution & sol) : base(sol) {}

    bool search() {
        return false;
    }

};

template <typename Solution, typename SearchComponents, typename... SearchComponentsRest> 
class LocalSearchStep<Solution, search_strategies::SteepestSlope, SearchComponents, SearchComponentsRest...> 
        : public LocalSearchStep<Solution, search_strategies::SteepestSlope, SearchComponentsRest...>  {
    BOOST_CONCEPT_ASSERT((concepts::SearchComponents<SearchComponents, Solution>));
    typedef LocalSearchStep<Solution, search_strategies::SteepestSlope, SearchComponentsRest...> base;
    typedef typename Fitness<SearchComponents, Solution>::type Fitness;
    using base::m_solution;
    typedef typename  Move<SearchComponents, Solution>::type Move;

public:
    LocalSearchStep(Solution & solution) :
        base(solution) {}

    LocalSearchStep(Solution & solution, SearchComponents searchComponents, SearchComponentsRest... rest) :
        base(solution, std::move(rest)...), m_searchComponents(std::move(searchComponents)) {}

    bool search() {
        return base::searchAndPassBest(best());
    }
private:
    typedef std::tuple<Fitness, SearchComponents &, Move> BestDesc;
    BestDesc best() {
        Fitness max = Fitness();
        bool init = false;
        auto adjustmentSet = call<GetMoves>(m_solution);
        auto currMove = adjustmentSet.first;
        Move bestMove = Move();

        for(;currMove !=  adjustmentSet.second; ++currMove) {
            Fitness gain = call<Gain>(m_solution, *currMove);
            if(!init || gain > max) {
                bestMove = *currMove;
                max = gain;
                init = true;
            } 
            if(call<StopCondition>(m_solution, *currMove)) {
                break;
            }
        }

        return BestDesc(max, m_searchComponents, bestMove);
    }
protected:
    
    template <typename Best>
    bool searchAndPassBest(const Best & b) {
        auto b2 = best();
        if(std::get<0>(b) > std::get<0>(b2)) {
            return base::searchAndPassBest(b);
        } else {
            return base::searchAndPassBest(b2);
        }
    }
    
    template <typename Action, typename... Args> 
    auto call(Args&&... args) -> 
    decltype(std::declval<SearchComponents>().template call<Action>(args...)){
        return m_searchComponents.template call<Action>(args...);
    }

    SearchComponents m_searchComponents;
};

template <typename Solution> 
class LocalSearchStep<Solution, search_strategies::SteepestSlope> 
        : public LocalSearchStepBase<Solution>  {
public:
    typedef LocalSearchStepBase<Solution> base;
    LocalSearchStep(Solution & sol) : base(sol) {}
    
    template <typename Best>
    bool searchAndPassBest(const Best &b) {
        if(std::get<0>(b) > 0) {
            std::get<1>(b).template call<Commit>(this->m_solution, std::get<2>(b));
        }
        return std::get<0>(b) > 0;
        
    }
};


template <typename SearchStrategy = search_strategies::ChooseFirstBetter,
          typename PostSearchAction,
          typename GlobalStopCondition,
          typename Solution,
          typename... Components>
bool local_search(
            Solution & solution,
            PostSearchAction psa,
            GlobalStopCondition gsc,
            Components... components) {
    LocalSearchStep<Solution, SearchStrategy, Components...> lss(solution, std::move(components)...);
    return search(lss, psa, gsc);
}

template <typename SearchStrategy = search_strategies::ChooseFirstBetter, 
          typename Solution, 
          typename... Components>
bool local_search_simple(Solution & solution, Components... components) {
    return local_search<SearchStrategy>(solution, utils::SkipFunctor(), utils::ReturnFalseFunctor(), std::move(components)...);
}




} // local_search
} // paal

#endif // LOCAL_SEARCH_SINGLE_SOLUTION_HPP
