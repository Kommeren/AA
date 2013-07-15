/**
 * @file 2_local_search_logger.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-15
 */
#ifndef TWO_LOCAL_SEARCH_LOGGER_HPP
#define TWO_LOCAL_SEARCH_LOGGER_HPP 

#include "paal/data_structures/cycle/cycle_algo.hpp"
#include "utils/logger.hpp"

namespace paal {
namespace utils {

template <typename Matrix>
class TwoLSLogger {
public:
    TwoLSLogger(const Matrix & mtx, unsigned int actionsPerLog = 1) : 
        m_mtx(mtx), m_actionPerLog(actionsPerLog), m_cnt(0) {}
    
    TwoLSLogger(TwoLSLogger &&) = default; 
    TwoLSLogger(const TwoLSLogger &) = default; 

    template <typename Cycle>
    void operator()(const Cycle & cycle) {
        if(m_cnt++ % m_actionPerLog == 0) {
            LOG("Length after\t" << m_cnt << ": " << simple_algo::getLength(m_mtx, cycle.get()));
        }
    }
private:
    const Matrix & m_mtx;
    const unsigned int m_actionPerLog;
    unsigned m_cnt;
};

template <typename Matrix>
TwoLSLogger<Matrix> make_twoLSLogger(Matrix & mtx, int actionsPerLog = 1) {
    return TwoLSLogger<Matrix>(mtx, actionsPerLog);
}

} //utils
} //paal
#endif /* TWO_LOCAL_SEARCH_LOGGER_HPP */
