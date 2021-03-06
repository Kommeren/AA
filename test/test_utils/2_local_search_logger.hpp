//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file 2_local_search_logger.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-15
 */
#ifndef PAAL_2_LOCAL_SEARCH_LOGGER_HPP
#define PAAL_2_LOCAL_SEARCH_LOGGER_HPP

#include "test_utils/logger.hpp"

#include "paal/data_structures/cycle/cycle_algo.hpp"

namespace paal {
namespace utils {

template <typename Matrix> class two_ls_logger {
  public:
    two_ls_logger(const Matrix &mtx, unsigned int actionsPerLog = 1)
        : m_mtx(mtx), m_action_per_log(actionsPerLog), m_cnt(0) {}

    two_ls_logger(two_ls_logger &&) = default;
    two_ls_logger(const two_ls_logger &) = default;

    template <typename Cycle> bool operator()(const Cycle &cycle) {
        if (m_cnt++ % m_action_per_log == 0) {
            LOGLN("Length after\t"
                  << m_cnt << ": "
                  << get_cycle_length(m_mtx, cycle.get_cycle()));
        }
        return true;
    }

  private:
    const Matrix &m_mtx;
    const unsigned int m_action_per_log;
    unsigned m_cnt;
};

template <typename Matrix>
two_ls_logger<Matrix> make_two_ls_logger(Matrix &mtx, int actionsPerLog = 1) {
    return two_ls_logger<Matrix>(mtx, actionsPerLog);
}

} //! utils
} //! paal
#endif // PAAL_2_LOCAL_SEARCH_LOGGER_HPP
