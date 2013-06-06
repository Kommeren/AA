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
    RowGenerationSolveLP() : m_oracle(0) {}
  
    template <typename LP>
    double operator()(LP & lp) {
        double res;
        res = lp.solve();
        while (!m_oracle->feasibleSolution(lp)) {
            m_oracle->addViolatedConstraint(lp);
            res = lp.solve();
        }
        return res;
    }
    
    void setOracle(Oracle * oracle) {
        m_oracle = oracle;
    }
    
private:
    Oracle * m_oracle;
};

} //paal
} //ir

#endif /* LP_ROW_GENERATION_HPP */
