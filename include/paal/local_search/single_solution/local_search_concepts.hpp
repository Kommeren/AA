/**
 * @file local_search_concepts.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <boost/concept_check.hpp>
#include <type_traits>

#include "paal/local_search/search_components.hpp"
#include "paal/local_search/search_traits.hpp"

namespace paal {
namespace local_search {
namespace local_search_concepts {

template <typename X, typename Solution, typename SearchComponents> 
class  ConceptsBase {
    protected:
        X x;
        Solution s;
        typename Update<SearchComponents, Solution>::type u;
};


template <typename X, typename Solution, typename SearchComponents> 
class  NeighborhoodGetter : protected ConceptsBase<X, Solution, SearchComponents> {
    public:
        BOOST_CONCEPT_USAGE(NeighborhoodGetter) {
            this->x(this->s);
        }
};

template <typename X, typename Solution, typename SearchComponents> 
class ImproveChecker : protected ConceptsBase<X, Solution, SearchComponents> {
    public:
        BOOST_CONCEPT_USAGE(ImproveChecker) {
            this->x(this->s, this->u);
        }
};


template <typename X, typename Solution, typename SearchComponents> 
class SolutionUpdater : protected ConceptsBase<X, Solution, SearchComponents>{
    public:
        BOOST_CONCEPT_USAGE(SolutionUpdater) {
            this->x(this->s, this->u);
        }
};

template <typename X, typename Solution, typename SearchComponents> 
class StopCondition : protected ConceptsBase<X, Solution, SearchComponents>{
    public:
        BOOST_CONCEPT_USAGE(StopCondition) {
            this->x(this->s, this->u);
        }
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
 
 
 
 
