/*
 *  * local_search.hpp
 *  *
 *  *  Created on: 03-01-2013
 *  *      Author: Piotr Wygocki
 *  */

#include <utility>
#include <algorithm>



/*
 * NeighbourGetter - musi miec opewrator bioracy iterator do rozwiazania, zwracajaca zbior SolIterRange, kazdy SolIterRange oznacza zbior z ktorym bedziem chcieli sie lapowac.
 *
 */


template <typename SolutionIter, typename NeighbourGetter, typename CheckIfImprove, typename Swapper> class LocalSearch {
public:
    typedef std::pair<SolutionIter, SolutionIter> SolIterRange;
    typedef std::pair<bool, SolIterRange> BoolAndRange;
    
        
    LocalSearch(SolIterRange range, NeighbourGetter ng, CheckIfImprove check, Swapper swap) :
     m_range(range), m_ng(ng), m_check(check), m_swap(swap), m_lastSearchSucceded(false) {}

    bool search() {
        m_lastSearchSucceded = false;
        std::find_if(m_range.first, m_range.last, checkSetsForSwap);
        return m_lastSearchSucceded;
    }

    SolIterRange getSolution() const {
        return m_range;
    }

private:
    template <typename SolutionEl> bool checkSetsForSwap(const SolutionEl & r) {
            auto setsRange = m_ng(r);
            auto findSucc = find_if(setsRange.first, setsRange.second, m_check(*setsRange.first, m_range)); 
            if(findSucc != setsRange.second) {       
                m_lastSearchSucceded = true;
                m_swap(m_range, *findSucc);
            }
            return m_lastSearchSucceded;

    }

    SolIterRange m_range;
    NeighbourGetter m_ng;
    CheckIfImprove m_check;
    Swapper m_swap;
    bool m_lastSearchSucceded;
};
