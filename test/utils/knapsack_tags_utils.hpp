/**
 * @file knapsack_tags_utils.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-11-05
 */
#ifndef KNAPSACK_TAGS_UTILS_HPP
#define KNAPSACK_TAGS_UTILS_HPP 
namespace pd = paal::detail;

template <typename T>
std::string to_string(T) {
    return "";
}

std::string to_string(pd::IntegralValueAndSizeTag) {
    return "value and size";
}

std::string to_string(pd::IntegralValueTag) {
    return "value";
}

std::string to_string(pd::IntegralSizeTag) {
    return "size";
}

std::string to_string(pd::NoRetrieveSolutionTag) {
    return "without output";
}

std::string to_string(pd::ZeroOneTag) {
    return "0/1";
}

struct FPTASTag{};
struct NoFPTASTag{};

template <typename IntegralTag, 
          typename IsZeroOne, 
          typename RetrieveSolution = pd::RetrieveSolutionTag,
          typename IsFPTAS = NoFPTASTag,
          typename Result,
          typename Objects, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename paal::detail::KnapsackBase<typename paal::utils::CollectionToIter<Objects>::type,
    ObjectSizeFunctor, ObjectValueFunctor>::ReturnType 
    detail_knapsack(const Objects & objects, 
                     paal::detail::FunctorOnIteratorPValue<ObjectSizeFunctor, 
                        typename paal::utils::CollectionToIter<Objects>::type> capacity, //capacity is of size type
                        Result result,
                     ObjectSizeFunctor size, 
                     ObjectValueFunctor value) 
{
    LOGLN("Knapsack " << to_string(IsZeroOne())  <<  " on " + to_string(IntegralTag()) + " " + to_string(RetrieveSolution()));
    return pd::knapsack(std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            size, 
            value,
            IsZeroOne(),
            IntegralTag(),
            RetrieveSolution());
}

#endif /* KNAPSACK_TAGS_UTILS_HPP */
