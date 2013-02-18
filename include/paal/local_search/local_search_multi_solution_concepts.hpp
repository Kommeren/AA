/**
 * @file local_search_multi_solution_concepts.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <boost/concept_check.hpp>
#include <type_traits>
#include "helpers/type_functions.hpp"

namespace paal {
namespace local_search {
namespace local_search_concepts {


//TODO checking if the method get const object dosn't work in this framework!
//we have to find some workaround
template <typename X>
class MultiSolution  {
    private:
        typedef decltype(std::declval<const X>().end()) IterTypeEnd;
    public:
        typedef decltype(std::declval<const X>().begin()) IterType;
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
class  MultiNeighborhoodGetter {
    private:
        typedef typename MultiSolution<Solution>::Element SolutionElement;
        typedef decltype(std::declval<X>().get(
                                std::declval<const Solution &>(),
                                std::declval<const SolutionElement&>()
                                ).first) UpdateIterator;
    
    public:
        typedef typename std::decay<decltype(*std::declval<UpdateIterator>())>::type Update;
        BOOST_CONCEPT_USAGE(MultiNeighborhoodGetter) {
            x.get(s, e);
        }

    private:

        X x;
        Solution  s;
        SolutionElement e;
};

template <typename X, typename Solution, typename NeighborhoodGetter> class MultiImproveChecker {
    public:
        BOOST_CONCEPT_USAGE(MultiImproveChecker) {
            x.gain(s, e, u);
        }
    
     private:

        X x;
        Solution s;
        typename MultiSolution<Solution>::Element e;
        typename MultiNeighborhoodGetter<NeighborhoodGetter, Solution>::Update u;
};


template <typename X, typename Solution, typename NeighborhoodGetter> class MultiSolutionUpdater {
    public:
        BOOST_CONCEPT_USAGE(MultiSolutionUpdater) {
            x.update(s, e, u);
        }
    
     private:

        X x;
        Solution s;
        typename MultiSolution<Solution>::Element e;
        typename MultiNeighborhoodGetter<NeighborhoodGetter, Solution>::Update u;
};

} // local_search_concepts
} // local_search
} //paal
 
 
 
 
