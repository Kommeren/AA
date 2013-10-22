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
#include "paal/utils/less_pointees.hpp"
#include "paal/utils/knapsack_utils.hpp"
#include "paal/dynamic/knapsack/fill_knapsack_dynamic_table.hpp"
#include "paal/dynamic/knapsack/knapsack_base.hpp"
#include "paal/greedy/knapsack_0_1_two_app.hpp"

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
            void retrieveSolution(ValueType maxValue, SizeType size , ObjectsIter oBegin, ObjectsIter oEnd, OutputIterator out) const {
                m_objectOnSize   .resize(size + 1);
                m_objectOnSizeRec.resize(size + 1);
                retrieveSolutionRec(maxValue, size, oBegin, oEnd, out);
            }
       

    private:
        template <typename OutputIterator>
            void retrieveSolutionRec(ValueType maxValue, SizeType capacity, ObjectsIter oBegin, ObjectsIter oEnd, OutputIterator out) const {
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


        void fillTable(ValueOrNullVector & values, ObjectsIter oBegin, ObjectsIter oEnd, SizeType capacity) const {
            fillKnapsackDynamicTable(values.begin(), values.begin() + capacity + 1, oBegin, oEnd,  m_size,
                    [&](ValueOrNull val, ObjectsIter obj) { return *val + m_value(*obj);},
                    [&](ValueOrNull left, ValueOrNull right) { return m_comparator(*left, *right);},
                    [](ValueOrNull & val) {val = ValueType();},
                    Knapsack_0_1_GetPositionRange());
        }

        ObjectSizeFunctor  m_size;
        ObjectValueFunctor m_value;
        Comparator m_comparator;
        mutable ValueOrNullVector  m_objectOnSize;
        mutable ValueOrNullVector  m_objectOnSizeRec;
    };
    
        
    
    template <typename ObjectsIter,
              typename ObjectSizeFunctor, 
              typename ObjectValueFunctor>
    puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>()))
    getValueBound_0_1(ObjectsIter oBegin, ObjectsIter oEnd, 
     puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity,
     ObjectValueFunctor value, ObjectSizeFunctor size, ArithmeticSizeTag) {
         auto out = utils::make_FunctorToOutputIterator(utils::SkipFunctor());

         return std::min(2 * knapsack_0_1_two_app(oBegin, oEnd, capacity, out, value, size).first,
                         getDensityValueBound(oBegin, oEnd, capacity, value, size));
    }
    
    template <typename ObjectsIter,
              typename ObjectSizeFunctor, 
              typename ObjectValueFunctor>
    puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>()))
    getValueBound_0_1(ObjectsIter oBegin, ObjectsIter oEnd, 
     puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity,
     ObjectValueFunctor value, ObjectSizeFunctor size, NonIntegralValueAndSizeTag) {
         return getDensityValueBound(oBegin, oEnd, capacity, value, size);
    }

    template <typename ObjectsIter,
              typename ObjectSizeFunctor, 
              typename ObjectValueFunctor>
    puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>()))
    getValueBound_0_1(ObjectsIter oBegin, ObjectsIter oEnd, 
     puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity,
     ObjectValueFunctor value, ObjectSizeFunctor size) {
         typedef puretype(std::declval<ObjectSizeFunctor >()(*std::declval<ObjectsIter>())) SizeType;
         return getValueBound_0_1(oBegin, oEnd, capacity, value, size, GetArithmeticSizeTag<SizeType>());
    }


} //detail 




namespace detail {

struct RetrieveSolution {
    template <typename Knapsack, typename IndexType, typename ValueType,
              typename ObjectsIter, typename OutputIterator>
    void operator()(const Knapsack & knapsack,
                    ValueType maxValue,
                    IndexType size,
                    ObjectsIter oBegin, 
                    ObjectsIter oEnd, 
                    OutputIterator out 
            ) const {
        knapsack.retrieveSolution(maxValue, size, oBegin, oEnd, out);
    }
};

/**
 * @brief Solution to Knapsack 0/1 problem
 *  overload for integral Size case
 */
template <typename ObjectsIter, 
          typename OutputIterator, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor, 
          typename RetrieveSolution>
    std::pair<puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())),
              puretype(std::declval<ObjectSizeFunctor >()(*std::declval<ObjectsIter>()))>
