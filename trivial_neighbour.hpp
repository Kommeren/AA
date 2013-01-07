/*
 *  * trivial_neighbour.hpp
 *  *
 *  *  Created on: 03-01-2013
 *  *      Author: Piotr Wygocki
 *  */
#include <utility>
#include <iterator_helpers.hpp>


template <typename SolutionIterator, typename SolutionElement> class TrivialNeigbourGetter {
    typedef IteratorWithExcludedElement<SolutionIterator, SolutionElement> OutIter;
    public:

        std::pair<OutIter, OutIter> getNeighbourhood(std::pair<SolutionIterator,SolutionIterator> sr, const SolutionElement &e) const {
            return std::make_pair(OutIter(sr.first, sr.second, e), OutIter(sr.second, sr.second, e));        
        }
};
