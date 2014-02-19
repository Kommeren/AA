/**
 * @file iterator_with_stop_condition.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-02-10
 */
#ifndef ITERATOR_WITH_STOP_CONDITION_HPP
#define ITERATOR_WITH_STOP_CONDITION_HPP

#include <boost/iterator/iterator_facade.hpp>

namespace paal {

    /**
     * @brief On each increment/decrement of the iterator the StopCondition is
     *          checked. If it is satisfied the underlying iterator is "moved" to the end.
     *          In other words when StopCondition is fulfilled, the range becomes empty.
     *
     * @tparam Iterator
     * @tparam StopCondition
     */
    template <typename Iterator, typename StopCondition>
        class IteratorWithStopCondition
        : public boost::iterator_facade<
          IteratorWithStopCondition<Iterator, StopCondition>
          , typename std::iterator_traits<Iterator>::value_type
          , typename std::iterator_traits<Iterator>::iterator_category
          , typename std::iterator_traits<Iterator>::reference
          , typename std::iterator_traits<Iterator>::difference_type
          >
    {
        typedef  typename std::iterator_traits<Iterator>::reference reference;

        /**
         * @brief private constructor. Use makeBeginIteratorWithStopCondition,
         *              makeEndIteratorWithStopCondition.
         *
         * @param begin
         * @param end
         * @param stopCondition
         */
            explicit IteratorWithStopCondition(Iterator begin, Iterator end, StopCondition stopCondition)
            : m_begin(begin), m_end(end), m_stopCondtion(stopCondition) {}

        friend class boost::iterator_core_access;
        template <typename I, typename SC>
            friend IteratorWithStopCondition<I,SC>
                makeBeginIteratorWithStopCondition(I, I, SC);

        template <typename I, typename SC>
            friend IteratorWithStopCondition<I,SC>
                makeEndIteratorWithStopCondition(I, SC);

        /**
         * @brief checked if stopCondition is fullfiled
         *        moves m_begin to m_end if yes.
         */
        void checkStop() {
            if(m_stopCondtion()) {
                m_begin = m_end;
            }
        }

        /**
         * @brief increment
         */
        void increment() {
            ++m_begin;
            checkStop();
        }

        /**
         * @brief decrement
         */
        void decrement() {
            --m_begin;
            checkStop();
        }

        /**
         * @brief equal
         *
         * @param other
         *
         * @return
         */
        bool equal(IteratorWithStopCondition const& other) const
        {
            return this->m_begin == other.m_begin;
        }

        /**
         * @brief dereference
         *
         * @return
         */
        reference dereference() const { return *m_begin; }

        /**
         * @brief advance
         *
         * @param n
         */
        void advance(typename std::iterator_traits<Iterator>::difference_type n) {
            m_begin += n;
            checkStop();
        }

        /**
         * @brief distance_to
         *
         * @param other
         *
         * @return
         */
        std::size_t distance_to(IteratorWithStopCondition other) {
            return other.m_begin - m_begin;
        }


        Iterator m_begin;
        Iterator m_end;
        StopCondition m_stopCondtion;
    };

    /**
     * @brief function to create begin of IteratorWithStopCondition
     *
     * @tparam Iterator
     * @tparam StopCondition
     * @param begin
     * @param end
     * @param stopCondition
     *
     * @return
     */
    template <typename Iterator, typename StopCondition>
        IteratorWithStopCondition<Iterator, StopCondition>
        makeBeginIteratorWithStopCondition(Iterator begin, Iterator end, StopCondition stopCondition) {
            return IteratorWithStopCondition<Iterator, StopCondition>(begin, end, stopCondition);
        }

    /**
     * @brief function to create end of IteratorWithStopCondition
     *
     * @tparam Iterator
     * @tparam StopCondition
     * @param end
     * @param stopCondition
     *
     * @return
     */
    template <typename Iterator, typename StopCondition>
        IteratorWithStopCondition<Iterator, StopCondition>
        makeEndIteratorWithStopCondition(Iterator end, StopCondition stopCondition) {
            return IteratorWithStopCondition<Iterator, StopCondition>(end, end, stopCondition);
        }


}//!paal

#endif /* ITERATOR_WITH_STOP_CONDITION_HPP */
