/*
 *  * local_search.hpp
 *  *
 *  *  Created on: 03-01-2013
 *  *      Author: Piotr Wygocki
 *  */

#ifndef __LOCAL_SEARCH__
#define __LOCAL_SEARCH__

#include <utility>
#include <algorithm>


/*
 * LocalSearch wywoluje metode search ktora przeszukuje sasiedztwo kazdego punktu i sprawdza czy da sie udoskonalic rozwiazanie.
 *
 * Zakladamy, ze rozwiazanie to para iteratorow oznaczajaca odpowiednio poczatek i koniec rozwiazania, (??? byc moze za malo ogolne ???)
 * w przypadku TSP byl by to poczatek i koniec listy wierzcholkow (ewentualnie krawedzi) w sciezce komiwojazera, w drzewie steinera byla by to lista  krawedzi drzewa, w FL lista fabryk. Oczywiscie moze to byc iterator po czymkkolwiek na przyklad po kolejncyhparach krawedzie z cyklu...
 *
 * W sasiedztwie kazdego elementu rozwiazania (to sasiedztwo w szzcegolnym przypadku moze byc oczywiscie calym grafem) szukamy ulepszen tego rozwiazania. W najprostrzym wariancie TSP bedzie to po prostu zbior wszystkich krawedzi z optymalnej sciezki
 *
 *
 * Wejscie : 
 * SolIterRange - rozwiaznie wejsciowe
 *
 * NeighbourGetter - musi miec metode getNeighbourhood() bioracy aktualna trase i element rozwiazania, zwracajaca pare iteratorow symbolizujaca zbior ulepszen. 
 *
 * CheckIfImprove - musi miec metode checkIfImproved bioraca dane  rozwiazanie i ulepszenie ktora sprawdza czy dzieki danemu ulepszeniu da sie poprawic aktualne rozwiazanie
 *
 * Swapper - ma metode swap bioraca dane rozwiazanie i nakladajace na nie odpowiednie ulepszenie
 */


template <typename SolutionIter, typename NeighbourGetter, typename CheckIfImprove, typename Swapper> class LocalSearchStep {
public:
    typedef std::pair<SolutionIter, SolutionIter> SolIterRange;
//    typedef std::pair<bool, SolIterRange> BoolAndRange;
         
    LocalSearchStep(SolutionIter solutionBegin ,SolutionIter solutionEnd
            , NeighbourGetter && ng, CheckIfImprove && check, Swapper && swap) :
     m_solutionBegin(solutionBegin), m_solutionEnd(solutionEnd), m_neighbourGetterFunctor(ng), 
     m_checkFunctor(check), m_swapFunctor(swap), m_lastSearchSucceded(false) {}

    bool search() {
        m_lastSearchSucceded = false;
        std::find_if(m_solutionBegin, m_solutionEnd, checkSetsForSwap);
        return m_lastSearchSucceded;
    }

    SolIterRange getSolution() const {
        return SolIterRange(m_solutionBegin, m_solutionEnd);
    }

private:
    template <typename SolutionEl> bool checkSetsForSwap(const SolutionEl & r) {
            auto adjustmentSet = m_neighbourGetterFunctor.getNeighbourhood(m_solutionBegin, m_solutionEnd, r);
            auto findSucc = std::find_if(adjustmentSet.first, adjustmentSet.second, 
                    std::bind(std::mem_fun(&CheckIfImprove::checkIfImproved), m_checkFunctor, r, m_solutionBegin, m_solutionEnd, std::placeholders::_1)); 
            if(findSucc != adjustmentSet.second) {       
                m_lastSearchSucceded = true;
                tie(m_solutionBegin, m_solutionEnd) = m_swapFunctor.swap(m_solutionBegin, m_solutionEnd, r, *findSucc);
            }
            return m_lastSearchSucceded;
    }

    SolutionIter m_solutionBegin;
    SolIterRange m_solutionEnd;
    NeighbourGetter m_neighbourGetterFunctor;
    CheckIfImprove m_checkFunctor;
    Swapper m_swapFunctor;
    bool m_lastSearchSucceded;
};

#endif // __LOCAL_SEARCH__
