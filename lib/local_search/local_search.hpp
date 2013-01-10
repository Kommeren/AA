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

namespace paal {
namespace local_search {


/*
 * LocalSearch wywoluje metode search ktora przeszukuje sasiedztwo kazdego punktu i sprawdza czy da sie udoskonalic rozwiazanie.
 *
 * Zakladamy, ze rozwiazanie ma metody begin() i end()  oznaczajaca odpowiednio poczatek i koniec rozwiazania, (??? byc moze za malo ogolne ???)
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
 * SolutionUpdater - ma metode update bioraca dane rozwiazanie i nakladajace na nie odpowiednie ulepszenie
 */


template <typename Solution, typename NeighbourGetter, typename CheckIfImprove, typename SolutionUpdater> class LocalSearchStep {
public:
         
    LocalSearchStep(Solution & solution,  NeighbourGetter && ng, 
            CheckIfImprove && check, SolutionUpdater && solutionUpdater) :
     m_solution(solution), m_neighbourGetterFunctor(ng), 
     m_checkFunctor(check), m_solutionUpdaterFunctor(solutionUpdater), m_lastSearchSucceded(false) {}

    bool search() {
        m_lastSearchSucceded = false;
        std::find_if(m_solution.begin(), m_solution.end(), checkSetsForSwap);
        return m_lastSearchSucceded;
    }

private:
    template <typename SolutionEl> bool checkSetsForSwap(const SolutionEl & r) {
            auto adjustmentSet = m_neighbourGetterFunctor.getNeighbourhood(m_solution, r);
            auto findSucc = std::find_if(adjustmentSet.first, adjustmentSet.second, 
                    std::bind(std::mem_fun(&CheckIfImprove::checkIfImproved), m_checkFunctor, r, m_solution, std::placeholders::_1)); 
            if(findSucc != adjustmentSet.second) {       
                m_lastSearchSucceded = true;
                m_solutionUpdaterFunctor.update(m_solution, r, *findSucc);
            }
            return m_lastSearchSucceded;
    }

    Solution & m_solution;
    NeighbourGetter m_neighbourGetterFunctor;
    CheckIfImprove m_checkFunctor;
    SolutionUpdater m_solutionUpdaterFunctor;
    bool m_lastSearchSucceded;
};

} // local_search
} // paal

#endif // __LOCAL_SEARCH__
