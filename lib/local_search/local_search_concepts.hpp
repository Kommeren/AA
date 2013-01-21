#include <boost/concept_check.hpp>
#include <type_traits>

namespace paal {
namespace local_search {
namespace local_search_concepts {

//TODO irterator is const iterator (it is better to remove this dependancy )
template <typename X>
class MultiSolution  {
    private:
        typedef decltype(std::declval<X>().cend()) IterTypeEnd;
    public:
        typedef decltype(std::declval<X>().cbegin()) IterType;
        typedef typename std::decay<decltype(*std::declval<IterType>())>::type Element;
        static_assert(std::is_same<IterType, IterTypeEnd>::value, "cbegin type != cend type");
        BOOST_CONCEPT_ASSERT((boost::ForwardIterator<IterType>));
        BOOST_CONCEPT_USAGE(MultiSolution) {
            x.cbegin();
            x.cend();
        }

    private:

        X x;
};


template <typename X, typename Solution> 
class  MultiNeighbourGetter {
    private:
        typedef typename MultiSolution<Solution>::Element SolutionElement;
        typedef decltype(std::declval<X>().getNeighbourhood(
                                std::declval<Solution>(),
                                std::declval<SolutionElement>()
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
 
 
 
 
