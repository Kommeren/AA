/**
 * @file glp_example.cpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-12-05
 */

#include "paal/lp/glp.hpp"

int main() {
//! [GLP Example]
    {
        // sample problem
        paal::lp::GLP lpInstance;

        lpInstance.set_max_obj_fun();
        auto colX = lpInstance.add_column(500);
        auto colY = lpInstance.add_column(300);
        auto row1 = lpInstance.add_row(paal::lp::DB, 7, 10);
        auto row2 = lpInstance.add_row(paal::lp::UP, 0, 1200);
        lpInstance.add_constraint_coef(row1, colX);
        lpInstance.add_constraint_coef(row1, colY);
        lpInstance.add_constraint_coef(row2, colX, 200);
        lpInstance.add_constraint_coef(row2, colY, 100);
        lpInstance.load_matrix();

        // solve it
        auto status = lpInstance.solve_to_extreme_point_primal();

        if (status == paal::lp::OPTIMAL) {
            std::cout << "Optimal solution cost: " << lpInstance.get_obj_value() << std::endl;
            std::cout << "X = " << lpInstance.get_col_prim(colX) << std::endl;
            std::cout << "Y = " << lpInstance.get_col_prim(colY) << std::endl;
        }
        else {
            std::cout << "Optimal solution not found" << std::endl;
        }

        // add new row
        lpInstance.add_row(paal::lp::UP, 0, 12);
        lpInstance.add_new_row_coef(colX);
        lpInstance.add_new_row_coef(colY, 2);
        lpInstance.load_new_row();

        // resolve it
        status = lpInstance.resolve_to_extreme_point_dual();

        if (status == paal::lp::OPTIMAL) {
            std::cout << "Optimal solution cost: " << lpInstance.get_obj_value() << std::endl;
            std::cout << "X = " << lpInstance.get_col_prim(colX) << std::endl;
            std::cout << "Y = " << lpInstance.get_col_prim(colY) << std::endl;
        }
        else {
            std::cout << "Optimal solution not found" << std::endl;
        }
    }
    paal::lp::GLP::free_env();
//! [GLP Example]

    return 0;
}

