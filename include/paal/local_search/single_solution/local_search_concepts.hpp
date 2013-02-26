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
class  GetNeighborhood : protected ConceptsBase<X, Solution, SearchComponents> {
    public:
        BOOST_CONCEPT_USAGE(GetNeighborhood) {
            this->x(this->s);
        }
};

template <typename X, typename Solution, typename SearchComponents> 
class Gain : protected ConceptsBase<X, Solution, SearchComponents> {
    public:
        BOOST_CONCEPT_USAGE(Gain) {
            this->x(this->s, this->u);
        }
};


template <typename X, typename Solution, typename SearchComponents> 
class UpdateSolution : protected ConceptsBase<X, Solution, SearchComponents>{
    public:
        BOOST_CONCEPT_USAGE(UpdateSolution) {
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
    typedef typename Traits::GetNeighborhood NG;
    typedef typename Traits::Gain IC;
    typedef typename Traits::UpdateSolution SU;
    typedef typename Traits::StopCondition SC;
public:
    BOOST_CONCEPT_ASSERT((GetNeighborhood<NG, Solution, X>));
    BOOST_CONCEPT_ASSERT((Gain<IC, Solution, X>));
    BOOST_CONCEPT_ASSERT((UpdateSolution<SU, Solution, X>));
    BOOST_CONCEPT_ASSERT((StopCondition<SC, Solution, X>));
};

} // local_search_concepts
} // local_search
} //paal
 
 
 
 
