/**
 * @file log_visitor.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-06-14
 */
#ifndef LOG_VISITOR_HPP
#define LOG_VISITOR_HPP

#include "paal/iterative_rounding/iterative_rounding.hpp"
namespace paal {

struct log_visitor : public ir::trivial_visitor {
    template <typename Problem, typename LP>
    void solve_lp(const Problem &, LP & lp) {
        LOGLN(lp);
    }

    template <typename Problem, typename LP>
    void round_col(const Problem &, LP & lp, lp::col_id col, double val) {
        LOGLN("Column "<< col.get() << " rounded to " << val);
    }

    template <typename Problem, typename LP>
    void relax_row(const Problem &, LP & lp, lp::row_id row) {
        LOGLN("Relax row " << row.get());
    }
};

} //!paal

#endif /* LOG_VISITOR_HPP */
