/**
 * @file trivial_neighbour.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef __TRIVIAL_NEIGHBOUR__
#define __TRIVIAL_NEIGHBOUR__

#include <utility>
#include "helpers/iterator_helpers.hpp"
#include "helpers/type_functions.hpp"

namespace paal {
namespace local_search{


struct TrivialNeigbourGetter {
    template <typename SolutionElement, typename Solution> 
                          std::pair<helpers::IteratorWithExcludedElement<typename SolToConstIter<Solution>::type, SolutionElement>, 
                                    helpers::IteratorWithExcludedElement<typename SolToConstIter<Solution>::type, SolutionElement> > 
            
            get(const Solution & sol, const SolutionElement &e) {
                typedef typename SolToConstIter<Solution>::type SolutionIterator;
                typedef helpers::IteratorWithExcludedElement<SolutionIterator, SolutionElement> OutIter;
                SolutionIterator begin = sol.begin();
                SolutionIterator end = sol.end();
                return std::make_pair(OutIter(begin, end, e), OutIter(end, end, e));        
            }
};

} //local_search
} //paal

#endif // __TRIVIAL_NEIGHBOUR__
