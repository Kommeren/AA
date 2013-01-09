#ifndef __2_LOCAL_SEARCH_SWAPPER__
#define __2_LOCAL_SEARCH_SWAPPER__

#include "data_structures/simple_cycle_manager.hpp"

template < typename CycleManager> class TwoLocalSearchSwapper  {
    public:
        TwoLocalSearchSwapper(CycleManager & cm) : m_cycleManager(cm) {}
        template <typename SolutionElement, typename SolutionRange> 
            SolutionRange swap(SolutionRange sr, const SolutionElement & se, const SolutionElement & ae) {
            m_cycleManager.swapEnds(se, ae);
            return m_cycleManager.getEdgeRange(se->first);
        }

        CycleManager & m_cycleManager;

};

#endif // __2_LOCAL_SEARCH_SWAPPER__
