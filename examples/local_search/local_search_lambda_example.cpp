/**
 * @file local_search_lambda_example.cpp
 * @brief local search example
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-02-04
 */

#include <vector>
#include <iostream>

#include "paal/local_search/local_search.hpp"

int main() {
    //! [Local Search Example]
    namespace ls =  paal::local_search;

    auto f = [](int x) {
        return -x * x + 12 * x - 27;
    };
    int solution {0};

    const std::vector<int> neighb {10, -10, 1, -1};

    auto getMoves = [neighb](int){
        return std::make_pair(neighb.begin(), neighb.end());
    };

    auto gain = [f](int sol, int move) {
        return f(sol + move) - f(sol);
    };

    auto commit = [](int & sol, int move) {
        sol = sol + move;
        return true;
    };

    ls::local_search_simple(solution,
            ls::make_SearchComponents(getMoves, gain, commit));

    std::cout << "Local search solution: " <<  solution << std::endl;
    //! [Local Search Example]
    return 0;
}

