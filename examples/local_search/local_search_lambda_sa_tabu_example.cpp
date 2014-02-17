/**
 * @file local_search_lambda_sa_tabu_example.cpp
 * @brief local search example 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-02-04
 */

#include <vector>
#include <iostream>

#include "paal/local_search/local_search.hpp"
#include "paal/local_search/simulated_annealing.hpp"
#include "paal/data_structures/tabu_list/tabu_list.hpp"
#include "paal/local_search/custom_components.hpp"

int main() {
    //! [Local Search Example]
    namespace ls =  paal::local_search;

    auto f = [](int x) {
        return -x*x + 12 * x -27;
    };
    //creating solution
    int solution(0);

    //neighborhood
    std::vector<int> neighb{10, -10, 1, -1};

    auto getMoves = [=](int){
            return std::make_pair(neighb.begin(), neighb.end());
    };

    auto gain = [=](int sol, int move) {
        return f(sol + move) - f(sol);
    };
    
    auto commit = [](int & sol, int move) {
        sol = sol + move;
    };

    //search
    ls::local_search_simple(solution, ls::make_SearchComponents(getMoves, gain, commit));

    //print
    std::cout << "Local search solution: " <<  solution << std::endl;
    //! [Local Search Example]
  
    //simulated annealing:
    //now each move is accepted with certain probability depending on
    //the move quality and iteration id.
    solution = 0;
    auto cooling = ls::ExponentialCoolingSchemaDependantOnIteration(1000, 0.999); //this is just a functor returning double
    auto gainSA = ls::make_SimulatedAnnealingGainAdaptor(gain, cooling); // we create new gain by adopting the old one
    ls::local_search_simple(solution, ls::make_SearchComponents(getMoves, gainSA, commit));// we run local search
    
    //print
    std::cout << "Simulated annealing solution: " <<  solution << std::endl;

    int currentSolution(0);
    int best(0);
    
    //getMovesRandom returns random move
    std::default_random_engine engine;
    std::uniform_int_distribution<> dist(0,4);
    auto getMovesRandom = [=](int) mutable {
        auto iter = neighb.begin() + dist(engine);
        return std::make_pair(iter, iter + 1);
    };
    
    ls::StopConditionCountLimit stopCondition(1000);
    paal::utils::SkipFunctor skip;
 
    //this commit remembers the best solution
    //in many cases it should be also used in simulated annealing
    auto recordSolutionCommit = 
            ls::make_RecordSolutionCommitAdapter(
                    best, //the reference to the best found solution which is going to be updated during the search
                    commit,  
                    paal::utils::make_FunctorToComparator(f, paal::utils::Greater()));// recordSolutionCommit must know how to compare solutions
   
    //random walk 
    ls::local_search(currentSolution, skip, stopCondition, 
            ls::make_SearchComponents(getMovesRandom, paal::utils::ReturnOneFunctor(), recordSolutionCommit));
    
    //print
    std::cout << "Random walk solution: " << best << std::endl;
    
    //tabu search
    //one of the implementations of tau list, remembers las 20 (solution, move) pairs.

    currentSolution = 0;
    best = 0;
    auto gainTabu =  ls::make_TabuGainAdaptor(
                        paal::data_structures::TabuListRememberSolutionAndMove<int, int>(20), gain);

    
    ls::local_search<ls::search_strategies::SteepestSlope>
        (currentSolution, skip, stopCondition, ls::make_SearchComponents(getMoves, gainTabu, recordSolutionCommit));
    
    //print
    std::cout << "Tabu solution: " << best << std::endl;

    return 0; 
}

