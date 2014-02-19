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

namespace detail {
    template <typename T>
    void use(const T & t);
}//detail

template <typename X>
class MultiSolution  {
    private:
        typedef decltype(std::declval<const X>().end()) IterTypeEnd;
    public:
        typedef decltype(std::declval<const X>().begin()) IterType;
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

namespace detail {
template <typename X, typename Solution, typename SearchComponents>
class MultiConceptsBase {
    typedef typename utils::CollectionToElem<Solution>::type SolutionElement;
protected:
    typedef typename MultiMove<SearchComponents, Solution>::type Move;
    X x;
    Solution  s;
    SolutionElement e;
    Move u;
};
}//detail

template <typename X, typename Solution, typename SearchComponents>
class  MultiGetMoves : public detail::MultiConceptsBase<X, Solution, SearchComponents> {
    public:
        BOOST_CONCEPT_USAGE(MultiGetMoves) {
            auto i = this->x(this->s, this->e);
            auto b = i.first;
            auto e = i.second;
            for(auto x = b; x != e; ++x) {
                const typename detail::MultiConceptsBase<X,Solution,SearchComponents>::Move & u = *x;
                detail::use(u);
            }
        }
};

template <typename X, typename Solution, typename SearchComponents>
class MultiGain : public detail::MultiConceptsBase<X, Solution, SearchComponents>{
    public:
        BOOST_CONCEPT_USAGE(MultiGain) {
            detail::use((this->x)(this->s, this->e, this->u) > 0);
        }
};


template <typename X, typename Solution, typename SearchComponents>
class MultiCommit : public detail::MultiConceptsBase<X, Solution, SearchComponents>{
    public:
        BOOST_CONCEPT_USAGE(MultiCommit) {
            this->x(this->s,this-> e, this->u);
        }
};

template <typename X, typename Solution, typename SearchComponents>
class MultiStepStopCondition : public detail::MultiConceptsBase<X, Solution, SearchComponents>{
    public:
        BOOST_CONCEPT_USAGE(MultiStepStopCondition) {
            bool b = this->x(this->s,this-> e, this->u);
            detail::use(b);
        }
};

template <typename X, typename Solution>
class MultiSearchComponents {
    typedef SearchComponentsTraits<X> Traits;
    typedef typename Traits::GetMovesT NG;
    typedef typename Traits::GainT IC;
    typedef typename Traits::CommitT SU;
public:
    BOOST_CONCEPT_ASSERT((MultiGetMoves<NG, Solution, X>));
    BOOST_CONCEPT_ASSERT((MultiGain<IC, Solution, X>));
    BOOST_CONCEPT_ASSERT((MultiCommit<SU, Solution, X>));
};

} // local_search_concepts
} // local_search
} //paal




