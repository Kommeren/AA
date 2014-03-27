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


template <typename X, typename Solution, typename search_components>
class  concepts_base {
    protected:
        X x;
        Solution s;
        typedef typename move_type<search_components, Solution>::value_type Move;
        Move u;
};

//avoid unused variable warning
template <typename T>
int use(const T & t);

template <typename X, typename Solution, typename search_components>
class  get_moves : protected concepts_base<X, Solution, search_components> {
    public:
        BOOST_CONCEPT_USAGE(get_moves) {
            auto i = this->x(this->s);
            auto b = std::begin(i);
            auto e = std::end(i);
            for(auto x = b; x != e; ++x) {
                const typename concepts_base<X,Solution,search_components>::Move & u = *x;
                use(u);
            }
        }
};

template <typename X, typename Solution, typename search_components>
class Gain : protected concepts_base<X, Solution, search_components> {
    public:
        BOOST_CONCEPT_USAGE(Gain) {
            use(this->x(this->s, this->u) > 0);
        }
};


template <typename X, typename Solution, typename search_components>
class Commit : protected concepts_base<X, Solution, search_components>{
    public:
        BOOST_CONCEPT_USAGE(Commit) {
            bool b = this->x(this->s, this->u);
            detail::use(b);
        }
};

template <typename X, typename Solution>
class search_components {
    typedef search_components_traits<X> Traits;
    typedef typename Traits::get_movesT NG;
    typedef typename Traits::GainT IC;
    typedef typename Traits::CommitT SU;
public:
    BOOST_CONCEPT_ASSERT((get_moves<NG, Solution, X>));
    BOOST_CONCEPT_ASSERT((Gain<IC, Solution, X>));
    BOOST_CONCEPT_ASSERT((Commit<SU, Solution, X>));
};

} // concepts
} // local_search
} //paal




