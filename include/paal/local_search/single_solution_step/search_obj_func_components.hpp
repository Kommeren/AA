/**
 * @file search_obj_func_components.hpp
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
          typename GetNeighborhood, 
          typename ObjectiveFunction, 
          typename UpdateSolution = TrivialUpdateSolution,
          typename StopCondition = utils::ReturnFalseFunctor> 
class SearchObjFunctionComponents {
public:
    SearchObjFunctionComponents(GetNeighborhood ng = GetNeighborhood(), 
                     ObjectiveFunction of = ObjectiveFunction(), 
                     UpdateSolution su = UpdateSolution(), 
                     StopCondition sc = StopCondition()) :
            m_neighborGetterFunctor(std::move(ng)), 
            m_objectiveFunction(std::move(of)), 
            m_solutionUpdaterFunctor(std::move(su)),
            m_stopConditionFunctor(std::move(sc)) {}
    

    GetNeighborhood & getNeighborhood() {
        return m_neighborGetterFunctor;
    } 
    
    ObjectiveFunction  & getObjectiveFunction()  {
        return m_objectiveFunction;
    } 
    
    UpdateSolution    & updateSolution()    {
        return m_solutionUpdaterFunctor;
    } 

    
    StopCondition      & stopCondition()      {
        return m_stopConditionFunctor;
    } 


private:
    GetNeighborhood m_neighborGetterFunctor;
    ObjectiveFunction m_objectiveFunction;
    UpdateSolution m_solutionUpdaterFunctor;
    StopCondition m_stopConditionFunctor;
};

template <typename SearchObjFunctionComponents> 
struct SearchObjFunctionComponentsTraits {
    typedef puretype(std::declval<SearchObjFunctionComponents>().getNeighborhood()) GetNeighborhood; 
    typedef puretype(std::declval<SearchObjFunctionComponents>().getObjectiveFunction()) ObjectiveFunction; 
    typedef puretype(std::declval<SearchObjFunctionComponents>().updateSolution()) UpdateSolution; 
    typedef puretype(std::declval<SearchObjFunctionComponents>().stopCondition()) StopCondition; 
};

} //local_search
} //paal
#endif /* SEARCH_OCJ_FUNC_COMPONENTS_HPP */
