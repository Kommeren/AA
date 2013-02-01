/**
 * @file 2_local_search.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef __2_LOCAL_SEARCH__
#define __2_LOCAL_SEARCH__

#include "local_search/local_search_multi_solution.hpp"
#include "local_search/trivial_neighbour.hpp"
#include "local_search/2_local_search/2_local_search_updater.hpp"
#include "local_search/2_local_search/2_local_search_checker.hpp"
#include "local_search/2_local_search/2_local_search_solution_adapter.hpp"
#include "data_structures/simple_cycle_manager.hpp"

namespace paal {
namespace local_search {
namespace two_local_search {


template <typename Cycle,
          typename ImproveChecker, 
          typename NeighbourhoodGetter = TrivialNeigbourGetter>

         class  TwoLocalSearchStep : 
             public LocalSearchStepMultiSolution<TwoLocalSearchContainer<Cycle>, 
                        NeighbourhoodGetter, ImproveChecker, TwoLocalSearchUpdater >  {
               
                typedef LocalSearchStepMultiSolution<TwoLocalSearchContainer<Cycle> , NeighbourhoodGetter, 
                            ImproveChecker, TwoLocalSearchUpdater > LocalSearchStepT;

                public:

                     TwoLocalSearchStep(Cycle c, 
                                        ImproveChecker ich, 
                                        NeighbourhoodGetter ng = NeighbourhoodGetter()) 

                        :   LocalSearchStepT(TwoLocalSearchContainer<Cycle>(m_cycle), std::move(ng), 
                                                std::move(ich), TwoLocalSearchUpdater()),
                                                m_cycle(std::move(c)) {}

                private:
                    Cycle m_cycle;
             };


template <typename Cycle,
          typename ImproveChecker, 
          typename NeighbourhoodGetter>

TwoLocalSearchStep<Cycle, ImproveChecker, NeighbourhoodGetter>  

    make_TwoLocalSearchStep(Cycle c, ImproveChecker ich, 
            NeighbourhoodGetter ng = TrivialNeigbourGetter()) {

    return TwoLocalSearchStep<Cycle, ImproveChecker, NeighbourhoodGetter>(c, ich, ng);
}


} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH__

