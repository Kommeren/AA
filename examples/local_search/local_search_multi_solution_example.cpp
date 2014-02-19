/**
 * @file local_search_multi_solution_example.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include <vector>
#include <cstdlib>

#include <boost/range/irange.hpp>

#include "paal/local_search/local_search.hpp"

using namespace  paal;

//! [Local Search Components Example]
const int  DIM = 3;
const float LOWER_BOUND = 0;
const float UPPER_BOUND = 1;

//basic types
typedef float SolutionElement;
typedef std::vector<SolutionElement> Solution;
typedef SolutionElement Move;
typedef float Fitness;


//objective function
Fitness f(const Solution & x) {
    SolutionElement x1(x.at(0)), x2(x.at(1)), x3(x.at(2));
    return x1 * x2 +  x2 * x3 + x3 * x1 - 3. * x1 * x2 * x3;
}


//returns range of new possible values for given SolutionElement
struct GetMoves {
    typedef std::vector<Move> Neigh;
    typedef typename Neigh::const_iterator Iter;
    const Neigh m_neighb;
    Neigh m_neighbCut;
public:

    GetMoves() : m_neighb{.01, -.01, .001, -.001}, m_neighbCut(m_neighb.size()) {}

    std::pair<Iter, Iter> operator()(const Solution & s, SolutionElement i) {
        for(auto j : boost::irange(std::size_t(0), m_neighb.size())) {
            m_neighbCut[j] = std::max(m_neighb[j] + i, LOWER_BOUND);
            m_neighbCut[j] = std::min(m_neighbCut[j], UPPER_BOUND);
        }
        return std::make_pair(m_neighbCut.begin(), m_neighbCut.end());
    }
};


//assigns given solution element with new value computes the objective function
//and assigns the old value to the solution element
//solution remains the same
struct Gain {
    Fitness operator()(const Solution & s, SolutionElement & se, Move u) {
        SolutionElement oldSE = se;
        Fitness oldValue = f(s);
        se = u;
        Fitness newValue = f(s);
        se = oldSE;
        return newValue - oldValue;
    }
};

//assigns the solution to the given move
struct Commit {
    void operator()(Solution & s, SolutionElement & se, Move u) {
        se = u;
    }
};
//! [Local Search Components Example]

typedef  local_search::MultiSearchComponents<GetMoves, Gain, Commit> SearchComp;

//creates random 0-1 vector
void fillRand(Solution &s) {
    const int MAX_VAL = 10000;
    for(SolutionElement & el : s) {
        el = Fitness(std::rand() % MAX_VAL) / Fitness(MAX_VAL);
    }
}

int main() {
//! [Local Search Example]
    //creating local search
    Solution sol{DIM, 0};
    fillRand(sol);

    //search
    local_search::local_search_multi_solution_simple(sol, SearchComp());

    //print solution
    std::cout << "Solution: " <<  std::endl;
    std::copy(sol.begin(), sol.end(),
                std::ostream_iterator<SolutionElement>(std::cout, "\n"));
//! [Local Search Example]
    return 0;
}
