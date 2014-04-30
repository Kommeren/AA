/**
 * @file knapsack_0_1.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-30
 */
#ifndef KNAPSACK_0_1_HPP
#define KNAPSACK_0_1_HPP


#include "paal/utils/functors.hpp"
#include "paal/utils/less_pointees.hpp"
#include "paal/utils/knapsack_utils.hpp"
#include "paal/dynamic/knapsack/fill_knapsack_dynamic_table.hpp"
#include "paal/dynamic/knapsack/get_upper_bound.hpp"
#include "paal/dynamic/knapsack/knapsack_common.hpp"
#include "paal/greedy/knapsack_0_1_two_app.hpp"

#include <boost/range/adaptor/reversed.hpp>
#include <boost/function_output_iterator.hpp>
#include <boost/optional.hpp>
#include <boost/range/irange.hpp>

#include <vector>

namespace paal {

namespace detail {

    /**
     * @brief For 0/1 knapsack dynamic algorithm for given element the table has to be traversed from the highest to the lowest element
     */
    struct Knapsack_0_1_get_position_range {
        template <typename T>
            auto operator()(T begin, T end) ->
            decltype(boost::irange(begin, end) | boost::adaptors::reversed)
            {
                return boost::irange(begin, end) | boost::adaptors::reversed;
            }
    };

    /**
     * @brief This class helps solving 0/1 knapsack problem.
     *        Function solve returns the optimal value
     *        Function Retrieve solution returns chosen elements
     *
     * @tparam ObjectsIter
     * @tparam ObjectSizeFunctor
     * @tparam ObjectValueFunctor
     */
    template <typename ObjectsIter,
              typename ObjectSizeFunctor,
              typename ObjectValueFunctor,
              typename Comparator>
    class Knapsack_0_1  {
        typedef knapsack_base<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> base;
        typedef typename base::SizeType   SizeType;
        typedef typename base::ValueType  ValueType;
        typedef typename base::ObjectType ObjectType;
        typedef typename base::ObjectRef  ObjectRef;
        typedef typename base::return_type  return_type;
        typedef boost::optional<ValueType> ValueOrNull;
        static_assert(std::is_integral<SizeType>::value, "Size type must be integral");
        typedef std::vector<ValueOrNull> ValueOrNullVector;

    public:

        Knapsack_0_1(
                ObjectSizeFunctor size,
                ObjectValueFunctor value,
                Comparator compare = Comparator()) :
            m_size(size),
            m_value(value),
            m_comparator(compare){}

        /**
         * @brief  Function solves dynamic programming problem
         * @returns the optimal value
         */
        template <typename GetBestElement>
        return_type solve(ObjectsIter oBegin, ObjectsIter oEnd, SizeType capacity, GetBestElement getBest) {
            m_object_on_size.resize(capacity + 1);
            fill_table(m_object_on_size, oBegin, oEnd, capacity);
            auto maxValue = getBest(m_object_on_size.begin(), m_object_on_size.end(), m_comparator);

            if(maxValue != m_object_on_size.end()) {
                return return_type(**maxValue, maxValue - m_object_on_size.begin());
            } else {
                return return_type(ValueType(), SizeType());
            }
        }

        //@brief here we find actual solution
        //that is, the chosen objects
        //this is done by simple divide and conquer strategy
        template <typename OutputIterator>
            void retrieve_solution(ValueType maxValue, SizeType size , ObjectsIter oBegin, ObjectsIter oEnd, OutputIterator out) const {
                m_object_on_size   .resize(size + 1);
                m_object_on_size_rec.resize(size + 1);
                retrieve_solution_rec(maxValue, size, oBegin, oEnd, out);
            }


