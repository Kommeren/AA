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
#include "paal/utils/iterator_utils.hpp"

namespace paal {
namespace local_search {

namespace search_strategies {
    class ChooseFirstBetter;
    class SteepestSlope;
}

namespace detail {
    template <typename Solution>
    typename std::enable_if<utils::has_get<Solution>::value, decltype(std::declval<Solution>().get()) &>::type 
    getSolution(Solution & s) {
        return s.get();
    }
    
    
    template <typename Solution>
    typename std::enable_if<!utils::has_get<Solution>::value, Solution &>::type 
    getSolution(Solution & s) {
        return s;
    }
}

template <typename Solution>
class LocalSearchStepMultiSolutionBase {
protected:    
    typedef typename utils::SolToElem<Solution>::type SolutionElement;

    LocalSearchStepMultiSolutionBase(Solution solution = Solution()) : 
        m_solution(std::move(solution)) {}

public:

    decltype(detail::getSolution(std::declval<Solution &>())) getSolution() {
        return detail::getSolution(m_solution);
    }

protected:

    Solution m_solution;
};

template <typename Solution,
          typename SearchStrategy,
          typename... MultiSearchComponentsRest>
class LocalSearchStepMultiSolution {
    static_assert(std::is_same<SearchStrategy, search_strategies::ChooseFirstBetter>::value || 
                    std::is_same<SearchStrategy, search_strategies::SteepestSlope>::value, "Wrong search strategy");
    LocalSearchStepMultiSolution() = delete;
};


template <typename Solution,
          typename MultiSearchComponents,
          typename... MultiSearchComponentsRest>
class LocalSearchStepMultiSolution<Solution, search_strategies::ChooseFirstBetter, MultiSearchComponents, MultiSearchComponentsRest...> : 
        public LocalSearchStepMultiSolution<Solution, search_strategies::ChooseFirstBetter, MultiSearchComponentsRest...> {
    BOOST_CONCEPT_ASSERT((local_search_concepts::MultiSearchComponents<MultiSearchComponents, Solution>));
    typedef typename MultiUpdate<MultiSearchComponents, Solution>::type Update;
    
    typedef LocalSearchStepMultiSolution<Solution, search_strategies::ChooseFirstBetter, MultiSearchComponentsRest...> base;
    typedef typename std::iterator_traits<typename utils::SolToIter<Solution>::type>::reference SolElementRef;
public:

    LocalSearchStepMultiSolution(
            Solution solution, 
            MultiSearchComponents sc, 
            MultiSearchComponentsRest... rest) : 
        base(std::move(solution), std::move(rest)...), m_searchComponents(std::move(sc)) {}
    
    LocalSearchStepMultiSolution(
            Solution solution = Solution()) : 
        base(std::move(solution)), m_searchComponents(MultiSearchComponents()) {}

    typedef typename  base::SolutionElement SolutionElement;
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
            auto adjustmentSet = m_searchComponents.getNeighborhood()(m_solution, r);
            for(const Update & update : utils::make_range(adjustmentSet)) {
                if(m_searchComponents.gain()(m_solution, r, update) > 0) {
                    m_searchComponents.updateSolution()(m_solution, r, update);
                    return true;
                } else if(m_searchComponents.stopCondition()(m_solution, r, update)) {
                    return false;
                }
            }
        }
          
        return base::search();
    }
private:
    MultiSearchComponents m_searchComponents;
};

template <typename Solution>
class LocalSearchStepMultiSolution<Solution, search_strategies::ChooseFirstBetter> : 
        public LocalSearchStepMultiSolutionBase<Solution> {
      
    typedef LocalSearchStepMultiSolutionBase<Solution> base;
    typedef typename std::iterator_traits<typename utils::SolToIter<Solution>::type>::reference SolElementRef;
    template <typename Sol, typename SS, typename... MSC>
    friend class LocalSearchStepMultiSolution;

    LocalSearchStepMultiSolution(Solution solution = Solution()) : 
        base(std::move(solution)) {}
public:
    using base::getSolution;
    bool search() {
        return false;
    }
};


template <typename Solution, typename MultiSearchComponents, typename... MultiSearchComponentsRest> 
class LocalSearchStepMultiSolution<Solution, search_strategies::SteepestSlope, MultiSearchComponents, MultiSearchComponentsRest...> : 
        public LocalSearchStepMultiSolution<Solution, search_strategies::SteepestSlope, MultiSearchComponentsRest...> {      

    BOOST_CONCEPT_ASSERT((local_search_concepts::MultiSearchComponents<MultiSearchComponents, Solution>));
    typedef typename MultiUpdate<MultiSearchComponents, Solution>::type Update;
    typedef LocalSearchStepMultiSolution<Solution, search_strategies::SteepestSlope, MultiSearchComponentsRest...> base;
public:

    LocalSearchStepMultiSolution(Solution solution = Solution(), MultiSearchComponents sc = MultiSearchComponents()) : 
        base(std::move(solution)), m_searchComponents(std::move(sc)) {}

    typedef typename base::SolutionElement SolutionElement;
    typedef typename utils::SolToIter<Solution>::type SolutionIterator;
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
    
    typedef std::tuple<Fitness, MultiSearchComponents &, SolutionIterator, Update> BestDesc;

    BestDesc best() {
        Fitness max = Fitness();
        bool init = false;
        bool stop = false;
        auto currSE = m_solution.begin();
        auto endSE =  m_solution.end();
        auto bestSE = currSE;
        Update bestUpdate = Update();
        for(;currSE != endSE && !stop; ++currSE) {
            auto adjustmentSet = m_searchComponents.getNeighborhood()(m_solution, *currSE);
            auto currUpdate = adjustmentSet.first;
            for(;currUpdate !=  adjustmentSet.second && !stop; ++currUpdate) {
                Fitness gain = m_searchComponents.gain()(m_solution, *currSE, *currUpdate); 
                
                if(!init || gain > max) {
                    init = true;
                    bestSE = currSE;
                    bestUpdate = *currUpdate;
                    max = gain;
                } 
                stop = m_searchComponents.stopCondition()(m_solution, *currSE, *currUpdate);
            }
        }
          
        return BestDesc(max, m_searchComponents, bestSE, bestUpdate);
    }
    MultiSearchComponents m_searchComponents;
};

template <typename Solution>
class LocalSearchStepMultiSolution<Solution, search_strategies::SteepestSlope> : 
        public LocalSearchStepMultiSolutionBase<Solution> {
      
    typedef LocalSearchStepMultiSolutionBase<Solution> base;
    typedef typename std::iterator_traits<typename utils::SolToIter<Solution>::type>::reference SolElementRef;
    template <typename Sol, typename SS, typename... MSC>
    friend class LocalSearchStepMultiSolution;

    LocalSearchStepMultiSolution(Solution solution = Solution()) : 
        base(std::move(solution)) {}
public:
    using base::getSolution;
    using base::m_solution;

    template <typename Best>
    bool searchAndPassBest(Best b) {
        if(std::get<0>(b) > 0) {
            std::get<1>(b).updateSolution()(m_solution, *std::get<2>(b), std::get<3>(b));
        }
        return std::get<0>(b) > 0;
    }
};



} // local_search
} // paal

#endif // LOCAL_SEARCH_MULTI_SOLUTION_HPP
