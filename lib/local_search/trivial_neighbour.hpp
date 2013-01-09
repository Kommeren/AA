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
        template <typename SolutionIterator, typename SolutionElement> std::pair<helpers::IteratorWithExcludedElement<SolutionIterator, SolutionElement>, 
                                                                                 helpers::IteratorWithExcludedElement<SolutionIterator, SolutionElement> > 
            
                getNeighbourhood(SolutionIterator begin, SolutionIterator end, const SolutionElement &e) const {
                    typedef helpers::IteratorWithExcludedElement<SolutionIterator, SolutionElement> OutIter;
                    return std::make_pair(OutIter(begin, end, e), OutIter(end, end, e));        
                }
};

} //local_search
} //paal

#endif // __TRIVIAL_NEIGHBOUR__
