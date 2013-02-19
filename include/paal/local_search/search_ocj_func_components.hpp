/**
 * @file search_ocj_func_components.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-18
 */
#ifndef SEARCH_OCJ_FUNC_COMPONENTS_HPP
#define SEARCH_OCJ_FUNC_COMPONENTS_HPP 

namespace paal {
namespace local_search {

//TODO constructors
template <
          typename NeighborhoodGetter, 
          typename ObjectiveFunction, 
          typename SolutionUpdater = TrivialSolutionUpdater,
          typename StopCondition = TrivialStopCondition> 
class SearchObjFunctionComponents {
public:
    SearchObjFunctionComponents(NeighborhoodGetter ng = NeighborhoodGetter(), 
                     ObjectiveFunction of = ObjectiveFunction(), 
                     SolutionUpdater su = SolutionUpdater(), 
                     StopCondition sc = StopCondition()) :
            m_neighborGetterFunctor(std::move(ng)), 
            m_objectiveFunction(std::move(of)), 
            m_solutionUpdaterFunctor(std::move(su)),
            m_stopConditionFunctor(std::move(sc)) {}
    

    NeighborhoodGetter & getNeighborhoodGetter() {
        return m_neighborGetterFunctor;
    } 
    
    ObjectiveFunction  & getObjectiveFunction()  {
        return m_objectiveFunction;
    } 
    
    SolutionUpdater    & getSolutionUpdater()    {
        return m_solutionUpdaterFunctor;
    } 

    
    StopCondition      & getStopCondition()      {
        return m_stopConditionFunctor;
    } 


private:
    NeighborhoodGetter m_neighborGetterFunctor;
    ObjectiveFunction m_objectiveFunction;
    SolutionUpdater m_solutionUpdaterFunctor;
    StopCondition m_stopConditionFunctor;
};

template <typename SearchObjFunctionComponents> 
struct SearchObjFunctionComponentsTraits {
    typedef puretype(std::declval<SearchObjFunctionComponents>().getNeighborhoodGetter()) NeighborhoodGetter; 
    typedef puretype(std::declval<SearchObjFunctionComponents>().getObjectiveFunction()) ObjectiveFunction; 
    typedef puretype(std::declval<SearchObjFunctionComponents>().getSolutionUpdater()) SolutionUpdater; 
    typedef puretype(std::declval<SearchObjFunctionComponents>().getStopCondition()) StopCondition; 
};

} //local_search
} //paal
#endif /* SEARCH_OCJ_FUNC_COMPONENTS_HPP */
