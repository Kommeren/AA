/**
 * @file subset_iterator.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "paal/utils/type_functions.hpp"
#include "paal/utils/make_tuple.hpp"

#ifndef SUBSET_ITERATOR_HPP
#define SUBSET_ITERATOR_HPP

namespace paal {
namespace data_structures {

template <int k, typename Iterator>
    class SubsetsIteratorEngine :
    private SubsetsIteratorEngine<k - 1, Iterator>{
        protected:
            /**
             * @brief current being
             *
             * @return
             */
            Iterator getBegin() {
                return m_begin;
            }


            /**
             * @brief end is stored in the SubsetsIteratorEngine<0>
             *
             * @return
             */
            Iterator getEnd() {
                return base::getEnd();
            }

            /**
             * @brief sets all iterators to m_end
             */
            void setToEnd() {
                m_begin = getEnd();
                base::setToEnd();
            }


        public:
            using base = SubsetsIteratorEngine<k - 1, Iterator>;

            /**
             * @brief constructor
             *
             * @param begin
             * @param end
             */
            SubsetsIteratorEngine(Iterator begin, Iterator end) :
                base(begin, end) {
                    if(k == 1) {
                        m_begin = begin;
                    } else {
                        auto baseBegin = base::getBegin();
                        if(baseBegin != end) {
                            m_begin = ++baseBegin;
                            if(m_begin == end) {
                                //when we are at the end all iterators are set to m_end
                                base::setToEnd();
                            }
                        } else {
                            //when we are at the end all iterators are set to m_end
                            setToEnd();
                        }
                    }
                }

            SubsetsIteratorEngine()  = default;

            /**
             * @brief sets next configuration of iterators, pointing to next subset
             *
             * @return
             */
            bool next() {
                ++m_begin;
                while(m_begin == getEnd()) {
                    if(base::next()) {
                        m_begin = base::getBegin();
                        if(m_begin == getEnd()) {
                            //when we are at the end all iterators are set to m_end
                            base::setToEnd();
                            return false;
                        }
                        ++m_begin;
                    } else {
                        return false;
                    }
                }
                return true;
            }


            //TODO copy paste (combine_iterator)
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
                auto call(F f, Args&& ... args) const ->
                decltype(std::declval<base>().call(std::move(f), std::forward<Args>(args)..., *std::declval<Iterator>())) {
                    return base::call(std::move(f), *m_begin, std::forward<Args>(args)...);
                }

            /**
             * @brief operator==
             *
             * @param left
             * @param right
             *
             * @return
             */
            friend bool operator==(const SubsetsIteratorEngine & left, const SubsetsIteratorEngine & right) {
                return left.m_begin == right.m_begin &&
                        static_cast<base>(left) == static_cast<base>(right);
            }

        private:
            Iterator m_begin;

    };

/**
 * @brief specialization for k==0 for boundary cases.
 *      This class stores iterator pointing to the end of the input collection
 *
 * @tparam Iterator
 */
template <typename Iterator>
    class SubsetsIteratorEngine<0, Iterator> {
        protected:
            /**
             * @brief constructor
             *
             * @param begin
             * @param end
             */
            SubsetsIteratorEngine(Iterator begin, Iterator end )
                : m_end(end) {}

            SubsetsIteratorEngine()  = default;

            /**
             * @brief getBegin, fake returns m_end
             *
             * @return
             */
            Iterator getBegin() {
                return m_end;
            }

            /**
             * @brief getEnd, returns end of the input collection
             *
             * @return
             */
            Iterator getEnd() {
                return m_end;
            }

            /**
             * @brief boundary case, does nothing
             */
            void setToEnd() {
            }

        public:
            /**
             * @brief boundary case, does nothing
             *
             * @return
             */
            bool next() const {
                return false;
            }

            /**
             * @brief actually calls f for given arguments
             *
             * @tparam F
             * @tparam Args
             * @param f
             * @param args
             *
             * @return
             */
            template <typename F, typename... Args>
                auto call(F f, Args&& ... args) const -> decltype(f(std::forward<Args>(args)...)) {
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
            friend bool operator==(const SubsetsIteratorEngine & left, const SubsetsIteratorEngine & right) {
                return true;
            }

        private:
            Iterator m_end;
    };

/**
 * @brief make for SubsetsIteratorEngine
 *
 * @tparam k
 * @tparam Iterator
 * @param b
 * @param e
 *
 * @return
 */
template <int k, typename Iterator>
    SubsetsIteratorEngine<k, Iterator>
    make_SubsetsIteratorEngine(Iterator b, Iterator e)  {
        return SubsetsIteratorEngine<k, Iterator>(b, e);
    }

/**
 * @class SubsetsIterator
 * @brief Iterator to all k-subsets of given collection.
 *
 * @tparam Iterator
 * @tparam k
 */
template <int k, typename Iterator, typename Joiner = MakeTuple>
    class SubsetsIterator :
        public boost::iterator_facade<SubsetsIterator<k, Iterator, Joiner>
        , puretype((SubsetsIteratorEngine<k, Iterator>().call(std::declval<Joiner>())))
        //                            , typename std::iterator_traits<Iterator>::iterator_category //TODO above forward tags are not yet implemented
        , typename boost::forward_traversal_tag
        , decltype(SubsetsIteratorEngine<k, Iterator>().call(std::declval<Joiner>()))
        >
    {
        public:
            /**
             * @brief constructor
             *
             * @param begin
             * @param end
             * @param joiner
             */
            SubsetsIterator(Iterator begin, Iterator end, Joiner joiner = Joiner{}) :
                m_joiner(joiner), m_iteratorEngine(begin, end)
                { }

            /**
             * @brief default constructor represents end of the range
             */
            SubsetsIterator() = default;

        private:

            /**
             * @brief reference type of the iterator
             */
            using ref =  decltype(SubsetsIteratorEngine<k, Iterator>().call(std::declval<Joiner>()));

            friend class boost::iterator_core_access;

            /**
             * @brief increments iterator
             */
            void increment() {
                m_iteratorEngine.next();
            }

            /**
             * @brief equal function
             *
             * @param other
             *
             * @return
             */
            bool equal(SubsetsIterator const& other) const
            {
                return this->m_iteratorEngine == other.m_iteratorEngine;
            }

            /**
             * @brief dereference
             *
             * @return
             */
            ref dereference() const { return m_iteratorEngine.call(m_joiner); }
            //TODO add random access support

            Joiner m_joiner;
            SubsetsIteratorEngine<k, Iterator> m_iteratorEngine;
    };


/**
 * @brief make for SubsetsIterator
 *
 * @tparam Iterator
 * @tparam k
 * @tparam Pointer
 * @param b
 * @param e
 *
 * @return
 */
template <int k, typename Iterator, typename Joiner = MakeTuple>
    std::pair<SubsetsIterator<k, Iterator, Joiner>, SubsetsIterator<k, Iterator, Joiner>>
    make_SubsetsIteratorRange(Iterator b, Iterator e, Joiner joiner = Joiner{})  {
        typedef SubsetsIterator<k, Iterator, Joiner> SI;
        return std::make_pair(SI(b,e, joiner), SI(e,e, joiner));
    }

} //data_structures
} //paal

#endif // SUBSET_ITERATOR_HPP

