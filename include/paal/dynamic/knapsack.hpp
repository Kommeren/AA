/**
 * @file knapsack.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */
#ifndef KNAPSACK_HPP
#define KNAPSACK_HPP 

#include <vector>

#include <boost/range/adaptor/reversed.hpp>
#include <boost/optional.hpp>

#include "paal/utils/functors.hpp"
#include "paal/utils/type_functions.hpp"
#include "paal/utils/iterator_utils.hpp"

namespace paal {

namespace detail {
/**
 * @brief For knapsack dynamic algorithm for given element the table has to be traversed from the lowest to highest element
 */
struct KnapsackGetPositionRange {
    template <typename T>
    auto operator()(T begin, T end) -> decltype(boost::irange(begin, end))
    {
        return boost::irange(begin, end);
    }
};

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
     * @brief Computes dynamic algorithm table (valuesBegin, valuesEnd)
     *        The values collection has element type ValueOrNull, 
     *        The  default constructed ValueOrNull should represent empty object.
     *        This collection is filled using init, compare and combine functors.
     *
     * @param valuesBegin begin of the table which will store 
     *  the values for specific positions in dynamic algorithm computation
     * @param valuesEnd
     * @param oBegin - possible object collection
     * @param oEnd
     * @param size - functor, for given opbjedt return its size
     * @param combine - for given ObjectsIter and value gives new object representing adding *ObjectsIter to value
     * @param compare - compares to values.
     * @param init - discover element and assign the 0 value
     *
     * @tparam ValueIterator has to be RandomAccess output iterator
     * @tparam ObjectsIter
     * @tparam ObjectSizeFunctor
     * @tparam Combine
     * @tparam Compare
     * @tparam Init
     * @tparam GetPositionRange
     */
template <typename ValueIterator, 
          typename ObjectsIter,
          typename ObjectSizeFunctor, 
          typename Combine,
          typename Compare, 
          typename Init,
          typename GetPositionRange>
puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>()))
fillKnapsackDynamicTable(ValueIterator valuesBegin, ValueIterator valuesEnd, 
         ObjectsIter oBegin, ObjectsIter oEnd, 
         puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity, //capacity is of size type
         ObjectSizeFunctor size, 
         Combine combine,
         Compare compare,
         Init init,
         GetPositionRange getRange) {
    typedef typename std::iterator_traits<ValueIterator>::value_type ValueOrNull;
    ValueOrNull nullVallue = ValueOrNull();
    typedef puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) SizeType;
    typedef typename std::iterator_traits<ObjectsIter>::reference ObjectRef;

    assert(std::distance(valuesBegin, valuesEnd) > capacity);
    valuesEnd = valuesBegin + capacity + 1;
    SizeType largestPosition = SizeType();
    std::fill(valuesBegin + 1, valuesEnd, nullVallue);
    init(*valuesBegin);

    for(auto objIter = oBegin; 
            objIter != oEnd; ++objIter) {
        ObjectRef obj = *objIter;
        auto objSize = size(obj);
        //for each position, from largest to smallest
        for(auto pos : getRange(0, largestPosition + 1)) {
            auto stat = *(valuesBegin + pos);
            //if position was reached before
            if(stat != nullVallue) {
                SizeType newPos = pos + objSize;
                auto & newStat = *(valuesBegin + newPos);
                //if we're not exceeding capacity
                if(newPos <= capacity) {
                    largestPosition = std::max(newPos, largestPosition);
                    auto newValue = combine(stat, objIter);
                    //if the value is bigger than previous
                    if(newStat == nullVallue || compare(newStat, newValue)) {
                        //update value
                        newStat = newValue;
                    }
                }
            }
        }
    }
    return largestPosition;
}

//TODO add to boost
//compare pointee using comparator
template<class Comparator>
struct less_pointees_t {
    less_pointees_t(Comparator compare) : m_compare(compare) {}

    template <typename OptionalPointee> 
    bool operator() ( OptionalPointee const& x, OptionalPointee const& y ) const { 
       return !y ? false : ( !x ? true : m_compare(*x, *y) ) ;
    }
private:
    Comparator m_compare;
};

template<class Comparator>
less_pointees_t<Comparator>
make_less_pointees_t(Comparator compare) {
    return less_pointees_t<Comparator>(compare);
}

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
          typename ObjectValueFunctor = utils::ReturnSomethingFunctor<int,1>>
class Knapsack_0_1 {
    typedef puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) SizeType;
    typedef puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())) ValueType;
    typedef puretype(*std::declval<ObjectsIter>()) ObjectType;
    typedef typename std::iterator_traits<ObjectsIter>::reference ObjectRef;
    typedef boost::optional<ValueType> ValueOrNull;
    static_assert(std::is_integral<SizeType>::value, "Size type must be integral");
    typedef std::vector<ValueOrNull> ValueOrNullVector;

public:

