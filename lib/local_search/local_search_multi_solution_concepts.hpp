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
        //TODO problem with boost_join
//        BOOST_CONCEPT_ASSERT((boost::ForwardIterator<IterType>));
        BOOST_CONCEPT_USAGE(MultiSolution) {
            x.begin();
            x.end();
        }

    private:

        X x;
};


template <typename X, typename Solution> 
class  MultiNeighbourhoodGetter {
    private:
        typedef typename MultiSolution<Solution>::Element SolutionElement;
        typedef decltype(std::declval<X>().get(
                                std::declval<Solution &>(),
                                std::declval<SolutionElement&>()
                                ).first) UpdateIterator;
    
    public:
        typedef typename std::decay<decltype(*std::declval<UpdateIterator>())>::type Update;
        BOOST_CONCEPT_USAGE(MultiNeighbourhoodGetter) {
            x.get(s, e);
        }

    private:

        X x;
        Solution s;
        SolutionElement e;
};

template <typename X, typename Solution, typename NeighbourhoodGetter> class MultiImproveChecker {
    public:
        BOOST_CONCEPT_USAGE(MultiImproveChecker) {
            x.gain(s, e, u);
        }
    
     private:

        X x;
        Solution s;
        typename MultiSolution<Solution>::Element e;
        typename MultiNeighbourhoodGetter<NeighbourhoodGetter, Solution>::Update u;
};


template <typename X, typename Solution, typename NeighbourhoodGetter> class MultiSolutionUpdater {
    public:
        BOOST_CONCEPT_USAGE(MultiSolutionUpdater) {
            x.update(s, e, u);
        }
    
     private:

        X x;
        Solution s;
        typename MultiSolution<Solution>::Element e;
        typename MultiNeighbourhoodGetter<NeighbourhoodGetter, Solution>::Update u;
};

} // local_search_concepts
} // local_search
} //paal
 
 
 
 
