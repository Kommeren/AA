/**
 * @file lp_row_generation.hpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-04
 */
#ifndef LP_ROW_GENERATION_HPP
#define LP_ROW_GENERATION_HPP 

#include "paal/lp/problem_type.hpp"

namespace paal {
namespace lp {

template <typename Oracle>
class RowGenerationSolveLP {
public:
    RowGenerationSolveLP(Oracle & oracle) : m_oracle(oracle) {}
  
    template <typename Problem, typename LP>
    ProblemType operator()(const Problem & problem, LP & lp) {
        auto probType = lp.solveToExtremePoint();
        while (probType == OPTIMAL && !m_oracle.feasibleSolution(problem, lp)) {
            m_oracle.addViolatedConstraint(problem, lp);
            probType = lp.resolveToExtremePoint();
        }
        return probType;
    }
    
private:
    Oracle & m_oracle;
};


template <typename Oracle>
RowGenerationSolveLP<Oracle>
make_RowGenerationSolveLP(Oracle & o) {
    return RowGenerationSolveLP<Oracle>(o);
}

} //lp
} //paal

#endif /* LP_ROW_GENERATION_HPP */
