/**
 * @file fusion_algorithms.hpp
 :* @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-02-26
 */
#ifndef STATIC_LAZY_JOIN_HPP
#define STATIC_LAZY_JOIN_HPP

#include "paal/utils/type_functions.hpp"

#include <boost/fusion/include/begin.hpp>

namespace paal {
namespace data_structures {


/**
 * @brief class for polymorphic join on boost fusion sequence
 */
class polymorfic_fold {

    struct Fold {
        template <typename Functor,
                 typename IterEnd,
                 typename AccumulatorFunctor,
                 typename AccumulatorData>
                     typename std::result_of<AccumulatorFunctor(AccumulatorData)>::type
                     operator()(
                             Functor,
                             AccumulatorFunctor accumulatorFunctor,
                             AccumulatorData accumulatorData,
                             IterEnd,
                             IterEnd) const {
                         return accumulatorFunctor(accumulatorData);
                     }

        template <typename Functor,
                 typename IterBegin,
                 typename IterEnd,
                 typename AccumulatorFunctor,
                 typename AccumulatorData,
                 ///this dummy condition is needed because on the lookup phase
                 ///some compilers on some instances (clang-3.4) try to instanciate this template
                 ///which causes infinite loop
                 typename Dummy = typename  std::enable_if<!std::is_same<IterEnd, IterBegin>::value>::type>
                     auto  operator()(
                             Functor f,
                             AccumulatorFunctor accumulatorFunctor,
                             AccumulatorData accumulatorData,
                             IterBegin begin,
                             IterEnd end) const ->
                     typename  std::result_of<Functor(
                             typename boost::fusion::result_of::deref<IterBegin>::type,
                             AccumulatorFunctor,
                             AccumulatorData,
                             //TODO Why this move is needed??? (without it doesn't compile on clang-3.4)
                             decltype(std::move(std::bind(*this, f,
                                     std::placeholders::_1,
                                     std::placeholders::_2,
                                     boost::fusion::next(begin),
                                     end))))>::type
                     {
                         auto continuation = std::bind(*this, f,
                                 std::placeholders::_1,
                                 std::placeholders::_2,
                                 boost::fusion::next(begin),
                                 end);

                         return f(*begin, accumulatorFunctor, accumulatorData, continuation);
                     }

    };


public:
    /**
     * @brief operator()
     *
     * @tparam Functor
     * @tparam AccumulatorFunctor
     * @tparam AccumulatorData
     * @tparam Sequence
     * @param f
     * @param accumulatorFunctor
     * @param accumulatorData
     * @param seq
     *
     * @return
     */
    template <typename Functor, typename AccumulatorFunctor, typename AccumulatorData, typename Sequence>
        auto operator()(
                Functor f,
                AccumulatorFunctor accumulatorFunctor,
                AccumulatorData accumulatorData,
                Sequence & seq) const ->
        decltype(Fold{}(f, accumulatorFunctor, accumulatorData,
                    boost::fusion::begin(seq), boost::fusion::end(seq)))
        {
            return Fold{}(f, accumulatorFunctor, accumulatorData,
                    boost::fusion::begin(seq), boost::fusion::end(seq));
        }
};

/**
 * @brief Find for StaticLazyJoin
 */
class Satisfy {

    /**
     * @brief operator(), specialization for empty Join
     *
     * @tparam Predicate
     *
     * @return
     */
    template <typename Predicate, typename IterEnd>
        bool satisfy(Predicate, IterEnd, IterEnd) const {
            return false;
        }

    /**
     * @brief operator()
     *
     * @tparam Predicate
     * @tparam GetRangeAndTag
     * @tparam GetRangeAndTagRest
     * @param pred
     * @param join
     *
     * @return
     */
    template <typename Predicate, typename IterBegin, typename IterEnd>
        bool satisfy(
                Predicate pred,
                IterBegin begin, IterEnd end) const {
            if(pred(*begin)) {
                return true;
            }
            return satisfy(pred, boost::fusion::next(begin), end);
        }

public:
    /**
     * @brief operator()
     *
     * @tparam Predicate
     * @tparam Seq
     * @param pred
     * @param seq
     *
     * @return
     */
    template <typename Predicate, typename Seq>
        bool operator()(
                Predicate pred,
                Seq & seq) const {
            return satisfy(pred, boost::fusion::begin(seq), boost::fusion::end(seq));
        }
};


} //!data_structures
} //!paal


#endif /* STATIC_LAZY_JOIN_HPP */
