/**
 * @file fill_knapsack_dynamic_table.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-29
 */
#ifndef FILL_KNAPSACK_DYNAMIC_TABLE_HPP
#define FILL_KNAPSACK_DYNAMIC_TABLE_HPP 

namespace paal {
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
         ObjectSizeFunctor size, 
         Combine combine,
         Compare compare,
         Init init,
         GetPositionRange getRange) {
    typedef typename std::iterator_traits<ValueIterator>::value_type ValueOrNull;
    ValueOrNull nullVallue = ValueOrNull();
    typedef puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) SizeType;
    typedef typename std::iterator_traits<ObjectsIter>::reference ObjectRef;

    SizeType maxSize = std::distance(valuesBegin, valuesEnd);

    std::fill(valuesBegin + 1, valuesEnd, nullVallue);
    init(*valuesBegin);

    auto posRange = getRange(0, maxSize);
    for(auto objIter = oBegin; 
            objIter != oEnd; ++objIter) {
        ObjectRef obj = *objIter;
        auto objSize = size(obj);
        //for each position, from largest to smallest
        for(auto pos : posRange) {
            auto stat = *(valuesBegin + pos);
            //if position was reached before
            if(stat != nullVallue) {
                SizeType newPos = pos + objSize;
                auto & newStat = *(valuesBegin + newPos);
                //if we're not exceeding maxSize
                if(newPos < maxSize) {
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
    return maxSize - 1;
}

} //paal
#endif /* FILL_KNAPSACK_DYNAMIC_TABLE_HPP */
