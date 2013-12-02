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

        lpInstance.setMaxObjFun();
        auto colX = lpInstance.addColumn(500);
        auto colY = lpInstance.addColumn(300);
        auto row1 = lpInstance.addRow(paal::lp::DB, 7, 10);
        auto row2 = lpInstance.addRow(paal::lp::UP, 0, 1200);
        lpInstance.addConstraintCoef(row1, colX);
        lpInstance.addConstraintCoef(row1, colY);
        lpInstance.addConstraintCoef(row2, colX, 200);
        lpInstance.addConstraintCoef(row2, colY, 100);
        lpInstance.loadMatrix();

        // solve it
        auto status = lpInstance.solveToExtremePointPrimal();

        if (status == paal::lp::OPTIMAL) {
            std::cout << "Optimal solution cost: " << lpInstance.getObjValue() << std::endl;
            std::cout << "X = " << lpInstance.getColPrim(colX) << std::endl;
            std::cout << "Y = " << lpInstance.getColPrim(colY) << std::endl;
        }
        else {
            std::cout << "Optimal solution not found" << std::endl;
        }

        // add new row
        lpInstance.addRow(paal::lp::UP, 0, 12);
        lpInstance.addNewRowCoef(colX);
        lpInstance.addNewRowCoef(colY, 2);
        lpInstance.loadNewRow();

        // resolve it
        status = lpInstance.resolveToExtremePointDual();

        if (status == paal::lp::OPTIMAL) {
            std::cout << "Optimal solution cost: " << lpInstance.getObjValue() << std::endl;
            std::cout << "X = " << lpInstance.getColPrim(colX) << std::endl;
            std::cout << "Y = " << lpInstance.getColPrim(colY) << std::endl;
        }
        else {
            std::cout << "Optimal solution not found" << std::endl;
        }
    }
    paal::lp::GLP::freeEnv();
//! [GLP Example]

    return 0;
}

