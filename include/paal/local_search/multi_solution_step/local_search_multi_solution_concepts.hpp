/**
 * @file local_search_multi_solution_concepts.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <boost/concept_check.hpp>
#include <type_traits>
#include "paal/utils/type_functions.hpp"
#include "paal/local_search/search_components.hpp"
#include "paal/local_search/search_traits.hpp"

namespace paal {
namespace local_search {
namespace local_search_concepts {

//avoid unused variable warning
template <typename T>
int use(const T & t);

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
    typedef typename utils::SolToElem<Solution>::type SolutionElement;
protected:
    typedef typename MultiUpdate<SearchComponents, Solution>::type Update;
    X x;
    Solution  s;
    SolutionElement e;
    Update u;
};

template <typename X, typename Solution, typename SearchComponents> 
class  MultiGetNeighborhood : public MultiConceptsBase<X, Solution, SearchComponents> {
    public:
        BOOST_CONCEPT_USAGE(MultiGetNeighborhood) {
            auto i = this->x(this->s, this->e);
            auto b = i.first;
            auto e = i.second;
            for(auto x = b; x != e; ++x) {
                const typename MultiConceptsBase<X,Solution,SearchComponents>::Update & u = *x;
                use(u);
            }
        }
};

template <typename X, typename Solution, typename SearchComponents> 
class MultiGain : public MultiConceptsBase<X, Solution, SearchComponents>{
    public:
        BOOST_CONCEPT_USAGE(MultiGain) {
            use((this->x)(this->s, this->e, this->u) > 0);
        }
};


template <typename X, typename Solution, typename SearchComponents> 
class MultiUpdateSolution : public MultiConceptsBase<X, Solution, SearchComponents>{
    public:
        BOOST_CONCEPT_USAGE(MultiUpdateSolution) {
            this->x(this->s,this-> e, this->u);
        }
};

template <typename X, typename Solution, typename SearchComponents> 
class MultiStopCondition : public MultiConceptsBase<X, Solution, SearchComponents>{
    public:
        BOOST_CONCEPT_USAGE(MultiStopCondition) {
            bool b = this->x(this->s,this-> e, this->u);
            use(b);
        }
};

template <typename X, typename Solution> 
class MultiSearchComponents {
    typedef SearchComponentsTraits<X> Traits; 
    typedef typename Traits::GetNeighborhood NG;
    typedef typename Traits::Gain IC;
    typedef typename Traits::UpdateSolution SU;
    typedef typename Traits::StopCondition SC;
public:
    BOOST_CONCEPT_ASSERT((MultiGetNeighborhood<NG, Solution, X>));
    BOOST_CONCEPT_ASSERT((MultiGain<IC, Solution, X>));
    BOOST_CONCEPT_ASSERT((MultiUpdateSolution<SU, Solution, X>));
    BOOST_CONCEPT_ASSERT((MultiStopCondition<SC, Solution, X>));
};

} // local_search_concepts
} // local_search
} //paal
 
 
 
 
