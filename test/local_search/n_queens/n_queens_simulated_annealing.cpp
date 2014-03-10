/**
 * @file n_queens_simulated_annealing.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-01-04
 */

#define BOOST_RESULT_OF_USE_DECLTYPE

#include <iostream>

#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/iterator/function_input_iterator.hpp>

#include "paal/local_search/n_queens/n_queens_local_search.hpp"
#include "paal/local_search/simulated_annealing.hpp"

#include "paal/data_structures/components/components_replace.hpp"
#include "paal/local_search/local_search.hpp"
#include "utils/logger.hpp"



//This is needed because of boost bug in function_input_iterator!!!
    template <typename F>
    struct RandomMoves {
        RandomMoves(F _f) : f(_f) {}
        typedef paal::local_search::Move result_type;

        result_type operator()() const {
            return f();
        }

        mutable F f;
    };

/**
 * @brief NQueensCommit functor, optimized fo SA puropses
 */
struct NQueensCommitSA {
    template <typename Solution, typename Idx>
        /**
         * @brief Operator swaps elements of the solution range
         *
         * @param sol
         * @param solutionElement
         * @param move
         */
    bool operator()(Solution & sol, Idx solutionElement, Idx move) const {
        sol.swapQueens(solutionElement, move);
        return false;
    }
};

int main(int argc, char ** argv) {
    if(argc != 2) {
        std::cout << argv[0] <<  " " << "number_of_queens"  << std::endl;
        return 1;
    }

    const int number_of_queens = std::stoi(argv[1]);

    namespace ls = paal::local_search;
    typedef ls::NQueensSolutionAdapter<std::vector<int>> Adapter;
    std::vector<int> queens(number_of_queens);
    boost::iota(queens, 0);

    std::default_random_engine rand;
    std::uniform_int_distribution<int> distribution(0,number_of_queens);

    //TODO add multisolution -> single solution
    auto gain = ls::NQueensGain{};
    auto commit = [](Adapter & sol, ls::Move m){return NQueensCommitSA()(sol, m.getFrom(), m.getTo());};
    auto randomMove = [=]() mutable {
                            return ls::Move(distribution(rand),
                                            distribution(rand));
    };

    RandomMoves<decltype(randomMove)> r(randomMove);

    auto movesBegin = boost::make_function_input_iterator(r, 0);
    auto movesEnd = boost::make_function_input_iterator(r, 10000000);

    auto getMoves = [=](Adapter & sol)
        {return std::make_pair(movesBegin, movesEnd);};

    int nr_of_iterations(0);
    auto countingGain = paal::utils::make_CountingFunctorAdaptor(gain, nr_of_iterations);

    //alternative cooling strategy
    ///auto coolingStrategy =  ls::make_ExponentialCoolingSchemaDependantOnTime(std::chrono::seconds(10), 100, 0.1);
    auto coolingStrategy = ls::ExponentialCoolingSchemaDependantOnIteration(10, 0.9999, 1000);
    auto saGain = ls::make_SimulatedAnnealingGainAdaptor(countingGain, coolingStrategy);
    auto comps = ls::make_SearchComponents(getMoves, saGain, commit);

    Adapter adaptor(queens);
    ls::local_search_simple(adaptor, comps);
    std::cout <<  Adapter(queens).objFun() << " " << nr_of_iterations << std::endl;

    return 0;
}



