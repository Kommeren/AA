/**
 * @file multi_solution_step/trivial_neighbor.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef TRIVIAL_NEIGHBOR_HPP
#define TRIVIAL_NEIGHBOR_HPP

#include <utility>
#include "paal/utils/iterator_utils.hpp"
#include "paal/utils/type_functions.hpp"

namespace paal {
namespace local_search{


struct TrivialNeighborGetter {
    template <typename SolutionElement, typename Solution> 
                          std::pair<utils::IteratorWithExcludedElement<typename utils::SolToConstIter<Solution>::type>, 
                                    utils::IteratorWithExcludedElement<typename utils::SolToConstIter<Solution>::type> > 
            
            operator()(const Solution & sol, const SolutionElement &e) {
                typedef typename utils::SolToConstIter<Solution>::type SolutionIterator;
                typedef utils::IteratorWithExcludedElement<SolutionIterator> OutIter;
                SolutionIterator begin = sol.begin();
                SolutionIterator end = sol.end();
                return std::make_pair(OutIter(begin, end, e), OutIter(end, end, e));        
            }
};

} //local_search
} //paal

#endif // TRIVIAL_NEIGHBOR_HPP
