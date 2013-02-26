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
template <typename GetNeighborhood, 
          typename Gain, 
          typename UpdateSolution,
          typename StopCondition = TrivialStopCondition> 
class SearchComponents {
public:
    SearchComponents(GetNeighborhood ng = GetNeighborhood(), 
                     Gain ch = Gain(), 
                     UpdateSolution su = UpdateSolution(), 
                     StopCondition sc = StopCondition()) :
            m_neighborGetterFunctor(std::move(ng)), 
            m_checkFunctor(std::move(ch)), 
            m_solutionUpdaterFunctor(std::move(su)),
            m_stopConditionFunctor(std::move(sc)) {}
    

    GetNeighborhood & getNeighborhood() {
        return m_neighborGetterFunctor;
    } 
    
    Gain     & gain()     {
        return m_checkFunctor;
    } 
    
    UpdateSolution   &  updateSolution()    {
        return m_solutionUpdaterFunctor;
    } 
    
    StopCondition      & stopCondition()      {
        return m_stopConditionFunctor;
    } 

    void setGetNeighborhood(GetNeighborhood ng) {
        m_neighborGetterFunctor = std::move(ng);
    } 
    
    void setGain(Gain ic)     {
        m_checkFunctor = std::move(ic);
    } 
    
    void setUpdateSolution(UpdateSolution su)    {
        m_solutionUpdaterFunctor = std::move(su);
    } 
    
    void setStopCondition(StopCondition sc)      {
        m_stopConditionFunctor = std::move(sc);
    } 

private:
    GetNeighborhood m_neighborGetterFunctor;
    Gain m_checkFunctor;
    UpdateSolution m_solutionUpdaterFunctor;
    StopCondition m_stopConditionFunctor;
};

template <typename GetNeighborhood, 
          typename Gain, 
          typename UpdateSolution,
          typename StopCondition = TrivialStopConditionMultiSolution> 
class MultiSearchComponents : 
    public SearchComponents<
                GetNeighborhood, 
                Gain, 
                UpdateSolution, 
                StopCondition> {
    typedef SearchComponents<
                GetNeighborhood, 
                Gain, 
                UpdateSolution, 
                StopCondition> base;
public:
    MultiSearchComponents(
            GetNeighborhood ng = GetNeighborhood(),
            Gain ch = Gain(),
            UpdateSolution su = UpdateSolution(),
            StopCondition sc = StopCondition()) : base(std::move(ng), std::move(ch), std::move(su), std::move(sc)) {}
};

template <typename GetNeighborhood, 
          typename Gain, 
          typename UpdateSolution,
          typename StopCondition = TrivialStopCondition>
SearchComponents<GetNeighborhood, Gain, UpdateSolution, StopCondition>
 make_SearchComponents(GetNeighborhood ng = GetNeighborhood(), 
                            Gain ic = Gain(), 
                            UpdateSolution su = UpdateSolution(), 
                            StopCondition sc = StopCondition()) {
    return SearchComponents<GetNeighborhood, Gain, UpdateSolution, StopCondition>(
                    std::move(ng), std::move(ic), std::move(su), std::move(sc));
}

template <typename SearchComponents, typename NewGetNeighborhood> 
struct SwapGetNeighborhood {};

/**
 * @brief type of SearchComponents with swapped GetNeighborhood
 */
template <typename GetNeighborhood, 
          typename Gain, 
          typename UpdateSolution,
          typename StopCondition,
          typename NewGetNeighborhood> 
struct SwapGetNeighborhood<SearchComponents<GetNeighborhood, Gain, UpdateSolution, StopCondition>, NewGetNeighborhood> {
    typedef SearchComponents<NewGetNeighborhood, Gain, UpdateSolution, StopCondition> type;
};


/**
 * @brief Swaps GetNeighborhood in the SearchComponents 
 */
template <typename GetNeighborhood, 
          typename Gain, 
          typename UpdateSolution,
          typename StopCondition,
          typename NewGetNeighborhood> 
