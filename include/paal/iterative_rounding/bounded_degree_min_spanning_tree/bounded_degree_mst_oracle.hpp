/**
 * @file bounded_degree_mst_oracle.hpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-05
 */
#ifndef BOUNDED_DEGREE_MST_ORACLE_HPP
#define BOUNDED_DEGREE_MST_ORACLE_HPP


namespace paal {
namespace ir {

class BoundedDegreeMSTOracle {
public:
    BoundedDegreeMSTOracle()
    { ; }
                           
    template <typename LP>
    bool feasibleSolution(const LP & lp) {
        return true;
    };
    
    template <typename LP>
    void addViolatedConstraint(LP & lp) {
        
    };

private:
    
};

} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_ORACLE_HPP */
