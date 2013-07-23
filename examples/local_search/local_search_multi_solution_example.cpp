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

#include "paal/local_search/local_search_step.hpp"

using namespace  paal;

//! [Local Search Components Example]
const int  DIM = 3;
const float LOWER_BOUND = 0;
const float UPPER_BOUND = 1;

//basic types
typedef float SolutionElement;
typedef std::vector<SolutionElement> Solution;
typedef SolutionElement Update;
typedef float Fitness;


//objective function
Fitness f(const Solution & x) {
    SolutionElement x1(x.at(0)), x2(x.at(1)), x3(x.at(2));
    return x1 * x2 +  x2 * x3 + x3 * x1 - 3. * x1 * x2 * x3;
}


//returns range of new possible values for given SolutionElement
struct GetNeigh {
    typedef std::vector<Update> Neigh;
    typedef typename Neigh::const_iterator Iter;
    const Neigh neighb;
    Neigh neighbCut;
public:

    GetNeigh() : neighb{.01, -.01, .001, -.001}, neighbCut(neighb.size()) {}

    std::pair<Iter, Iter> operator()(const Solution & s, SolutionElement i) {
        for(int j : boost::irange(size_t(0), neighb.size())) {
            neighbCut[j] = std::max(neighb[j] + i, LOWER_BOUND);
            neighbCut[j] = std::min(neighbCut[j], UPPER_BOUND);
        }
        return std::make_pair(neighbCut.begin(), neighbCut.end());
    }
};


//assigns given solution element with new value computes the objective function
//and assigns the old value to the solution element 
//solution remains the same
struct Gain {
    Fitness operator()(const Solution & s, SolutionElement & se, Update u) {
        SolutionElement oldSE = se;
        Fitness oldValue = f(s);
        se = u;
        Fitness newValue = f(s);
        se = oldSE;
        return newValue - oldValue;
    }
};

//assigns the solution to the given update 
struct UpdateSolution {
    void operator()(Solution & s, SolutionElement & se, Update u) {
        se = u;
    }
};
//! [Local Search Components Example]

//creates random 0-1 vector
typedef  local_search::MultiSearchComponents<GetNeigh, Gain, UpdateSolution> SearchComp;
void fillRand(Solution &s) {
    const int MAX_VAL = 10000;
    for(SolutionElement & el : s) {
        el = Fitness(std::rand() % MAX_VAL) / Fitness(MAX_VAL); 
    }
}

int main() {
//! [Local Search Example]
    //creating local search
    Solution initSol{DIM, 0};
    fillRand(initSol);
    local_search::LocalSearchStepMultiSolution<
        Solution, 
        local_search::search_strategies::ChooseFirstBetter, 
        SearchComp> ls(initSol);

    //search
    search(ls); 

    //print solution
    std::cout << "Solution: " <<  std::endl;
    std::copy(ls.getSolution().begin(), ls.getSolution().end(),
                std::ostream_iterator<SolutionElement>(std::cout, "\n"));
//! [Local Search Example]
    return 0;
}
