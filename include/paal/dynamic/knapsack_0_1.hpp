/**
 * @file knapsack_0_1.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-30
 */
#ifndef KNAPSACK_0_1_HPP
#define KNAPSACK_0_1_HPP 

#include <vector>

#include <boost/range/adaptor/reversed.hpp>
#include <boost/optional.hpp>

#include "paal/utils/functors.hpp"
#include "paal/utils/type_functions.hpp"
#include "paal/utils/less_pointees.hpp"
#include "paal/dynamic/knapsack/fill_knapsack_dynamic_table.hpp"
#include "paal/dynamic/knapsack/knapsack_base.hpp"

namespace paal {

namespace detail {
    /**
     * @brief For 0/1 knapsack dynamic algorithm for given element the table has to be traversed from the highest to the lowest element
     */
    struct Knapsack_0_1_GetPositionRange {
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
        typedef KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> base;
        typedef typename base::SizeType   SizeType;
        typedef typename base::ValueType  ValueType;
        typedef typename base::ObjectType ObjectType;
        typedef typename base::ObjectRef  ObjectRef;
        typedef typename base::ReturnType  ReturnType;
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
        ReturnType solve(ObjectsIter oBegin, ObjectsIter oEnd, SizeType capacity, GetBestElement getBest) {
            m_objectOnSize.resize(capacity + 1);
            fillTable(m_objectOnSize, oBegin, oEnd, capacity);
            auto maxValue = getBest(m_objectOnSize.begin(), m_objectOnSize.end(), m_comparator);

            if(maxValue != m_objectOnSize.end()) {
                return ReturnType(**maxValue, maxValue - m_objectOnSize.begin());
            } else {
                return ReturnType(ValueType(), SizeType());
            }
        }

        //@brief here we find actual solution
        //that is, the chosen objects
        //this is done by simple divide and conquer strategy
        template <typename OutputIterator>
            void retrieveSolution(ValueType maxValue, SizeType size , ObjectsIter oBegin, ObjectsIter oEnd, OutputIterator out) {
                m_objectOnSize   .resize(size + 1);
                m_objectOnSizeRec.resize(size + 1);
                retrieveSolutionRec(maxValue, size, oBegin, oEnd, out);
            }
       

    private:
        template <typename OutputIterator>
            void retrieveSolutionRec(ValueType maxValue, SizeType capacity, ObjectsIter oBegin, ObjectsIter oEnd, OutputIterator out) {
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
                fillTable(m_objectOnSize, oBegin, midle, capacity);
                fillTable(m_objectOnSizeRec, midle, oEnd, capacity);

                SizeType capacityLeftPartInOptimalSolution = SizeType();
                for(auto capacityLeftPart : boost::irange(SizeType(), capacity + 1)) {
                    auto left  = m_objectOnSize[capacityLeftPart];
                    auto right = m_objectOnSizeRec[capacity - capacityLeftPart];
                    if(left && right) {
                        if(*left + *right == maxValue) {
                            capacityLeftPartInOptimalSolution = capacityLeftPart;
                            break;
                        }
                    }
                }
                auto left  = m_objectOnSize[capacityLeftPartInOptimalSolution];
                auto right = m_objectOnSizeRec[capacity - capacityLeftPartInOptimalSolution];
                assert(left && right && *left + *right == maxValue);

                retrieveSolutionRec(*left, capacityLeftPartInOptimalSolution, oBegin, midle, out); 
                retrieveSolutionRec(*right, capacity - capacityLeftPartInOptimalSolution, midle, oEnd, out); 
            }


        void fillTable(ValueOrNullVector & values, ObjectsIter oBegin, ObjectsIter oEnd, SizeType capacity) {
            fillKnapsackDynamicTable(values.begin(), values.begin() + capacity + 1, oBegin, oEnd,  m_size,
                    [&](ValueOrNull val, ObjectsIter obj) { return *val + m_value(*obj);},
                    [&](ValueOrNull left, ValueOrNull right) { return m_comparator(*left, *right);},
                    [](ValueOrNull & val) {val = ValueType();},
                    Knapsack_0_1_GetPositionRange());
        }

        ObjectSizeFunctor  m_size;
        ObjectValueFunctor m_value;
        Comparator m_comparator;
        ValueOrNullVector  m_objectOnSize;
        ValueOrNullVector  m_objectOnSizeRec;
    };
    
        
    //TODO produce better bound based on approximation algorithm 
    template <typename ObjectsIter,
              typename ObjectSizeFunctor, 
              typename ObjectValueFunctor>
    puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>()))
    getSizeBound_0_1(ObjectsIter oBegin, ObjectsIter oEnd, 
     puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity,
     ObjectValueFunctor value, ObjectSizeFunctor size) {
         typedef KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> base;
         typedef typename base::ObjectRef  ObjectRef;
         typedef typename base::SizeType   SizeType;
         typedef typename base::ValueType  ValueType;

         auto density = [&](ObjectRef obj){return double(value(obj))/double(size(obj));};
         auto maxElement = density(*std::max_element(oBegin, oEnd, 
                          [&](ObjectRef left, ObjectRef right){return density(left) < density(right);}));
         return capacity * maxElement;
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
          typename ObjectValueFunctor>
    std::pair<puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())),
puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>()))>
knapsack_0_1_on_size(ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value) {
    typedef puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())) ValueType;

    detail::Knapsack_0_1<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor, std::less<ValueType>> knapsack(size, value);
    auto maxValueAndSize = knapsack.solve(oBegin, oEnd, capacity, 
            detail::GetMaxElementOnCapacityIndexedCollection<ValueType>());
    knapsack.retrieveSolution(maxValueAndSize.first, maxValueAndSize.second, oBegin, oEnd, out);
    return maxValueAndSize;
}

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
          typename ObjectValueFunctor>
    std::pair<puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())),
puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>()))>
knapsack_0_1_on_value(ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value) {
    typedef puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())) ValueType;
    typedef puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) SizeType;

    detail::Knapsack_0_1<ObjectsIter, ObjectValueFunctor, ObjectSizeFunctor, std::greater<SizeType>> knapsack(value, size);
    auto maxValue = detail::getSizeBound_0_1(oBegin, oEnd, capacity, value, size);
    auto maxValueAndSize = knapsack.solve(oBegin, oEnd, maxValue, 
            detail::GetMaxElementOnValueIndexedCollection<boost::optional<SizeType>, ValueType>(boost::optional<SizeType>(capacity + 1)));
    knapsack.retrieveSolution(maxValueAndSize.first, maxValueAndSize.second, oBegin, oEnd, out);
    return std::make_pair(maxValueAndSize.second, maxValueAndSize.first);
}

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
          typename ObjectValueFunctor = utils::ReturnSomethingFunctor<int,1>>
    std::pair<puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())),
puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>()))>
knapsack_0_1(ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value = ObjectValueFunctor()) {

    typedef puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())) ValueType;
    detail::Knapsack_0_1<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor, std::less<ValueType>> knapsack(size, value);
    if(detail::getSizeBound_0_1(oBegin, oEnd, capacity, value, size) > capacity) {
        return knapsack_0_1_on_size(oBegin, oEnd, capacity, out, size, value);
    } else {
        return knapsack_0_1_on_value(oBegin, oEnd, capacity, out, size, value);
    }
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
 * @param out the result is returned using output iterator
 * @param size functor that for given object returns its size
 * @param value functor that for given object returns its value
 */
template <typename ObjectsIter, 
         typename ObjectSizeFunctor, 
         typename ObjectValueFunctor = utils::ReturnSomethingFunctor<int,1>>
             std::pair<puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())),
         puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>()))>
knapsack_0_1_no_output_on_size(ObjectsIter oBegin, 
                     ObjectsIter oEnd, 
                     puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity, //capacity is of size type
                     ObjectSizeFunctor size, 
                     ObjectValueFunctor value = ObjectValueFunctor()) {

                 typedef puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())) ValueType;
                 detail::Knapsack_0_1<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor, std::less<ValueType>> knapsack(size, value);
                 return knapsack.solve(oBegin, oEnd, capacity, 
                         detail::GetMaxElementOnCapacityIndexedCollection<ValueType>());
             }

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
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
    std::pair<puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())),
puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>()))>
knapsack_0_1_no_output_on_value(ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity, //capacity is of size type
        ObjectSizeFunctor size, 
        ObjectValueFunctor value) {
    typedef puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())) ValueType;
    typedef puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) SizeType;

    detail::Knapsack_0_1<ObjectsIter, ObjectValueFunctor, ObjectSizeFunctor, std::greater<SizeType>> knapsack(value, size);
    auto maxValue = detail::getSizeBound_0_1(oBegin, oEnd, capacity, value, size);
    auto maxValueAndSize = knapsack.solve(oBegin, oEnd, maxValue, 
            detail::GetMaxElementOnValueIndexedCollection<boost::optional<SizeType>, ValueType>(boost::optional<SizeType>(capacity + 1)));
    return std::make_pair(maxValueAndSize.second, maxValueAndSize.first);
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
 * @param out the result is returned using output iterator
 * @param size functor that for given object returns its size
 * @param value functor that for given object returns its value
 */
template <typename ObjectsIter, 
         typename ObjectSizeFunctor, 
         typename ObjectValueFunctor = utils::ReturnSomethingFunctor<int,1>>
             std::pair<puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())),
         puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>()))>
knapsack_0_1_no_output(ObjectsIter oBegin, 
                     ObjectsIter oEnd, 
                     puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity, //capacity is of size type
                     ObjectSizeFunctor size, 
                     ObjectValueFunctor value = ObjectValueFunctor()) {

    typedef puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())) ValueType;
    detail::Knapsack_0_1<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor, std::less<ValueType>> knapsack(size, value);
    if(detail::getSizeBound_0_1(oBegin, oEnd, capacity, value, size) > capacity) {
        return knapsack_0_1_no_output_on_size(oBegin, oEnd, capacity, size, value);
    } else {
        return knapsack_0_1_no_output_on_value(oBegin, oEnd, capacity, size, value);
    }
}

}//paal

#endif /* KNAPSACK_0_1_HPP */
