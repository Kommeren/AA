/**
 * @file n_queens_components.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-01-02
 */
#ifndef N_QUEENS_COMPONENETS_HPP
#define N_QUEENS_COMPONENETS_HPP


namespace paal {
namespace local_search {

/**
 * @brief NQueensCommit functor
 */
struct NQueensCommit {
    template <typename Solution, typename Idx>
        /**
         * @brief Operator swaps elements of the solution range
         *
         * @param sol
         * @param solutionElement
         * @param move
         */
    void operator()(Solution & sol, Idx solutionElement, Idx move) const {
        sol.swapQueens(solutionElement, move);
    }
};

/**
 * @brief NQueensGetMoves functor
 */
struct NQueensGetMoves {
    /**
     * @brief operator returns all the elements
     *
     * @tparam Solution
     * @tparam Idx
     * @param solution
     *
     * @return
     */
    template <typename Solution, typename Idx>
    auto operator()(const Solution & solution, Idx idx) const ->
            std::pair<decltype(solution.begin()), decltype(solution.end())> {
        return std::make_pair(solution.begin() + idx + 1, solution.end());
    }
};

/**
 * @brief NQueensGain functor
 */
struct NQueensGain {
    /**
     * @brief computes difference in cost
     *
     * @tparam Solution
     * @tparam Idx
     * @param solution
     * @param solutionElement
     * @param move
     *
     * @return
     */
    template <typename Solution, typename Idx>
    int operator()(const Solution & solution, Idx solutionElement, Idx move) const {
        int x1 = solutionElement;
        int y1 = solution.getY(solutionElement);
        int x2 = move;
        int y2 = solution.getY(move);

        return - solution.getNumAttacing(x1, y2)
               - solution.getNumAttacing(x2, y1)
               + solution.getNumAttacing(x1, y1) - 2
               + solution.getNumAttacing(x2, y2) - 2
               - 2 * (std::abs(x1 - x2) == std::abs(y1 - y2));
    }
};
} //!local_search
} // !// paal


#endif /* N_QUEENS_COMPONENETS_HPP */
