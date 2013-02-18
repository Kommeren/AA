/**
 * @file 2_local_search_updater.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
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
            s.get().flip(se.second, ae.first);
        }
};

} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH_SWAPPER__