knapsack_0_1(ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value, 
        IntegralSizeTag,
        RetrieveSolution retrieveSolution) {
    typedef puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())) ValueType;

    detail::Knapsack_0_1<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor, std::less<ValueType>> knapsack(size, value);
    auto maxValueAndSize = knapsack.solve(oBegin, oEnd, capacity, 
            detail::GetMaxElementOnCapacityIndexedCollection<ValueType>());
    retrieveSolution(knapsack, maxValueAndSize.first, maxValueAndSize.second, oBegin, oEnd, out);
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
          typename RetrieveSolution>
    std::pair<puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())),
puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>()))>
knapsack_0_1(ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value,
        IntegralValueTag,
        RetrieveSolution retrieveSolution) {
    typedef puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())) ValueType;
    typedef puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) SizeType;

    detail::Knapsack_0_1<ObjectsIter, ObjectValueFunctor, ObjectSizeFunctor, std::greater<SizeType>> knapsack(value, size);
    auto maxValue = detail::getValueBound_0_1(oBegin, oEnd, capacity, value, size);
    auto maxValueAndSize = knapsack.solve(oBegin, oEnd, maxValue, 
            detail::GetMaxElementOnValueIndexedCollection<boost::optional<SizeType>, ValueType>(boost::optional<SizeType>(capacity + 1)));
    retrieveSolution(knapsack, maxValueAndSize.first, maxValueAndSize.second, oBegin, oEnd, out);
    return std::make_pair(maxValueAndSize.second, maxValueAndSize.first);
}


/**
 * @brief Solution to Knapsack 0/1 problem
 *  overload for integral Size and Value case
 */
template <typename ObjectsIter, 
          typename OutputIterator, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor,
          typename RetrieveSolution>
    std::pair<puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())),
puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>()))>
knapsack_0_1(ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value, 
        IntegralValueAndSizeTag,
        RetrieveSolution retrieveSolution) {

    typedef puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())) ValueType;
    detail::Knapsack_0_1<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor, std::less<ValueType>> knapsack(size, value);
    if(detail::getValueBound_0_1(oBegin, oEnd, capacity, value, size) > capacity) {
        return knapsack_0_1(oBegin, oEnd, capacity, out, size, value, IntegralSizeTag(), retrieveSolution);
    } else {
        return knapsack_0_1(oBegin, oEnd, capacity, out, size, value, IntegralValueTag(), retrieveSolution);
    }
}

template <typename ObjectsIter, 
          typename OutputIterator, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor,
          typename IntegralTag, //always equals NonIntegralValueAndSizeTag
          typename RetrieveSolution
          >
    std::pair<puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())),
puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>()))>
knapsack_0_1(ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value, 
        NonIntegralValueAndSizeTag,
        RetrieveSolution retrieveSolution) {
    //trick to avoid checking assert on template definition parse
    static_assert(std::is_same<IntegralTag, NonIntegralValueAndSizeTag>::value, 
            "At least one of the value or size must return integral value");
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
    typedef puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) SizeType;
    return detail::knapsack_0_1(oBegin, oEnd, capacity, out, size, value, 
                detail::GetIntegralTag<SizeType, ValueType>(),
                detail::RetrieveSolution());
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
         typename ObjectValueFunctor = utils::ReturnSomethingFunctor<int,1>>
             std::pair<puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())),
         puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>()))>
knapsack_0_1_no_output(ObjectsIter oBegin, 
                     ObjectsIter oEnd, 
                     puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity, //capacity is of size type
                     ObjectSizeFunctor size, 
                     ObjectValueFunctor value = ObjectValueFunctor()) {
    typedef puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())) ValueType;
    typedef puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) SizeType;
    return detail::knapsack_0_1(
                oBegin, oEnd, capacity, 
                utils::make_FunctorToOutputIterator(utils::SkipFunctor()), 
                size, value, 
                detail::GetIntegralTag<SizeType, ValueType>(),
                utils::SkipFunctor());
}

}//paal

#endif /* KNAPSACK_0_1_HPP */
