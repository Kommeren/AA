/**
 * @file local_search_concepts.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#include "paal/local_search/search_components.hpp"
#include "paal/local_search/search_traits.hpp"

#include <boost/concept_check.hpp>

#include <type_traits>

namespace paal {
namespace local_search {
namespace concepts {

template <typename Functor, typename Solution, typename SearchComponents>
struct concepts_base {
    Functor functor;
    Solution s;
    typedef typename move_type<SearchComponents, Solution>::value_type Move;
    Move u;
};

template <typename X, typename Solution, typename SearchComponents>
struct get_moves : concepts_base<X, Solution, SearchComponents> {
    BOOST_CONCEPT_USAGE(get_moves) {
        auto i = this->functor(this->s);
        auto b = std::begin(i);
        auto e = std::end(i);
        for (auto x = b; x != e; ++x) {
            const typename concepts_base<X, Solution, SearchComponents>::Move &
                u = *x;
            boost::ignore_unused_variable_warning(u);
        }
    }
};

template <typename X, typename Solution, typename SearchComponents>
struct gain : concepts_base<X, Solution, SearchComponents> {
    BOOST_CONCEPT_USAGE(gain) {
        boost::ignore_unused_variable_warning(this->functor(this->s, this->u) > this->functor(this->s, this->u));
    }
};

template <typename X, typename Solution, typename SearchComponents>
struct commit : concepts_base<X, Solution, SearchComponents> {
    BOOST_CONCEPT_USAGE(commit) {
        bool b = this->functor(this->s, this->u);
        boost::ignore_unused_variable_warning(b);
    }
};

template <typename X, typename Solution> class search_components {
    typedef search_components_traits<X> Traits;
    typedef typename Traits::GetMovesT NG;
    typedef typename Traits::GainT IC;
    typedef typename Traits::CommitT SU;

  public:
    BOOST_CONCEPT_ASSERT((get_moves<NG, Solution, X>));
    BOOST_CONCEPT_ASSERT((gain<IC, Solution, X>));
    BOOST_CONCEPT_ASSERT((commit<SU, Solution, X>));
};

} // concepts
} // local_search
} // paal
