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
namespace concepts {

template <typename X, typename Solution, typename SearchComponents> 
class  ConceptsBase {
    protected:
        X x;
        Solution s;
        typedef typename Update<SearchComponents, Solution>::type Update;
        Update u;
};

//avoid unused variable warning
template <typename T>
int use(const T & t);

template <typename X, typename Solution, typename SearchComponents> 
class  GetNeighborhood : protected ConceptsBase<X, Solution, SearchComponents> {
    public:
        BOOST_CONCEPT_USAGE(GetNeighborhood) {
            auto i = this->x(this->s);
            auto b = i.first;
            auto e = i.second;
            for(auto x = b; x != e; ++x) {
                const typename ConceptsBase<X,Solution,SearchComponents>::Update & u = *x;
                use(u);
            }
        }
};

template <typename X, typename Solution, typename SearchComponents> 
class Gain : protected ConceptsBase<X, Solution, SearchComponents> {
    public:
        BOOST_CONCEPT_USAGE(Gain) {
            int i = this->x(this->s, this->u);          
            use(i);
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
            bool b = this->x(this->s, this->u);
            use(b);
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

} // concepts
} // local_search
} //paal
 
 
 
 
