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

template <typename Solution, 
          typename MultiSearchComponents>
class LocalSearchStepMultiSolutionBase {
    BOOST_CONCEPT_ASSERT((local_search_concepts::MultiSearchComponents<MultiSearchComponents, Solution>));
protected:    
    typedef typename utils::SolToElem<Solution>::type SolutionElement;
    typedef typename MultiUpdate<MultiSearchComponents, Solution>::type Update;

    LocalSearchStepMultiSolutionBase(Solution solution = Solution(), MultiSearchComponents sc = MultiSearchComponents()) : 
        m_solution(std::move(solution)), m_searchComponents(std::move(sc)) {}

public:

    MultiSearchComponents & getSearchComponents() {
        return m_searchComponents;
    }

    decltype(detail::getSolution(std::declval<Solution &>())) getSolution() {
        return detail::getSolution(m_solution);
    }

protected:

    Solution m_solution;
    MultiSearchComponents m_searchComponents;
};

template <typename Solution, 
          typename MultiSearchComponents,
          typename SearchStrategy = search_strategies::ChooseFirstBetter>
class LocalSearchStepMultiSolution : 
        public LocalSearchStepMultiSolutionBase<Solution, MultiSearchComponents> {
      
    typedef LocalSearchStepMultiSolutionBase<Solution, MultiSearchComponents> base;
    static_assert(std::is_same<SearchStrategy, search_strategies::ChooseFirstBetter>::value || 
                    std::is_same<SearchStrategy, search_strategies::SteepestSlope>::value, "Wrong search strategy");
    typedef typename std::iterator_traits<typename utils::SolToIter<Solution>::type>::reference SolElementRef;
public:

    LocalSearchStepMultiSolution(Solution solution = Solution(), MultiSearchComponents sc = MultiSearchComponents()) : 
        base(std::move(solution), std::move(sc)) {}

    typedef typename  base::SolutionElement SolutionElement;
    typedef typename  base::Update Update;
    using base::m_solution;

    /**
     * @brief tries to improve the solution using local search 
     *
     * @return true if the solution is improved false otherwise 
     */
    bool search() {

        for(SolElementRef r : m_solution) {
            auto adjustmentSet = this->m_searchComponents.getNeighborhood()(m_solution, r);
            for(const Update & update : utils::make_range(adjustmentSet)) {
                if(this->m_searchComponents.gain()(m_solution, r, update) > 0) {
                    this->m_searchComponents.updateSolution()(m_solution, r, update);
                    return true;
                } else if(this->m_searchComponents.stopCondition()(m_solution, r, update)) {
                    return false;
                }
            }
        }
          
        return false;
    }
};

template <typename Solution, typename MultiSearchComponents> 
class LocalSearchStepMultiSolution<Solution, MultiSearchComponents, search_strategies::SteepestSlope> : 
        public LocalSearchStepMultiSolutionBase<Solution, MultiSearchComponents> {      

    typedef LocalSearchStepMultiSolutionBase<Solution, MultiSearchComponents> base;
public:

    LocalSearchStepMultiSolution(Solution solution = Solution(), MultiSearchComponents sc = MultiSearchComponents()) : 
        base(std::move(solution), std::move(sc)) {}

    typedef typename base::SolutionElement SolutionElement;
    typedef typename base::Update Update;
    typedef typename MultiFitness<MultiSearchComponents, Solution>::type Fitness;
    using base::m_solution;

    /**
     * @brief tries to improve the solution using local search 
     *
     * @return true if the solution is improved false otherwise 
     */
    bool search() {
        Fitness max = -1;
        bool stop = false;
        auto currSE = m_solution.begin();
        auto endSE =  m_solution.end();
        auto bestSE = currSE;
        Update bestUpdate;
        for(;currSE != endSE && !stop; ++currSE) {
            auto adjustmentSet = this->m_searchComponents.getNeighborhood()(m_solution, *currSE);
            auto currUpdate = adjustmentSet.first;
            for(;currUpdate !=  adjustmentSet.second && !stop; ++currUpdate) {
                Fitness gain = this->m_searchComponents.gain()(m_solution, *currSE, *currUpdate); 
                
                if(gain > max) {
                    bestSE = currSE;
                    bestUpdate = *currUpdate;
                    max = gain;
                } 
                stop = this->m_searchComponents.stopCondition()(m_solution, *currSE, *currUpdate);
            }
        }

        if(max > 0) {
            this->m_searchComponents.updateSolution()(m_solution, *bestSE, bestUpdate);
        }
          
        return max > 0;
    }
};

} // local_search
} // paal

#endif // LOCAL_SEARCH_MULTI_SOLUTION_HPP
