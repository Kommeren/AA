/**
 * @file combine_iterator.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/empty.hpp>

#include "paal/utils/type_functions.hpp"

#ifndef COMBINE_ITERATOR_HPP
#define COMBINE_ITERATOR_HPP

namespace paal {
namespace data_structures {


/**
 * @brief class representing set of ranges with two operation next and call
 *
 * @tparam Ranges
 */
template <typename... Ranges>
    class CombineIteratorEngine;

/**
 * @class CombineIteratorEngine
 * @brief actual implementation
 *
 * @tparam Range
 * @tparam RangesRest
 */
template <typename Range, typename... RangesRest>
    class CombineIteratorEngine<Range, RangesRest...> :
     private CombineIteratorEngine<RangesRest...> {

        public:
            using base = CombineIteratorEngine<RangesRest...>;
            using Iterator = typename boost::range_iterator<Range>::type;

            /**
             * @brief constructor
             *
             * @param range
             * @param rest
             */
            CombineIteratorEngine(Range & range, RangesRest & ... rest) :
                base(rest...)
                , m_begin(std::begin(range))
                , m_curr(std::begin(range))
                , m_end(std::end(range))
        {}

            CombineIteratorEngine()  = default;

            /**
             * @brief move iterators to the next position
             *
             * @return
             */
            bool next() {
                if(!base::next()) {
                    ++m_curr;
                    if(m_curr == m_end) {
                        m_curr = m_begin;
                        return false;
                    }
                }
                return true;
            }


            /**
             * @brief calls arbitrary function f on (*m_curr)...
             *
             * @tparam F
             * @tparam Args
             * @param f
             * @param args
             *
             * @return
             */
            template <typename F, typename... Args>
                auto call(F f, Args&& ... args) ->
                decltype(std::declval<base>().call(std::move(f), std::forward<Args>(args)..., *std::declval<Iterator>())) {
                    return base::call(std::move(f), std::forward<Args>(args)..., *m_curr);
                }

            /**
             * @brief operator==
             *
             * @param left
             * @param right
             *
             * @return
             */
            friend bool operator==(const CombineIteratorEngine & left, const CombineIteratorEngine & right) {
                return left.m_begin == right.m_begin
                       && left.m_end == right.m_end
                       && left.m_curr == right.m_curr
                       && static_cast<base>(left) == static_cast<base>(right);
            }

        private:
            Iterator m_begin;
            Iterator m_curr;
            Iterator m_end;
    };

/**
 * @brief specialization for empty ranges lists
 */
template <>
    class CombineIteratorEngine<> {
        public:
            /**
             * @brief no next configuration
             *
             * @return
             */
            bool next() {
                return false;
            }


            /**
             * @brief actually  calls function f
             *
             * @tparam F
             * @tparam Args
             * @param f
             * @param args
             *
             * @return
             */
            template <typename F, typename... Args>
                auto call(F f, Args&& ... args) -> decltype(f(std::forward<Args>(args)...)) {
                    return f(std::forward<Args>(args)...);
                }

            /**
             * @brief operator==, always true
             *
             * @param left
             * @param right
             *
             * @return
             */
            friend bool operator==(const CombineIteratorEngine & left, const CombineIteratorEngine & right) {
                return true;
            }
    };

namespace detail {
    //TODO can you do this without alias???
    template <typename T>
    using rem_ref = typename std::remove_reference<T>::type;
}


/**
 * @brief make for CombineIteratorEngine
 *
 * @tparam Ranges
 * @param ranges
 *
 * @return
 */
template <typename... Ranges>
    CombineIteratorEngine<detail::rem_ref<Ranges>...>
    make_CombineIteratorEngine(Ranges&& ...  ranges) {
        //see comments in make_CombineIterator
        return CombineIteratorEngine<detail::rem_ref<Ranges>...>{ranges...};
    }

/**
 * @brief CombineIterator iterates through all combinations of values from given ranges
 *        and returns them joined together using given Joiner
 *
 * @tparam Joiner
 * @tparam Ranges
 */
template <typename Joiner, typename... Ranges>
    class CombineIterator : public boost::iterator_facade<CombineIterator<Joiner, Ranges...>
                            , puretype(CombineIteratorEngine<Ranges...>().call(std::declval<Joiner>()))
                            , boost::forward_traversal_tag //TODO this should be minimal tag of the ranges
                            , decltype(CombineIteratorEngine<Ranges...>().call(std::declval<Joiner>()))
                            >
{
    public:
        /**
         * @brief constructor
         *
         * @param joiner
         * @param ranges
         */
        CombineIterator(Joiner joiner, Ranges & ... ranges) :
            m_joiner(joiner), m_iteratorEngine(ranges...),
            m_end(sizeof...(Ranges) ? isEmpty(ranges...) : true)
        { }

        /**
         * @brief default constructor represents end of the range
         */
        CombineIterator() : m_end(true) {};

    private:
        /**
         * @brief returns true if at least one of given ranges is empty
         *
         * @tparam Range
         * @tparam RangesRest
         * @param range
         * @param rest
         *
         * @return
         */
        template <typename Range, typename... RangesRest>
            bool isEmpty(const Range & range, const RangesRest & ... rest) {
                if(boost::empty(range))  {
                    return true;
                } else {
                    return isEmpty(rest...);
                }
            }

        /**
         * @brief boundary case for isEmpty
         *
         * @return
         */
        bool isEmpty() {
            return false;
        }

        using ref =  decltype(CombineIteratorEngine<Ranges...>().call(std::declval<Joiner>()));

        friend class boost::iterator_core_access;

        /**
         * @brief increments iterator
         */
        void increment() {
            if(!m_iteratorEngine.next()) {
                m_end = true;
            }
        }

        /**
         * @brief equal function
         *
         * @param other
         *
         * @return
         */
        bool equal(CombineIterator const& other) const
        {
            return this->m_end == other.m_end &&
                (this->m_end || this->m_iteratorEngine == other.m_iteratorEngine);
        }

        /**
         * @brief dereference
         *
         * @return
         */
        ref dereference() const { return m_iteratorEngine.call(m_joiner); }

        Joiner m_joiner;
        mutable CombineIteratorEngine<Ranges...> m_iteratorEngine;
        bool m_end;
};


/**
 * @brief make for CombineIterator
 *
 * @tparam Joiner
 * @tparam Ranges
 * @param joiner
 * @param ranges
 *
 * @return
 */
template <typename Joiner, typename... Ranges>
    CombineIterator<Joiner, detail::rem_ref<Ranges>...>
    make_CombineIterator(Joiner joiner, Ranges&& ...  ranges) {
        //we do not forward the ranges, because CombineIterator expects lvalues
        //we Use Ranges && because, we'd like to cover const/nonconst cases
        return CombineIterator<Joiner, detail::rem_ref<Ranges>...>{joiner, ranges...};
    }

} //data_structures
} //paal

#endif // COMBINE_ITERATOR_HPP

