/**
 * @file lp_row_generation.hpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-04
 */
#ifndef LP_ROW_GENERATION_HPP
#define LP_ROW_GENERATION_HPP 

namespace paal {
namespace ir {

template <typename Oracle>
class RowGenerationSolveLP {
public:
    RowGenerationSolveLP(Oracle & oracle) : m_oracle(oracle) {}
  
    template <typename Solution, typename LP>
    double operator()(const Solution &, LP & lp) {
        double res;
        res = lp.solveToExtremePoint();
        while (!m_oracle.feasibleSolution(lp)) {
            m_oracle.addViolatedConstraint(lp);
            res = lp.solveToExtremePoint();
        }
        return res;
    }
    
private:
    Oracle & m_oracle;
};


template <typename Oracle>
RowGenerationSolveLP<Oracle>
make_RowGenerationSolveLP(Oracle & o) {
    return RowGenerationSolveLP<Oracle>(o);
}

} //paal
} //ir

#endif /* LP_ROW_GENERATION_HPP */
