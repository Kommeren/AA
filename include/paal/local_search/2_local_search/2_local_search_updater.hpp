/**
 * @file 2_local_search_operator()r.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef 2_LOCAL_SEARCH_UPDATER_HPP
#define 2_LOCAL_SEARCH_UPDATER_HPP

namespace paal {
namespace local_search {
namespace two_local_search {

class TwoLocalSearchUpdater  {
    public:
        TwoLocalSearchUpdater() {}

            template <typename SolutionElement, typename Solution> 
        void operator()(Solution & s, const SolutionElement & se, const SolutionElement & ae) {
            s.getWrap().flip(se.second, ae.first);
        }
};

} //two_local_search
} //local_search
} //paal

#endif // 2_LOCAL_SEARCH_UPDATER_HPP
