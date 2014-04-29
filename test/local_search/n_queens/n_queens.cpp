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
    std::string strategy = "FirstBetter";
    if(argc != 2 && argc != 3) {
        std::cout << argv[0] <<  " " << "number_of_queens strategy=FirstBetter"  << std::endl;
        return 1;
    } else if(argc == 3) {
        strategy = argv[2];
        assert(strategy == "FirstBetter" || strategy == "SteepestSlope");
    }
    const int number_of_queens = std::stoi(argv[1]);

    namespace ls = paal::local_search;
    typedef ls::n_queens_solution_adapter<std::vector<int>> Adapter;
    std::vector<int> queens(number_of_queens);
    boost::iota(queens, 0);

    ls::n_queens_local_search_components<> comps;
    int nr_of_iterations(0);
    auto countingGain = paal::utils::make_counting_functor_adaptor(comps.get<ls::Gain>(), nr_of_iterations);
    auto countingComps = paal::data_structures::replace<ls::Gain>(countingGain, comps);

    if(strategy == "FirstBetter") {
        ls::n_queens_solution_first_improving(queens, countingComps);
    } else {
        paal::utils::always_false nop;
        ls::n_queens_solution_local_search(queens, ls::best_improving_strategy{}, nop, nop, countingComps);
    }
    std::cout <<  Adapter(queens).obj_fun() << " " << nr_of_iterations << std::endl;

    return 0;
}



