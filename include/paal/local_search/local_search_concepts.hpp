/**
 * @file local_search_concepts.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <boost/concept_check.hpp>
#include <type_traits>
#include "paal/helpers/type_functions.hpp"

namespace paal {
namespace local_search {
namespace local_search_concepts {

template <typename X, typename Solution> 
class  NeighborhoodGetter {
    private:
        typedef decltype(std::declval<X>().get(
                                std::declval<Solution &>()
                                ).first) UpdateIterator;
    
    public:
        typedef typename std::decay<decltype(*std::declval<UpdateIterator>())>::type Update;
        BOOST_CONCEPT_USAGE(NeighborhoodGetter) {
            x.get(s);
        }

    private:

        X x;
        Solution s;
};

template <typename X, typename Solution, typename NeighborhoodGetterT> class ImproveChecker {
    public:
        BOOST_CONCEPT_USAGE(ImproveChecker) {
            x.gain(s, u);
        }
    
     private:

        X x;
        Solution s;
        typename NeighborhoodGetter<NeighborhoodGetterT, Solution>::Update u;
};


template <typename X, typename Solution, typename NeighborhoodGetterT> class SolutionUpdater {
    public:
        BOOST_CONCEPT_USAGE(SolutionUpdater) {
            x.update(s, u);
        }
    
     private:

        X x;
        Solution s;
        typename NeighborhoodGetter<NeighborhoodGetterT, Solution>::Update u;
};

} // local_search_concepts
} // local_search
} //paal
 
 
 
 
