#include <boost/concept_check.hpp>
#include <type_traits>
#include "helpers/type_functions.hpp"

namespace paal {
namespace local_search {
namespace local_search_concepts {

template <typename X>
class MultiSolution  {
    private:
        typedef decltype(std::declval<X>().end()) IterTypeEnd;
    public:
        typedef decltype(std::declval<X>().begin()) IterType;
        typedef typename std::decay<decltype(*std::declval<IterType>())>::type Element;
        static_assert(std::is_same<IterType, IterTypeEnd>::value, "begin type != end type");
        BOOST_CONCEPT_ASSERT((boost::ForwardIterator<IterType>));
        BOOST_CONCEPT_USAGE(MultiSolution) {
            x.begin();
            x.end();
        }

    private:

        X x;
};


template <typename X, typename Solution> 
class  MultiNeighbourGetter {
    private:
        typedef typename MultiSolution<Solution>::Element SolutionElement;
        typedef decltype(std::declval<X>().getNeighbourhood(
                                std::declval<Solution &>(),
                                std::declval<SolutionElement&>()
                                ).first) UpdateIterator;
    
    public:
        typedef typename std::decay<decltype(*std::declval<UpdateIterator>())>::type UpdateElement;
        BOOST_CONCEPT_USAGE(MultiNeighbourGetter) {
            x.getNeighbourhood(s, e);
        }

    private:

        X x;
        Solution s;
        SolutionElement e;
};

template <typename X, typename Solution, typename NeighbourGetter> class MultiCheckIfImprove {
    public:
        BOOST_CONCEPT_USAGE(MultiCheckIfImprove) {
            x.checkIfImproved(s, e, u);
        }
    
     private:

        X x;
        Solution s;
        typename MultiSolution<Solution>::Element e;
        typename MultiNeighbourGetter<NeighbourGetter, Solution>::UpdateElement u;
};


template <typename X, typename Solution, typename NeighbourGetter> class MultiSolutionUpdater {
    public:
        BOOST_CONCEPT_USAGE(MultiSolutionUpdater) {
            x.update(s, e, u);
        }
    
     private:

        X x;
        Solution s;
        typename MultiSolution<Solution>::Element e;
        typename MultiNeighbourGetter<NeighbourGetter, Solution>::UpdateElement u;
};

} // local_search_concepts
} // local_search
} //paal
 
 
 
 
