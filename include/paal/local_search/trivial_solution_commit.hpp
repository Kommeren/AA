/**
 * @file trivial_solution_commit.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-11
 */
#ifndef TRIVIAL_COMMIT_HPP
#define TRIVIAL_COMMIT_HPP


/**
 * @brief Used in case when update is actually the new solution
 */
struct trivial_commit {
    /**
     * @brief We assume that operator() receives a new solution
     *
     * @tparam Solution
     * @param s
     * @param u
     */
    template <typename Solution>
    bool operator()(Solution & s, const Solution & u) const {
        s = u;
        return true;
    }
};
#endif /* TRIVIAL_COMMIT_HPP */
