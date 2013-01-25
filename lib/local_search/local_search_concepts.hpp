#include <boost/concept_check.hpp>
#include <type_traits>
#include "helpers/type_functions.hpp"

namespace paal {
namespace local_search {
namespace local_search_concepts {

template <typename X, typename Solution> 
class  NeighbourGetter {
    private:
        typedef decltype(std::declval<X>().getNeighbourhood(
                                std::declval<Solution &>()
                                ).first) UpdateIterator;
    
    public:
        typedef typename std::decay<decltype(*std::declval<UpdateIterator>())>::type UpdateElement;
        BOOST_CONCEPT_USAGE(NeighbourGetter) {
            x.getNeighbourhood(s);
        }

    private:

        X x;
        Solution s;
};

template <typename X, typename Solution, typename NeighbourGetterT> class CheckIfImprove {
    public:
        BOOST_CONCEPT_USAGE(CheckIfImprove) {
            x.checkIfImproved(s, u);
        }
    
     private:

        X x;
        Solution s;
        typename NeighbourGetter<NeighbourGetterT, Solution>::UpdateElement u;
};


template <typename X, typename Solution, typename NeighbourGetterT> class SolutionUpdater {
    public:
        BOOST_CONCEPT_USAGE(SolutionUpdater) {
            x.update(s, u);
        }
    
     private:

        X x;
        Solution s;
        typename NeighbourGetter<NeighbourGetterT, Solution>::UpdateElement u;
};

} // local_search_concepts
} // local_search
} //paal
 
 
 
 