    private:
        template <typename OutputIterator>
            void retrieve_solution_rec(ValueType maxValue, SizeType capacity, ObjectsIter oBegin, ObjectsIter oEnd, OutputIterator out) const {
                if(maxValue == ValueType()) {
                    return;
                }

                auto objNr = std::distance(oBegin, oEnd);
                assert(objNr);

                //boundary case only one object left
                if(objNr == 1) {
                    assert(m_value(*oBegin) == maxValue);
                    *out = *oBegin;
                    ++out;
                    return ;
                }

                //main case, at least 2 objects left
                auto midle = oBegin + objNr / 2;
                fill_table(m_object_on_size, oBegin, midle, capacity);
                fill_table(m_object_on_size_rec, midle, oEnd, capacity);

                SizeType capacityLeftPartInOptimalSolution = SizeType();
                for(auto capacityLeftPart : boost::irange(SizeType(), capacity + 1)) {
                    auto left  = m_object_on_size[capacityLeftPart];
                    auto right = m_object_on_size_rec[capacity - capacityLeftPart];
                    if(left && right) {
                        if(*left + *right == maxValue) {
                            capacityLeftPartInOptimalSolution = capacityLeftPart;
                            break;
                        }
                    }
                }
                auto left  = m_object_on_size[capacityLeftPartInOptimalSolution];
                auto right = m_object_on_size_rec[capacity - capacityLeftPartInOptimalSolution];
                assert(left && right && *left + *right == maxValue);

                retrieve_solution_rec(*left, capacityLeftPartInOptimalSolution, oBegin, midle, out);
                retrieve_solution_rec(*right, capacity - capacityLeftPartInOptimalSolution, midle, oEnd, out);
            }


        void fill_table(ValueOrNullVector & values, ObjectsIter oBegin, ObjectsIter oEnd, SizeType capacity) const {
            fill_knapsack_dynamic_table(values.begin(), values.begin() + capacity + 1, oBegin, oEnd,  m_size,
                    [&](ValueOrNull val, ObjectsIter obj) { return *val + m_value(*obj);},
                    [&](ValueOrNull left, ValueOrNull right) { return m_comparator(*left, *right);},
                    [](ValueOrNull & val) {val = ValueType();},
                    Knapsack_0_1_get_position_range());
        }

        ObjectSizeFunctor  m_size;
        ObjectValueFunctor m_value;
        Comparator m_comparator;
        mutable ValueOrNullVector  m_object_on_size;
        mutable ValueOrNullVector  m_object_on_size_rec;
    };


    template <typename Knapsack, typename IndexType, typename ValueType,
              typename ObjectsIter, typename OutputIterator>
    void retrieve_solution(const Knapsack & knapsack,
                    ValueType maxValue,
                    IndexType size,
                    ObjectsIter oBegin,
                    ObjectsIter oEnd,
                    OutputIterator out,
                    retrieve_solution_tag) {
        knapsack.retrieve_solution(maxValue, size, oBegin, oEnd, out);
    }

