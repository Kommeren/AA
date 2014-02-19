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
struct TrivialCommit {
    /**
     * @brief We assume that operator() receives a new solution
     *
     * @tparam Solution
     * @param s
     * @param u
     */
    template <typename Solution>
    void operator()(Solution & s, const Solution & u) const {
        s = u;
    }
};
#endif /* TRIVIAL_COMMIT_HPP */
