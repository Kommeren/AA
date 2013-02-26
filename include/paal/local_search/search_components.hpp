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

template <typename SearchComponents, typename NewNeighborhoodGetter> 
struct SwapNeighborhoodGetter {};

/**
 * @brief type of SearchComponents with swapped NeighborhoodGetter
 */
template <typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition,
          typename NewNeighborhoodGetter> 
struct SwapNeighborhoodGetter<SearchComponents<NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition>, NewNeighborhoodGetter> {
    typedef SearchComponents<NewNeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition> type;
};


/**
 * @brief Swaps NeighborhoodGetter in the SearchComponents 
 */
template <typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition,
          typename NewNeighborhoodGetter> 
SearchComponents<NewNeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition> 
swapNeighborhoodGetter(SearchComponents<NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition> sc, 
                   NewNeighborhoodGetter ng) {
    return make_SearchComponents(std::move(ng), 
                                 std::move(sc.getImproveChecker()), 
                                 std::move(sc.getSolutionUpdater()),
                                 std::move(sc.getStopCondition()));
}

template <typename SearchComponents, typename NewImproveChecker> 
struct SwapImproveChecker {};

/**
 * @brief type of SearchComponents with swapped Improvechecker
 */
template <typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition,
          typename NewImproveChecker> 
struct SwapImproveChecker<SearchComponents<NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition>, NewImproveChecker> {
    typedef SearchComponents<NeighborhoodGetter, NewImproveChecker, SolutionUpdater, StopCondition> type;
};


/**
 * @brief Swaps Improve checker in the SearchComponents 
 */
template <typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition,
          typename NewImproveChecker> 
SearchComponents<NeighborhoodGetter, NewImproveChecker, SolutionUpdater, StopCondition> 
swapImproveChecker(SearchComponents<NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition> sc, 
                   NewImproveChecker ic) {
    return make_SearchComponents(std::move(sc.getNeighborhoodGetter()), 
                                 std::move(ic), 
                                 std::move(sc.getSolutionUpdater()),
                                 std::move(sc.getStopCondition()));
}

template <typename SearchComponents, typename NewSolutionUpdater> 
struct SwapSolutionUpdater {};

/**
 * @brief type of SearchComponents with swapped SolutionUpdater
 */
template <typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition,
          typename NewSolutionUpdater> 
struct SwapSolutionUpdater<SearchComponents<NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition>, NewSolutionUpdater> {
    typedef SearchComponents<NeighborhoodGetter, ImproveChecker, NewSolutionUpdater, StopCondition> type;
};


/**
 * @brief Swaps SolutionUpdater in the SearchComponents 
 */
template <typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition,
          typename NewSolutionUpdater> 
SearchComponents<NeighborhoodGetter, ImproveChecker, NewSolutionUpdater, StopCondition> 
swapSolutionUpdater(SearchComponents<NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition> sc, 
                   NewSolutionUpdater su) {
    return make_SearchComponents(std::move(sc.getNeighborhoodGetter()), 
                                 std::move(sc.getImproveChecker()), 
                                 std::move(su),
                                 std::move(sc.getStopCondition()));
}


template <typename SearchComponents, typename NewStopCondition> 
struct SwapStopCondition {};

/**
 * @brief type of SearchComponents with swapped SolutionUpdater
 */
template <typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition,
          typename NewStopCondition> 
struct SwapStopCondition<SearchComponents<NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition>, NewStopCondition> {
    typedef SearchComponents<NeighborhoodGetter, ImproveChecker, SolutionUpdater, NewStopCondition> type;
};


/**
 * @brief Swaps SolutionUpdater in the SearchComponents 
 */
template <typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition,
          typename NewStopCondition> 
SearchComponents<NeighborhoodGetter, ImproveChecker, SolutionUpdater, NewStopCondition> 
swapStopCondition(SearchComponents<NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition> sc, 
                   NewStopCondition stop) {
    return make_SearchComponents(std::move(sc.getNeighborhoodGetter()), 
                                 std::move(sc.getImproveChecker()), 
                                 std::move(sc.getStopCondition()),
                                 std::move(stop));
}

} //local_search
} //paal
#endif /* SEARCH_COMPONENTS_HPP */
