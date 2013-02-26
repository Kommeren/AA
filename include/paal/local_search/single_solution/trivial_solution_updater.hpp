/**
 * @file trivial_solution_operator()r.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-11
 */
#ifndef TRIVIAL_SOLUTION_UPDATER_HPP
#define TRIVIAL_SOLUTION_UPDATER_HPP 


/**
 * @brief We assume that operator() is a new solution
 */
struct TrivialSolutionUpdater {
    template <typename Solution> void operator()(Solution & s, const Solution & u) const {
        s = u;
    }
};
#endif /* TRIVIAL_SOLUTION_UPDATER_HPP */