SearchComponents<NewGetNeighborhood, Gain, UpdateSolution, StopCondition> 
swapGetNeighborhood(SearchComponents<GetNeighborhood, Gain, UpdateSolution, StopCondition> sc, 
                   NewGetNeighborhood ng) {
    return make_SearchComponents(std::move(ng), 
                                 std::move(sc.gain()), 
                                 std::move(sc.updateSolution()),
                                 std::move(sc.stopCondition()));
}

template <typename SearchComponents, typename NewGain> 
struct SwapGain {};

/**
 * @brief type of SearchComponents with swapped Improvechecker
 */
template <typename GetNeighborhood, 
          typename Gain, 
          typename UpdateSolution,
          typename StopCondition,
          typename NewGain> 
struct SwapGain<SearchComponents<GetNeighborhood, Gain, UpdateSolution, StopCondition>, NewGain> {
    typedef SearchComponents<GetNeighborhood, NewGain, UpdateSolution, StopCondition> type;
};


/**
 * @brief Swaps Improve checker in the SearchComponents 
 */
template <typename GetNeighborhood, 
          typename Gain, 
          typename UpdateSolution,
          typename StopCondition,
          typename NewGain> 
SearchComponents<GetNeighborhood, NewGain, UpdateSolution, StopCondition> 
swapGain(SearchComponents<GetNeighborhood, Gain, UpdateSolution, StopCondition> sc, 
                   NewGain ic) {
    return make_SearchComponents(std::move(sc.getNeighborhood()), 
                                 std::move(ic), 
                                 std::move(sc.updateSolution()),
                                 std::move(sc.stopCondition()));
}

template <typename SearchComponents, typename NewUpdateSolution> 
struct SwapUpdateSolution {};

/**
 * @brief type of SearchComponents with swapped UpdateSolution
 */
template <typename GetNeighborhood, 
          typename Gain, 
          typename UpdateSolution,
          typename StopCondition,
          typename NewUpdateSolution> 
struct SwapUpdateSolution<SearchComponents<GetNeighborhood, Gain, UpdateSolution, StopCondition>, NewUpdateSolution> {
    typedef SearchComponents<GetNeighborhood, Gain, NewUpdateSolution, StopCondition> type;
};


/**
 * @brief Swaps UpdateSolution in the SearchComponents 
 */
template <typename GetNeighborhood, 
          typename Gain, 
          typename UpdateSolution,
          typename StopCondition,
          typename NewUpdateSolution> 
SearchComponents<GetNeighborhood, Gain, NewUpdateSolution, StopCondition> 
swapUpdateSolution(SearchComponents<GetNeighborhood, Gain, UpdateSolution, StopCondition> sc, 
                   NewUpdateSolution su) {
    return make_SearchComponents(std::move(sc.getNeighborhood()), 
                                 std::move(sc.gain()), 
                                 std::move(su),
                                 std::move(sc.stopCondition()));
}


template <typename SearchComponents, typename NewStopCondition> 
struct SwapStopCondition {};

/**
 * @brief type of SearchComponents with swapped UpdateSolution
 */
template <typename GetNeighborhood, 
          typename Gain, 
          typename UpdateSolution,
          typename StopCondition,
          typename NewStopCondition> 
struct SwapStopCondition<SearchComponents<GetNeighborhood, Gain, UpdateSolution, StopCondition>, NewStopCondition> {
    typedef SearchComponents<GetNeighborhood, Gain, UpdateSolution, NewStopCondition> type;
};


/**
 * @brief Swaps UpdateSolution in the SearchComponents 
 */
template <typename GetNeighborhood, 
          typename Gain, 
          typename UpdateSolution,
          typename StopCondition,
          typename NewStopCondition> 
SearchComponents<GetNeighborhood, Gain, UpdateSolution, NewStopCondition> 
swapStopCondition(SearchComponents<GetNeighborhood, Gain, UpdateSolution, StopCondition> sc, 
                   NewStopCondition stop) {
    return make_SearchComponents(std::move(sc.getNeighborhood()), 
                                 std::move(sc.gain()), 
                                 std::move(sc.stopCondition()),
                                 std::move(stop));
}

} //local_search
} //paal
#endif /* SEARCH_COMPONENTS_HPP */