    template <typename Knapsack, typename IndexType, typename ValueType,
              typename ObjectsIter, typename OutputIterator>
    void retrieve_solution(const Knapsack & knapsack,
                    ValueType maxValue,
                    IndexType size,
                    ObjectsIter oBegin,
                    ObjectsIter oEnd,
                    OutputIterator out,
                    no_retrieve_solution_tag)  {
    }

/**
 * @brief Solution to Knapsack 0/1 problem
 *  overload for integral Size case
 */
template <typename ObjectsIter,
          typename OutputIterator,
          typename ObjectSizeFunctor,
          typename ObjectValueFunctor,
          typename retrieve_solution_tag>
    FunctorsOnIteratorPValuePair<ObjectValueFunctor, ObjectSizeFunctor, ObjectsIter>
knapsack(ObjectsIter oBegin,
        ObjectsIter oEnd,
        FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out,
        ObjectSizeFunctor size,
        ObjectValueFunctor value,
        zero_one_tag,
        integral_size_tag,
        retrieve_solution_tag retrieve_solutionTag) {
    typedef detail::FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter> ValueType;

    Knapsack_0_1<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor, utils::Less> knapsack(size, value);
    auto maxValueAndSize = knapsack.solve(oBegin, oEnd, capacity,
            get_max_element_on_capacity_indexed_collection<ValueType>());
    retrieve_solution(knapsack, maxValueAndSize.first, maxValueAndSize.second, oBegin, oEnd, out, retrieve_solutionTag);
    return maxValueAndSize;
}

/**
 * @brief Solution to Knapsack 0/1 problem
 *  overload for integral Value case
 */
template <typename ObjectsIter,
          typename OutputIterator,
          typename ObjectSizeFunctor,
          typename ObjectValueFunctor,
          typename retrieve_solution_tag>
    FunctorsOnIteratorPValuePair<ObjectValueFunctor, ObjectSizeFunctor, ObjectsIter>
knapsack(ObjectsIter oBegin,
        ObjectsIter oEnd,
        FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out,
        ObjectSizeFunctor size,
        ObjectValueFunctor value,
        zero_one_tag,
        integral_value_tag,
        retrieve_solution_tag retrieve_solutionTag) {
    typedef FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> SizeType;
    typedef FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter> ValueType;

    Knapsack_0_1<ObjectsIter, ObjectValueFunctor, ObjectSizeFunctor, utils::Greater> knapsack(value, size);
    auto maxValue = get_value_upper_bound(oBegin, oEnd, capacity, value, size, zero_one_tag());
    auto maxValueAndSize = knapsack.solve(oBegin, oEnd, maxValue,
            get_max_element_on_value_indexed_collection<boost::optional<SizeType>, ValueType>(boost::optional<SizeType>(capacity + 1)));
    retrieve_solution(knapsack, maxValueAndSize.first, maxValueAndSize.second, oBegin, oEnd, out, retrieve_solutionTag);
    return std::make_pair(maxValueAndSize.second, maxValueAndSize.first);
}

} //detail

/**
 * @brief Solution to Knapsack 0/1 problem
 *
 * @tparam ObjectsIter
 * @tparam OutputIterator
 * @tparam ObjectSizeFunctor
 * @tparam ObjectValueFunctor
 * @param oBegin given objects
 * @param oEnd
 * @param out the result is returned using output iterator
 * @param size functor that for given object returns its size
 * @param value functor that for given object returns its value
 */
template <typename ObjectsIter,
          typename OutputIterator,
          typename ObjectSizeFunctor,
          typename ObjectValueFunctor = utils::return_something_functor<int,1>>
    detail::FunctorsOnIteratorPValuePair<ObjectValueFunctor, ObjectSizeFunctor, ObjectsIter>
knapsack_0_1(ObjectsIter oBegin,
        ObjectsIter oEnd,
        detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out,
        ObjectSizeFunctor size,
        ObjectValueFunctor value = ObjectValueFunctor()) {

    return detail::knapsack_check_integrality(oBegin, oEnd, capacity, out,
            size, value, detail::zero_one_tag());
}

/**
 * @brief Solution to Knapsack 0/1 problem, without retrieving the objects in the solution
 *
 * @tparam ObjectsIter
 * @tparam OutputIterator
 * @tparam ObjectSizeFunctor
 * @tparam ObjectValueFunctor
 * @param oBegin given objects
 * @param oEnd
 * @param size functor that for given object returns its size
 * @param value functor that for given object returns its value
 */
template <typename ObjectsIter,
         typename ObjectSizeFunctor,
         typename ObjectValueFunctor = utils::return_something_functor<int,1>>
    detail::FunctorsOnIteratorPValuePair<ObjectValueFunctor, ObjectSizeFunctor, ObjectsIter>
knapsack_0_1_no_output(
        ObjectsIter oBegin,
        ObjectsIter oEnd,
        detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        ObjectSizeFunctor size,
        ObjectValueFunctor value = ObjectValueFunctor()) {
    return detail::knapsack_check_integrality(oBegin, oEnd, capacity,
                boost::make_function_output_iterator(utils::skip_functor()),
                size, value, detail::zero_one_tag(),detail::no_retrieve_solution_tag());
}

}//paal

#endif /* KNAPSACK_0_1_HPP */
