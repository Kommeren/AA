/**
 * @file tabu_list.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-01-09
 */
#ifndef TABU_LIST_HPP
#define TABU_LIST_HPP

#include <unordered_set>
#include <queue>

#include <boost/functional/hash.hpp>

namespace paal {
namespace data_structures {

/**
 * @brief This Tabu list remember some number of last moves
 *
 * @tparam Move
 */
template <typename Move>
struct TabuListRememberMove {

    /**
     * @brief TabuListRememberMove constructor
     *
     * @param size
     */
    TabuListRememberMove(unsigned size) :
        m_size(size), m_forbidenMovesSet(size) {
        }

    /**
     * @brief is tabu member function
     *
     * @tparam Solution
     * @param move
     *
     * @return
     */
    template <typename Solution>
    bool isTabu(const Solution &, Move move) const {
        return isTabu(std::move(move));
    }

    /**
     * @brief accept member function
     *
     * @tparam Solution
     * @param move
     */
    template <typename Solution>
    void accept(const Solution &, Move move) {
        assert(!isTabu(move));
        m_forbidenMovesSet.insert(move);
        if(m_forbidenMovesFIFO.size() == m_size) {
            m_forbidenMovesSet.erase(m_forbidenMovesFIFO.front());
            m_forbidenMovesFIFO.pop_front();
        }
        m_forbidenMovesFIFO.push_back(std::move(move));
    }

private:
    /**
     * @brief is tabu does not depend on Solution here
     *
     * @param move
     *
     * @return
     */
    bool isTabu(const Move & move) const {
        return m_forbidenMovesSet.find(move) != m_forbidenMovesSet.end();
    }

    unsigned m_size;
    std::unordered_set<Move, boost::hash<Move>> m_forbidenMovesSet;
    std::deque<Move> m_forbidenMovesFIFO;
};

/**
 * @brief This Tabu list remember both current solution and move
 *        It is implemented as TabuListRememberMove<pair<Solution, Move>> with nullptr passed as dummy solution
 *
 * @tparam Solution
 * @tparam Move
 */
template <typename Solution, typename Move>
class TabuListRememberSolutionAndMove : TabuListRememberMove<std::pair<Solution, Move>> {
    typedef TabuListRememberMove<std::pair<Solution, Move>> base;
public:
    /**
     * @brief constructor
     *
     * @param size
     */
    TabuListRememberSolutionAndMove(unsigned size) : base(size) {}

    /**
     * @brief isTabu redirects work to base class
     *
     * @param s
     * @param move
     *
     * @return
     */
    bool isTabu(Solution s, Move move) const {
        return base::isTabu(nullptr, std::make_pair(std::move(s), std::move(move)));
    }

    /**
     * @brief accept redirects work to base class
     *
     * @param s
     * @param move
     */
    void accept(Solution & s, const Move & move) {
        base::accept(nullptr, std::make_pair(std::move(s), std::move(move)));
    }
};

} //!data_structures
} //!paal


#endif /* TABU_LIST_HPP */
