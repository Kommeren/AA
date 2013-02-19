/**
 * @file search_components.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-18
 */
#ifndef SEARCH_COMPONENTS_HPP
#define SEARCH_COMPONENTS_HPP 

#include "trivial_stop_condition.hpp"
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
    
     SolutionUpdater   & getSolutionUpdater()    {
        return m_solutionUpdaterFunctor;
    } 

    
    StopCondition      & getStopCondition()      {
        return m_stopConditionFunctor;
    } 


private:
    NeighborhoodGetter m_neighborGetterFunctor;
    ImproveChecker m_checkFunctor;
    SolutionUpdater m_solutionUpdaterFunctor;
    StopCondition m_stopConditionFunctor;
};

template <typename SearchComponents> 
struct SearchComponentsTraits {
    typedef puretype(std::declval<SearchComponents>().getNeighborhoodGetter()) NeighborhoodGetter; 
    typedef puretype(std::declval<SearchComponents>().getImproveChecker()) ImproveChecker; 
    typedef puretype(std::declval<SearchComponents>().getSolutionUpdater()) SolutionUpdater; 
    typedef puretype(std::declval<SearchComponents>().getStopCondition()) StopCondition; 
    template <typename Solution> struct UpdateTraits {
        typedef decltype(std::declval<NeighborhoodGetter>().get(
                                std::declval<Solution &>()
                                ).first) UpdateIterator;
        typedef typename helpers::IterToElem<UpdateIterator>::type Update;

    };

};


} //local_search
} //paal
#endif /* SEARCH_COMPONENTS_HPP */
