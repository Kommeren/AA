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

#include "paal/utils/iterator_utils.hpp"
#include "paal/local_search/local_search.hpp"
#include "local_search_concepts.hpp"

namespace paal {
namespace local_search {

namespace search_strategies {
    class ChooseFirstBetter;
    class SteepestSlope;
}


template <typename Solution, 
          typename SearchComponents> 

class LocalSearchStepBase {
    BOOST_CONCEPT_ASSERT((concepts::SearchComponents<SearchComponents, Solution>));
protected:
    LocalSearchStepBase(Solution solution, SearchComponents searchComponents) :
        m_solution(std::move(solution)), m_searchComponents(std::move(searchComponents)) {}
   
    Solution m_solution;
    SearchComponents m_searchComponents;

public:
    typedef typename  Update<SearchComponents, Solution>::type Update;
    
    Solution & getSolution() {
        return m_solution;
    }
};


template <typename Solution,
          typename SearchComponents,
          typename SearchStrategy = search_strategies::ChooseFirstBetter> 

class LocalSearchStep : 
    public LocalSearchStepBase<Solution, SearchComponents> {
    
        static_assert(std::is_same<SearchStrategy, search_strategies::ChooseFirstBetter>::value || 
                      std::is_same<SearchStrategy, search_strategies::SteepestSlope>::value, "Wrong search strategy");
    typedef LocalSearchStepBase<Solution, SearchComponents> base;
    using base::m_solution;

protected:
    using base::m_searchComponents;

public:
    LocalSearchStep(Solution solution = Solution(), SearchComponents searchComponents = SearchComponents()) :
        base(std::move(solution), std::move(searchComponents)) {}
    typedef typename base::Update Update;

    bool search() {
        auto adjustmentSet = m_searchComponents.getNeighborhood()(m_solution);

        for(const Update & update : utils::make_range(adjustmentSet)) {
            if(m_searchComponents.gain()(m_solution, update) > 0) {
                m_searchComponents.updateSolution()(m_solution, update);
                return true;
            } else {
                if(m_searchComponents.stopCondition()(m_solution, update)) {
                    break;
                }
            }
        }
        return false;
    }
};

template <typename Solution, typename SearchComponents> 
class LocalSearchStep<Solution, SearchComponents, search_strategies::SteepestSlope> 
        : public LocalSearchStepBase<Solution, SearchComponents>  {
    typedef LocalSearchStepBase<Solution, SearchComponents> base;
    typedef typename Fitness<SearchComponents, Solution>::type Fitness;
    using base::m_solution;

protected:
    using base::m_searchComponents;

public:
    LocalSearchStep(Solution solution = Solution(), SearchComponents searchComponents= SearchComponents()) :
        base(std::move(solution), std::move(searchComponents)) {}

    bool search() {
        Fitness max = -1;
        auto adjustmentSet = m_searchComponents.getNeighborhood()(m_solution);
        auto currUpdate = adjustmentSet.first;
        auto bestUpdate = currUpdate;

        for(;currUpdate !=  adjustmentSet.second; ++currUpdate) {
            Fitness gain = m_searchComponents.gain()(m_solution, *currUpdate);
            if(gain > max) {
                bestUpdate = currUpdate;
                max = gain;
            } 
            if(m_searchComponents.stopCondition()(m_solution, *currUpdate)) {
                break;
            }
        }
        if(max > 0) {
            m_searchComponents.updateSolution()(m_solution, *bestUpdate);
        }
        return max > 0;
    }
};

} // local_search
} // paal

#endif // LOCAL_SEARCH_SINGLE_SOLUTION_HPP
