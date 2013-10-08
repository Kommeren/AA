/**
 * @file lp.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */


#include <vector>
#include <boost/type_traits/is_convertible.hpp>

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/generalised_assignment/generalised_assignment.hpp"

using namespace  paal;
using namespace  paal::ir;

int fun() {
}

int main() {
    GLPBase base;
    ColId x12 = base.addColumn(0, DB, 0, 1 );
    ColId x13 = base.addColumn(2, DB, 0, 1 );
    ColId x14 = base.addColumn(2, DB, 0, 1 );
    ColId x23 = base.addColumn(1, DB, 0, 1 );
    ColId x24 = base.addColumn(0, DB, 0, 1 );
    ColId x34 = base.addColumn(1, DB, 0, 1 );
    
    ColId x1 = base.addColumn(0, DB, 0, 1 );
    ColId x2 = base.addColumn(0, DB, 0, 1 );
    ColId x3 = base.addColumn(1, DB, 0, 1 );
    ColId x4 = base.addColumn(1, DB, 0, 1 );
    
    ColId p12 = base.addColumn(0, DB, 0, 1 );
    ColId p13 = base.addColumn(2, DB, 0, 1 );
    ColId p14 = base.addColumn(2, DB, 0, 1 );
    ColId p23 = base.addColumn(1, DB, 0, 1 );
    ColId p24 = base.addColumn(0, DB, 0, 1 );
    ColId p34 = base.addColumn(1, DB, 0, 1 );

    fun();

//    addRow();
//
    static_assert(boost::is_convertible<double, int>::value, "bu!");

    return 0;
}

