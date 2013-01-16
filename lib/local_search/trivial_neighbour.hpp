/*
 *  * trivial_neighbour.hpp
 *  *
 *  *  Created on: 03-01-2013
 *  *      Author: Piotr Wygocki
 *  */

#ifndef __TRIVIAL_NEIGHBOUR__
#define __TRIVIAL_NEIGHBOUR__

#include <utility>
#include "helpers/iterator_helpers.hpp"

namespace paal {
namespace local_search{


struct TrivialNeigbourGetter {
        template <typename SolutionElement, typename Solution> 
                              std::pair<helpers::IteratorWithExcludedElement<decltype(std::declval<Solution>().cbegin()), SolutionElement>, 
                                        helpers::IteratorWithExcludedElement<decltype(std::declval<Solution>().cbegin()), SolutionElement> > 
            
                getNeighbourhood(const Solution & sol, const SolutionElement &e) const {
                    typedef decltype(std::declval<Solution>().cbegin()) SolutionIterator;
                    typedef helpers::IteratorWithExcludedElement<SolutionIterator, SolutionElement> OutIter;
                    SolutionIterator begin = sol.cbegin();
                    SolutionIterator end = sol.cend();
                    return std::make_pair(OutIter(begin, end, e), OutIter(end, end, e));        
                }
};



/*template <typename SolutionElement, typename Solution> 
struct TrivialNeigbourGetter2 {
                typedef decltype(std::declval<Solution>().cbegin()) SolutionIterator;
                typedef helpers::IteratorWithExcludedElement<SolutionIterator, SolutionElement> OutIter;

                std::pair<OutIter, OutIter> 
                getNeighbourhood(const Solution & sol, const SolutionElement &e) const {
                    SolutionIterator begin = sol.cbegin();
                    SolutionIterator end = sol.cend();
                    return std::make_pair(OutIter(begin, end, e), OutIter(end, end, e));        
                }
};*/

} //local_search
} //paal

#endif // __TRIVIAL_NEIGHBOUR__
