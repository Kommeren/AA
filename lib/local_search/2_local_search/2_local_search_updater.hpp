#ifndef __2_LOCAL_SEARCH_SWAPPER__
#define __2_LOCAL_SEARCH_SWAPPER__

namespace paal {
namespace local_search {
namespace two_local_search {

class TwoLocalSearchUpdater  {
    public:
        TwoLocalSearchUpdater() {}

            template <typename SolutionElement, typename Solution> 
        void update(Solution & s, const SolutionElement & se, const SolutionElement & ae) {
            s.getCycleManager().flip(se.second, ae.first);
        }
};

} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH_SWAPPER__
