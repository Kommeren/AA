/**
 * @file knapsack_common.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-30
 */
#ifndef KNAPSACK_COMMON_HPP
#define KNAPSACK_COMMON_HPP

namespace paal {
namespace detail {

// this overloads checks if SizeType and ValueType are integral
template <typename ObjectsIter, typename OutputIterator,
          typename ObjectSizeFunctor, typename ObjectValueFunctor,
          typename Is_0_1_Tag,
          typename RetrieveSolution = retrieve_solution_tag>
FunctorsOnIteratorPValuePair<ObjectValueFunctor, ObjectSizeFunctor, ObjectsIter>
knapsack_check_integrality(
    ObjectsIter oBegin, ObjectsIter oEnd,
    FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter>
        capacity, // capacity is of size type
    OutputIterator out, ObjectSizeFunctor size, ObjectValueFunctor value,
    Is_0_1_Tag is_0_1_Tag,
    RetrieveSolution retrieve_solutionTag = RetrieveSolution()) {

    typedef FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> SizeType;
    typedef FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter> ValueType;
    return knapsack(oBegin, oEnd, capacity, out, size, value, is_0_1_Tag,
                    detail::GetIntegralTag<SizeType, ValueType>(),
                    retrieve_solutionTag);
}

// this overloads is for nonintegral SizeType and ValueType
// this case is invalid and allwas asserts!
template <typename ObjectsIter, typename OutputIterator,
          typename ObjectSizeFunctor, typename ObjectValueFunctor,
          typename IntegralTag, // always equals non_integral_value_and_size_tag
          typename RetrieveSolution, typename Is_0_1_Tag>
FunctorsOnIteratorPValuePair<ObjectValueFunctor, ObjectSizeFunctor, ObjectsIter>
knapsack(ObjectsIter oBegin, ObjectsIter oEnd,
         FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter>
             capacity, // capacity is of size type
         OutputIterator out, ObjectSizeFunctor size, ObjectValueFunctor value,
         Is_0_1_Tag is_0_1_Tag, non_integral_value_and_size_tag,
         RetrieveSolution retrieve_solution) {
    // trick to avoid checking assert on template definition parse
    static_assert(
        std::is_same<IntegralTag, non_integral_value_and_size_tag>::value,
        "At least one of the value or size must return integral value");
}

/**
 * @brief Solution to Knapsack  problem
 *  overload for integral Size and Value case
 */
template <typename ObjectsIter, typename OutputIterator,
          typename ObjectSizeFunctor, typename ObjectValueFunctor,
          typename Is_0_1_Tag, typename RetrieveSolution>
FunctorsOnIteratorPValuePair<ObjectValueFunctor, ObjectSizeFunctor, ObjectsIter>
knapsack(ObjectsIter oBegin, ObjectsIter oEnd,
         FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter>
             capacity, // capacity is of size type
         OutputIterator out, ObjectSizeFunctor size, ObjectValueFunctor value,
         Is_0_1_Tag is_0_1_Tag, integral_value_and_size_tag,
         RetrieveSolution retrieve_solutionTag) {
    if (get_value_upper_bound(oBegin, oEnd, capacity, value, size, is_0_1_Tag) >
        capacity) {
        return knapsack(oBegin, oEnd, capacity, out, size, value, is_0_1_Tag,
                        integral_size_tag(), retrieve_solutionTag);
    } else {
        return knapsack(oBegin, oEnd, capacity, out, size, value, is_0_1_Tag,
                        integral_value_tag(), retrieve_solutionTag);
    }
}

} //! detail
} //! paal
#endif /* KNAPSACK_COMMON_HPP */
