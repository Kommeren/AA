/**
 * @file paal/local_search/search_components.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-18
 */
#ifndef SEARCH_COMPONENTS_HPP
#define SEARCH_COMPONENTS_HPP 

#include "paal/local_search/single_solution/trivial_stop_condition.hpp"
#include "paal/local_search/multi_solution/trivial_stop_condition_multi_solution.hpp"
#include "paal/helpers/type_functions.hpp"

namespace paal {
namespace local_search {

//TODO constructors
template <typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition = TrivialStopCondition> 
class SearchComponents {
public:
    SearchComponents(NeighborhoodGetter ng = NeighborhoodGetter(), 
                     ImproveChecker ch = ImproveChecker(), 
                     SolutionUpdater su = SolutionUpdater(), 
                     StopCondition sc = StopCondition()) :
            m_neighborGetterFunctor(std::move(ng)), 
            m_checkFunctor(std::move(ch)), 
            m_solutionUpdaterFunctor(std::move(su)),
            m_stopConditionFunctor(std::move(sc)) {}
    

    NeighborhoodGetter & getNeighborhoodGetter() {
        return m_neighborGetterFunctor;
    } 
    
    ImproveChecker     & getImproveChecker()     {
        return m_checkFunctor;
    } 
    
    SolutionUpdater   &  getSolutionUpdater()    {
        return m_solutionUpdaterFunctor;
    } 
    
    StopCondition      & getStopCondition()      {
        return m_stopConditionFunctor;
    } 

    void setNeighborhoodGetter(NeighborhoodGetter ng) {
        m_neighborGetterFunctor = std::move(ng);
    } 
    
    void setImproveChecker(ImproveChecker ic)     {
        m_checkFunctor = std::move(ic);
    } 
    
    void setSolutionUpdater(SolutionUpdater su)    {
        m_solutionUpdaterFunctor = std::move(su);
    } 
    
    void setStopCondition(StopCondition sc)      {
        m_stopConditionFunctor = std::move(sc);
    } 

private:
    NeighborhoodGetter m_neighborGetterFunctor;
    ImproveChecker m_checkFunctor;
    SolutionUpdater m_solutionUpdaterFunctor;
    StopCondition m_stopConditionFunctor;
};

template <typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition = TrivialStopConditionMultiSolution> 
class MultiSearchComponents : 
    public SearchComponents<
                NeighborhoodGetter, 
                ImproveChecker, 
                SolutionUpdater, 
                StopCondition> {
    typedef SearchComponents<
                NeighborhoodGetter, 
                ImproveChecker, 
                SolutionUpdater, 
                StopCondition> base;
public:
    MultiSearchComponents(
            NeighborhoodGetter ng = NeighborhoodGetter(),
            ImproveChecker ch = ImproveChecker(),
            SolutionUpdater su = SolutionUpdater(),
            StopCondition sc = StopCondition()) : base(std::move(ng), std::move(ch), std::move(su), std::move(sc)) {}
};

template <typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition = TrivialStopCondition>
SearchComponents<NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition>
 make_SearchComponents(NeighborhoodGetter ng = NeighborhoodGetter(), 
                            ImproveChecker ic = ImproveChecker(), 
                            SolutionUpdater su = SolutionUpdater(), 
                            StopCondition sc = StopCondition()) {
    return SearchComponents<NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition>(
                    std::move(ng), std::move(ic), std::move(su), std::move(sc));
}

} //local_search
} //paal
#endif /* SEARCH_COMPONENTS_HPP */
