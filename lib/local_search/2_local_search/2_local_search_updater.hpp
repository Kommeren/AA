#ifndef __2_LOCAL_SEARCH_SWAPPER__
#define __2_LOCAL_SEARCH_SWAPPER__

namespace paal {
namespace local_search {
namespace two_local_search {

template < typename CycleManager> class TwoLocalSearchUpdater  {
    public:
        TwoLocalSearchSwapper(CycleManager & cm) : m_cycleManager(cm) {}
        template <typename SolutionElement, typename SolutionRange> 
            void update(SolutionRange sr, const SolutionElement & se, const SolutionElement & ae) {
            m_cycleManager.swapEnds(se, ae);
        }

        CycleManager & m_cycleManager;

};
} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH_SWAPPER__
