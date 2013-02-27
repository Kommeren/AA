/**
 * @file multi_solution/trivial_neighbor.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef TRIVIAL_NEIGHBOR_HPP
#define TRIVIAL_NEIGHBOR_HPP

#include <utility>
#include "paal/helpers/iterator_helpers.hpp"
#include "paal/helpers/type_functions.hpp"

namespace paal {
namespace local_search{


struct TrivialNeigborGetter {
    template <typename SolutionElement, typename Solution> 
                          std::pair<helpers::IteratorWithExcludedElement<typename helpers::SolToConstIter<Solution>::type>, 
                                    helpers::IteratorWithExcludedElement<typename helpers::SolToConstIter<Solution>::type> > 
            
            operator()(const Solution & sol, const SolutionElement &e) {
                typedef typename helpers::SolToConstIter<Solution>::type SolutionIterator;
                typedef helpers::IteratorWithExcludedElement<SolutionIterator> OutIter;
                SolutionIterator begin = sol.begin();
                SolutionIterator end = sol.end();
                return std::make_pair(OutIter(begin, end, e), OutIter(end, end, e));        
            }
};

} //local_search
} //paal

#endif // TRIVIAL_NEIGHBOR_HPP
