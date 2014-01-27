/**
 * @file n_queens_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-01-04
 */

#include <iostream>

#include <boost/range/algorithm_ext/iota.hpp>
#include "paal/local_search/n_queens/n_queens_local_search.hpp"

#include "paal/data_structures/components/components_replace.hpp"
#include "utils/logger.hpp"


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

    ls::NQueensLocalSearchComponents<> comps;
    int nr_of_iterations(0);
    auto countingGain = paal::utils::make_CountingFunctorAdaptor(comps.get<ls::Gain>(), nr_of_iterations);
    auto countingComps = paal::data_structures::replace<ls::Gain>(countingGain, comps);

    ls::nQueensSolutionLocalSearchSimple(queens, countingComps);
    std::cout <<  Adapter(queens).objFun() << " " << nr_of_iterations << std::endl;

    return 0;
}


