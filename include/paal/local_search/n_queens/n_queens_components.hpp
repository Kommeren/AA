/**
 * @file n_queens_components.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-01-02
 */
#ifndef N_QUEENS_COMPONENETS_HPP
#define N_QUEENS_COMPONENETS_HPP


#include "paal/data_structures/subset_iterator.hpp"
#include <boost/iterator/function_input_iterator.hpp>
#include <boost/range/adaptor/transformed.hpp>

namespace paal {
namespace local_search {

/**
 * @brief class describing Move
 */
struct Move {
    /**
     * @brief constructor
     *
     * @param from
     * @param to
     */
    Move(int from, int to) : m_from(from), m_to(to) {}

    /**
     * @brief getter for m_from
     *
     * @return
     */
    int getFrom() const {
        return m_from;
    }

    /**
     * @brief getter for m_to
     *
     * @return
     */
    int getTo() const {
        return m_to;
    }

private:
    int m_from;
    int m_to;
};

/**
 * @brief Functor creating Move
 */
struct MakeMove {
    /**
     * @brief operator()
     *
     * @param from
     * @param to
     *
     * @return
     */
    Move operator()(int from,
                    int to) const {
        return Move(from, to);
    }
};

/**
 * @brief NQueensCommit functor
 */
struct NQueensCommit {
    template <typename Solution>
        /**
         * @brief Operator swaps elements of the solution range
         *
         * @param sol
         * @param move
         */
    bool operator()(Solution & sol, Move move) const {
        sol.swapQueens(move.getFrom(), move.getTo());
        return true;
    }
};

namespace detail {

    struct TupleToMove {
        using result_type = Move;
        result_type operator() (std::tuple<int, int> t) const {
            return Move(std::get<0>(t), std::get<1>(t));
        }
    };
}//!detail

/**
 * @brief NQueensGetMoves functor
 */
class NQueensGetMoves {


    /**
     * @brief Functor needed for type computation
     *
     * @tparam Solution
     */
    template <typename Solution>
    struct TypesEval {
        using SolutionIter = decltype(std::declval<Solution>().begin());
        using Subset = data_structures::SubsetsIterator<2, SolutionIter, MakeMove>;
        using IterPair = std::pair<Subset, Subset>;
        using Range = boost::iterator_range<Subset>;

    };

public:
    /**
     * @brief operator() returns all the elements
     *
     * @tparam Solution
     * @param solution
     *
     * @return
     */
    template <typename Solution>
    auto operator()(const Solution & solution) const ->
        typename TypesEval<Solution>::Range
    {
        return boost::make_iterator_range(data_structures::make_SubsetsIteratorRange<2>
                        (solution.begin(), solution.end(), MakeMove{}));
            ;
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
     * @param solution
     * @param move
     *
     * @return
     */
    template <typename Solution>
    int operator()(const Solution & solution, Move move) const {
        int x1 = move.getFrom();
        int y1 = solution.getY(x1);
        int x2 = move.getTo();
        int y2 = solution.getY(x2);

        return - solution.getNumAttacing(x1, y2)
               - solution.getNumAttacing(x2, y1)
               + solution.getNumAttacing(x1, y1) - 2
               + solution.getNumAttacing(x2, y2) - 2
               - 2 * (std::abs(x1 - x2) == std::abs(y1 - y2));
    }
};
} //!local_search
} //!paal


#endif /* N_QUEENS_COMPONENETS_HPP */