Knapsack_0_1(
         ObjectSizeFunctor size, 
         ObjectValueFunctor value = ObjectValueFunctor()) :
    m_size(size),
    m_value(value) {}

typedef std::pair<ValueType, SizeType> SolveReturn;

/**
 * @brief  Function solves dynamic programming problem
 * @returns the optimal value
 */
    SolveReturn solve(ObjectsIter oBegin, ObjectsIter oEnd, SizeType capacity) {
        m_objectOnSize.resize(capacity + 1);
        fillTable(m_objectOnSize, oBegin, oEnd, capacity);
        auto maxValue = std::max_element(m_objectOnSize.begin(), m_objectOnSize.end(), make_less_pointees_t(compare));

        if(maxValue != m_objectOnSize.end()) {
            return SolveReturn(**maxValue, maxValue - m_objectOnSize.begin());
        } else {
            return SolveReturn(ValueType(), SizeType());
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

        SizeType capacityLeftPartInOptimalSolution;
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

    static bool compare(ValueOrNull left, ValueOrNull right) {
        return *left < *right;
    }
    
    void fillTable(ValueOrNullVector & values, ObjectsIter oBegin, ObjectsIter oEnd, SizeType capacity) {
        fillKnapsackDynamicTable(values.begin(), values.end(), oBegin, oEnd, capacity,  m_size,
                [&](ValueOrNull val, ObjectsIter obj) {return *val + m_value(*obj);},
                compare,
                [](ValueOrNull & val) {val = ValueType();},
                Knapsack_0_1_GetPositionRange());
    }

    ObjectSizeFunctor  m_size;
    ObjectValueFunctor m_value;
    ValueOrNullVector  m_objectOnSize;
    ValueOrNullVector  m_objectOnSizeRec;
};
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

    detail::Knapsack_0_1<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> knapsack(size, value);
    auto maxValueAndSize = knapsack.solve(oBegin, oEnd, capacity);
    knapsack.retrieveSolution(maxValueAndSize.first, maxValueAndSize.second, oBegin, oEnd, out);
    return maxValueAndSize;
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
knapsack_0_1(ObjectsIter oBegin, 
         ObjectsIter oEnd, 
         puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity, //capacity is of size type
         ObjectSizeFunctor size, 
         ObjectValueFunctor value = ObjectValueFunctor()) {

    detail::Knapsack_0_1<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> knapsack(size, value);
    return knapsack.solve(oBegin, oEnd, capacity);
}
    
    /**
     * @brief Solution to the knapsack problem 
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
knapsack(ObjectsIter oBegin, 
         ObjectsIter oEnd, 
         puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity, //capacity is of size type
         OutputIterator out, 
         ObjectSizeFunctor size, 
         ObjectValueFunctor value = ObjectValueFunctor()) {
    typedef puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) SizeType;
    typedef puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())) ValueType;
    typedef puretype(*std::declval<ObjectsIter>()) ObjectType;
    typedef typename std::iterator_traits<ObjectsIter>::reference ObjectRef;
    typedef boost::optional<std::pair<ObjectsIter, ValueType>> ObjIterWithValueOrNull;
    static_assert(std::is_integral<SizeType>::value, "Size type must be integral");

    std::vector<ObjIterWithValueOrNull>  objectOnSize(capacity + 1);

    auto compare = [](const ObjIterWithValueOrNull & left, const ObjIterWithValueOrNull& right) {
        return left->second < right->second;
    };

    auto objectOnSizeBegin = objectOnSize.begin();
    auto objectOnSizeEnd = objectOnSizeBegin + 1 + detail::fillKnapsackDynamicTable(objectOnSizeBegin, objectOnSize.end(), 
            oBegin, oEnd, capacity, size,
            [&](ObjIterWithValueOrNull val, ObjectsIter obj) -> ObjIterWithValueOrNull  
                {return std::make_pair(obj, val->second + value(*obj));},
            compare,
            [](ObjIterWithValueOrNull & val) {val = std::make_pair(ObjectsIter(), ValueType());},
            detail::KnapsackGetPositionRange());

    //getting position of the max value in the objectOnSize array
    auto maxPos = std::max_element(objectOnSizeBegin, objectOnSizeEnd, detail::make_less_pointees_t(compare));
    
    //setting solution
    auto remainingSpaceInKnapsack = maxPos;
    while(remainingSpaceInKnapsack != objectOnSizeBegin) {
        assert(*remainingSpaceInKnapsack);
        ObjectRef obj = *((*remainingSpaceInKnapsack)->first);
        *out = obj;
        ++out;
        remainingSpaceInKnapsack -= size(obj);
    }

    typedef std::pair<ValueType, SizeType> ReturnType;

    //returning result
    if(maxPos != objectOnSizeEnd) {
        assert(*maxPos);
        return ReturnType((*maxPos)->second, maxPos - objectOnSizeBegin);
    } else {
        return ReturnType(ValueType(), SizeType());
    }
}


}//paal

#endif /* KNAPSACK_HPP */
