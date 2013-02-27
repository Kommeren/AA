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

#include "paal/helpers/iterator_helpers.hpp"
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
    BOOST_CONCEPT_ASSERT((local_search_concepts::SearchComponents<SearchComponents, Solution>));
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
public:
        LocalSearchStep(Solution solution = Solution(), SearchComponents searchComponents = SearchComponents()) :
            base(std::move(solution), std::move(searchComponents)) {}
        typedef typename base::Update Update;

        bool search() {
            auto adjustmentSet = this->m_searchComponents.getNeighborhood()(this->m_solution);

            for(const Update & update : helpers::make_range(adjustmentSet)) {
                if(this->m_searchComponents.gain()(this->m_solution, update) > 0) {
                    this->m_searchComponents.updateSolution()(this->m_solution, update);
                    return true;
                } else {
                    if(this->m_searchComponents.stopCondition()(this->m_solution, update)) {
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
public:
    LocalSearchStep(Solution solution = Solution(), SearchComponents searchComponents= SearchComponents()) :
        base(std::move(solution), std::move(searchComponents)) {}

    bool search() {
        int max = INT_MIN;
        auto adjustmentSet = this->m_searchComponents.getNeighborhood()(this->m_solution);
        auto curr = adjustmentSet.first;
        auto best = curr;

        for(;curr !=  adjustmentSet.second; ++curr) {
            int gain = this->m_searchComponents.gain()(this->m_solution, *curr);
            if(gain > max) {
                best = curr;
                max = gain;
            } else {
                if(this->m_searchComponents.stopCondition()(this->m_solution, *curr)) {
                    break;
                }
            }
        }
        if(max > 0) {
            this->m_searchComponents.updateSolution()(this->m_solution, *best);
        }
        return max > 0;
    }
};

} // local_search
} // paal

#endif // LOCAL_SEARCH_SINGLE_SOLUTION_HPP
