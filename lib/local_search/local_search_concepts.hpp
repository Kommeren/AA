#include <boost/concept_check.hpp>
#include <type_traits>
#include "helpers/type_functions.hpp"

namespace paal {
namespace local_search {
namespace local_search_concepts {

template <typename X, typename Solution> 
class  NeighbourhoodGetter {
    private:
        typedef decltype(std::declval<X>().get(
                                std::declval<Solution &>()
                                ).first) UpdateIterator;
    
    public:
        typedef typename std::decay<decltype(*std::declval<UpdateIterator>())>::type UpdateElement;
        BOOST_CONCEPT_USAGE(NeighbourhoodGetter) {
            x.get(s);
        }

    private:

        X x;
        Solution s;
};

template <typename X, typename Solution, typename NeighbourhoodGetterT> class ImproveChecker {
    public:
        BOOST_CONCEPT_USAGE(ImproveChecker) {
            x.gain(s, u);
        }
    
     private:

        X x;
        Solution s;
        typename NeighbourhoodGetter<NeighbourhoodGetterT, Solution>::UpdateElement u;
};


template <typename X, typename Solution, typename NeighbourhoodGetterT> class SolutionUpdater {
    public:
        BOOST_CONCEPT_USAGE(SolutionUpdater) {
            x.update(s, u);
        }
    
     private:

        X x;
        Solution s;
        typename NeighbourhoodGetter<NeighbourhoodGetterT, Solution>::UpdateElement u;
};

} // local_search_concepts
} // local_search
} //paal
 
 
 
 
