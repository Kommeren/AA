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

namespace detail {
    template <typename T>
    void use(const T & t);
}//!detail


template <typename X, typename Solution, typename SearchComponents>
class  ConceptsBase {
    protected:
        X x;
        Solution s;
        typedef typename MoveType<SearchComponents, Solution>::type Move;
        Move u;
};

//avoid unused variable warning
template <typename T>
int use(const T & t);

template <typename X, typename Solution, typename SearchComponents>
class  GetMoves : protected ConceptsBase<X, Solution, SearchComponents> {
    public:
        BOOST_CONCEPT_USAGE(GetMoves) {
            auto i = this->x(this->s);
            auto b = i.first;
            auto e = i.second;
            for(auto x = b; x != e; ++x) {
                const typename ConceptsBase<X,Solution,SearchComponents>::Move & u = *x;
                use(u);
            }
        }
};

template <typename X, typename Solution, typename SearchComponents>
class Gain : protected ConceptsBase<X, Solution, SearchComponents> {
    public:
        BOOST_CONCEPT_USAGE(Gain) {
            use(this->x(this->s, this->u) > 0);
        }
};


template <typename X, typename Solution, typename SearchComponents>
class Commit : protected ConceptsBase<X, Solution, SearchComponents>{
    public:
        BOOST_CONCEPT_USAGE(Commit) {
            bool b = this->x(this->s, this->u);
            detail::use(b);
        }
};

template <typename X, typename Solution>
class SearchComponents {
    typedef SearchComponentsTraits<X> Traits;
    typedef typename Traits::GetMovesT NG;
    typedef typename Traits::GainT IC;
    typedef typename Traits::CommitT SU;
public:
    BOOST_CONCEPT_ASSERT((GetMoves<NG, Solution, X>));
    BOOST_CONCEPT_ASSERT((Gain<IC, Solution, X>));
    BOOST_CONCEPT_ASSERT((Commit<SU, Solution, X>));
};

} // concepts
} // local_search
} //paal




