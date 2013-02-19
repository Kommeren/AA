/**
 * @file local_search_multi_solution_concepts.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <boost/concept_check.hpp>
#include <type_traits>
#include "paal/helpers/type_functions.hpp"
#include "search_components.hpp"
#include "search_traits.hpp"

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

template <typename X, typename Solution, typename SearchComponents> 
class MultiConceptsBase {
    typedef typename MultiUpdate<SearchComponents, Solution>::type Update;
    typedef typename helpers::SolToElem<Solution>::type SolutionElement;
protected:
    X x;
    Solution  s;
    SolutionElement e;
    Update u;
};

template <typename X, typename Solution, typename SearchComponents> 
class  MultiNeighborhoodGetter : public MultiConceptsBase<X, Solution, SearchComponents> {
    public:
        BOOST_CONCEPT_USAGE(MultiNeighborhoodGetter) {
            this->x.get(this->s, this->e);
        }
};

template <typename X, typename Solution, typename SearchComponents> 
class MultiImproveChecker : public MultiConceptsBase<X, Solution, SearchComponents>{
    public:
        BOOST_CONCEPT_USAGE(MultiImproveChecker) {
            this->x.gain(this->s, this->e, this->u);
        }
};


template <typename X, typename Solution, typename SearchComponents> 
class MultiSolutionUpdater : public MultiConceptsBase<X, Solution, SearchComponents>{
    public:
        BOOST_CONCEPT_USAGE(MultiSolutionUpdater) {
            this->x.update(this->s,this-> e, this->u);
        }
};

template <typename X, typename Solution, typename SearchComponents> 
class MultiStopCondition : public MultiConceptsBase<X, Solution, SearchComponents>{
    public:
        BOOST_CONCEPT_USAGE(MultiStopCondition) {
            this->x.stop(this->s,this-> e, this->u);
        }
};

template <typename X, typename Solution> 
class MultiSearchComponents {
    typedef SearchComponentsTraits<X> Traits; 
    typedef typename Traits::NeighborhoodGetter NG;
    typedef typename Traits::ImproveChecker IC;
    typedef typename Traits::SolutionUpdater SU;
    typedef typename Traits::StopCondition SC;
public:
    BOOST_CONCEPT_ASSERT((MultiNeighborhoodGetter<NG, Solution, X>));
    BOOST_CONCEPT_ASSERT((MultiImproveChecker<IC, Solution, X>));
    BOOST_CONCEPT_ASSERT((MultiSolutionUpdater<SU, Solution, X>));
    BOOST_CONCEPT_ASSERT((MultiStopCondition<SC, Solution, X>));
};

} // local_search_concepts
} // local_search
} //paal
 
 
 
 
