/**
 * @file local_search_concepts.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <boost/concept_check.hpp>
#include <type_traits>

#include "search_components.hpp"

namespace paal {
namespace local_search {
namespace local_search_concepts {

template <typename X, typename Solution, typename SearchComponents> 
class  NeighborhoodGetter {
    private:
    
    public:
        BOOST_CONCEPT_USAGE(NeighborhoodGetter) {
            x.get(s);
        }

    private:

        X x;
        Solution s;
        typename SearchComponentsTraits<SearchComponents>::template UpdateTraits<Solution>::Update u;
};

template <typename X, typename Solution, typename SearchComponents> class ImproveChecker {
    public:
        BOOST_CONCEPT_USAGE(ImproveChecker) {
            x.gain(s, u);
        }
    
     private:

        X x;
        Solution s;
        typename SearchComponentsTraits<SearchComponents>::template UpdateTraits<Solution>::Update u;
};


template <typename X, typename Solution, typename SearchComponents> class SolutionUpdater {
    public:
        BOOST_CONCEPT_USAGE(SolutionUpdater) {
            x.update(s, u);
        }
    
     private:

        X x;
        Solution s;
        typename SearchComponentsTraits<SearchComponents>::template UpdateTraits<Solution>::Update u;
};

template <typename X, typename Solution, typename SearchComponents> class StopCondition {
    public:
        BOOST_CONCEPT_USAGE(StopCondition) {
            x.stop(s, u);
        }
    
     private:

        X x;
        Solution s;
        typename SearchComponentsTraits<SearchComponents>::template UpdateTraits<Solution>::Update u;
};

template <typename X, typename Solution> 
class SearchComponents {
    typedef SearchComponentsTraits<X> Traits; 
    typedef typename Traits::NeighborhoodGetter NG;
    typedef typename Traits::ImproveChecker IC;
    typedef typename Traits::SolutionUpdater SU;
    typedef typename Traits::StopCondition SC;
public:
    BOOST_CONCEPT_ASSERT((NeighborhoodGetter<NG, Solution, X>));
    BOOST_CONCEPT_ASSERT((ImproveChecker<IC, Solution, X>));
    BOOST_CONCEPT_ASSERT((SolutionUpdater<SU, Solution, X>));
    BOOST_CONCEPT_ASSERT((StopCondition<SC, Solution, X>));
};

} // local_search_concepts
} // local_search
} //paal
 
 
 
 
