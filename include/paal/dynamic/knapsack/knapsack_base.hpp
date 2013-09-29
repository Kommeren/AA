/**
 * @file knapsack_base.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-30
 */
#ifndef KNAPSACK_BASE_HPP
#define KNAPSACK_BASE_HPP 
namespace paal {
namespace detail {

    //definition of basic types
    template <typename ObjectsIter, 
             typename ObjectSizeFunctor, 
             typename ObjectValueFunctor>
    struct KnapsackBase {
        typedef puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) SizeType;
        typedef puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>())) ValueType;
        typedef puretype(*std::declval<ObjectsIter>()) ObjectType;
        typedef typename std::iterator_traits<ObjectsIter>::reference ObjectRef;
        typedef std::pair<ValueType, SizeType> ReturnType;
        static_assert(std::is_integral<SizeType>::value, "Size type must be integral");
    };

    //if the knapsack dynamic table is indexed by values,
    //the procedure to find the best element is to find the biggest index i in the table that
    // *i is smaller than given threshold(capacity)
    template <typename MaxValueType, typename SizeType>
    struct GetMaxElementOnValueIndexedCollection {
        GetMaxElementOnValueIndexedCollection(MaxValueType maxValue) :
            m_maxValue(maxValue) {}

        template <typename Iterator, typename Comparator>
            Iterator operator()(Iterator begin, Iterator end, Comparator compare) {
                typedef std::pair<Iterator, SizeType> Return;
                auto compareOpt = make_less_pointees_t(compare);
                //traverse in reverse order, skip the first
                for(auto iter = end - 1; iter != begin; --iter ) {
                    if(*iter && compareOpt(m_maxValue, *iter)) {
                        return iter;
                    }
                }

                return end;
            }
    private:
        MaxValueType m_maxValue;
    };
    
    //if the knapsack dynamic table is indexed by sizes,
    //the procedure to find the best element is to find the biggest 
    //index i in the table that maximizes *i
    template <typename ValueType>
    struct GetMaxElementOnCapacityIndexedCollection {
        template <typename Iterator, typename Comparator>
            Iterator operator()(Iterator begin, Iterator end, Comparator compare) {
                return std::max_element(begin, end, make_less_pointees_t(compare));
            }
    };



}//detail
}//paal
#endif /* KNAPSACK_BASE_HPP */
