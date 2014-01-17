/**
 * @file n_queens_solution.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-01-02
 */
#ifndef N_QUEENS_SOLUTION_HPP
#define N_QUEENS_SOLUTION_HPP 

#include <vector>

#include <boost/iterator/counting_iterator.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/numeric.hpp>

namespace paal {
namespace local_search {

    /**
     * @brief Adapts vector representing n queen problem to class
     * able to efficiently compute gain of given move
     *
     * @tparam NQueensPositionsVector
     */
    template <typename NQueensPositionsVector>
    struct NQueensSolutionAdapter {
        typedef typename boost::counting_iterator<int> QueensIterator;

        /**
         * @brief constructor
         *
         * @param pos
         */
        NQueensSolutionAdapter(NQueensPositionsVector & pos) :
            m_queenPosition(pos),  
            m_numeberAttacingDiagonalNegative(boost::distance(pos), 0),  
            m_numeberAttacingDiagonalNonnegative(boost::distance(pos), 0),
            m_numeberAttacingCounterDiagonal(2 * boost::distance(pos), 0) 
        {
            for(auto q : boost::irange(0, int(boost::distance(pos)))) {
                increase(q);
            }
        }

        /**
         * @brief begin of the queens positions' collection
         *
         * @return 
         */
        QueensIterator begin() const {
            return QueensIterator(0);
        }

        /**
         * @brief end of the queens positions' collection
         *
         * @return 
         */
        QueensIterator end() const {
            return QueensIterator(m_queenPosition.size());
        }

        /**
         * @brief swaps two queens positions
         *
         * @param xLeft
         * @param xRight
         */
        void swapQueens(int xLeft, int xRight) {
            int leftPosition = m_queenPosition[xLeft];
            putQueen(xLeft, m_queenPosition[xRight]);
            putQueen(xRight, leftPosition);
        }
            
        /**
         * @brief get number of queens attacing (x,y) position
         *
         * @param x
         * @param y
         *
         * @return 
         */
        int getNumAttacing(int x, int y) const {
            return m_numeberAttacingCounterDiagonal[x + y] + getDiagonal(x, y);
        }

        /**
         * @brief return y for xth queen 
         *
         * @param x
         *
         * @return 
         */
        int getY(int x) const {
            return m_queenPosition[x];
        }

        /**
         * @brief computes total number of conflicts on the board
         *
         * @return 
         */
        int objFun() const {
            auto attacingNr = [](int sum, int n) {
                return sum + n * (n-1) / 2;
            };
            int sum = boost::accumulate(m_numeberAttacingCounterDiagonal, 0, attacingNr);
                sum = boost::accumulate(m_numeberAttacingDiagonalNegative, sum, attacingNr);
            return    boost::accumulate(m_numeberAttacingDiagonalNonnegative, sum, attacingNr);
        }

    private:
        
        /**
         * @brief puts xth queen on position y
         *
         * @param x
         * @param y
         */
        void putQueen(int x, int y) {
            decrease(x);
            m_queenPosition[x] = y;
            increase(x);
        }


        /**
         * @brief get diagonal counter for diagonal of the xth queen
         *
         * @param x
         *
         * @return 
         */
        int & getDiagonal(int x) {
            return getDiagonal(x, m_queenPosition[x]);
        }

        /**
         * @brief gets diagonal crossing (x,y) position
         *
         * @param x
         * @param y
         *
         * @return 
         */
        int & getDiagonal(int x, int y) {
            if(x >= y) {
                return m_numeberAttacingDiagonalNegative[x - y];
            } else {
                return m_numeberAttacingDiagonalNonnegative[y - x];
            }
        }
        
        /**
         * @brief const version of getDiagonal(x,y)
         *
         * @param x
         * @param y
         *
         * @return 
         */
        int getDiagonal(int x, int y) const {
            if(x >= y) {
                return m_numeberAttacingDiagonalNegative[x - y];
            } else {
                return m_numeberAttacingDiagonalNonnegative[y - x];
            }
        }

        /**
         * @brief decrease diagonals counters fo xth queen
         *
         * @param x
         */
        void decrease(int x) {
            --m_numeberAttacingCounterDiagonal[x + m_queenPosition[x]];
            --getDiagonal(x);
        }
        
        /**
         * @brief decrese diagonal counter for the xth queen
         *
         * @param x
         */
        void increase(int x) {
            ++m_numeberAttacingCounterDiagonal[x + m_queenPosition[x]];
            ++getDiagonal(x);
        }

        NQueensPositionsVector & m_queenPosition;
        std::vector<int> m_numeberAttacingDiagonalNegative;
        std::vector<int> m_numeberAttacingDiagonalNonnegative;
        std::vector<int> m_numeberAttacingCounterDiagonal;
    };

} //!local_search 
} //!paal

#endif /* N_QUEENS_SOLUTION_HPP */
