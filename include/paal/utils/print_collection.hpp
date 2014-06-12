/**
 * @file print_collection.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-06-12
 */

#ifndef PRINT_COLLECTION_HPP
#define PRINT_COLLECTION_HPP

#include <boost/range/empty.hpp>
#include <boost/range.hpp>

namespace paal {

    /**
     * @brief prints collection with delimiters without trailing delimiter
     *
     * @tparam Range
     * @tparam Stream
     * @param o
     * @param r
     * @param del
     */
template <typename Range, typename Stream>
void print_collection(Stream & o, Range && r, const std::string & del) {
   if(boost::empty(r)) {
       return;
   }
   auto b = std::begin(r);
   auto e = std::end(r);
   o << *b;
   for(auto && x : boost::make_iterator_range(++b, e) ) {
       o << del << x;
   }
}

}//!paal


#endif /* PRINT_COLLECTION_HPP */
