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
#include "helpers/type_functions.hpp"

namespace paal {
namespace local_search{


struct TrivialNeigbourGetter {
    template <typename SolutionElement, typename Solution> 
                          std::pair<helpers::IteratorWithExcludedElement<typename SolToIter<Solution>::type, SolutionElement>, 
                                    helpers::IteratorWithExcludedElement<typename SolToIter<Solution>::type, SolutionElement> > 
            
            gethood(Solution & sol, const SolutionElement &e) {
                typedef typename SolToIter<Solution>::type SolutionIterator;
                typedef helpers::IteratorWithExcludedElement<SolutionIterator, SolutionElement> OutIter;
                SolutionIterator begin = sol.begin();
                SolutionIterator end = sol.end();
                return std::make_pair(OutIter(begin, end, e), OutIter(end, end, e));        
            }
};

} //local_search
} //paal

#endif // __TRIVIAL_NEIGHBOUR__
