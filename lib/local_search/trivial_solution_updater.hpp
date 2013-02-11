/**
 * @file trivial_solution_updater.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-11
 */
#ifndef TRIVIAL_SOLUTION_UPDATER_HPP
#define TRIVIAL_SOLUTION_UPDATER_HPP 


/**
 * @brief We assume that update is a new solution
 */
struct TrivialSolutionUpdater {
    template <typename Solution> void update(Solution & s, const Solution & u) const {
        s = u;
    }
};
#endif /* TRIVIAL_SOLUTION_UPDATER_HPP */
