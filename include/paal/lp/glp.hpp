/**
 * @file glp.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-05-06
 */
#ifndef LPBASE_HPP
#define LPBASE_HPP
#include <numeric>
#include <glpk.h>

#include <boost/iterator/zip_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "paal/data_structures/bimap.hpp"
#include "paal/lp/ids.hpp"
#include "paal/lp/bound_type.hpp"
#include "paal/lp/problem_type.hpp"

namespace paal {
namespace lp {


/**
 * @class GLPBase
 * @brief This class contains member functions needed to initialize the GLPK LP solver.
 */
class GLPBase {
public:
    /**
     * Constructor.
     *
     * @param numberOfRows initial number of rows in the LP instance
     * @param numberOfColumns initial number of columns in the LP instance
     * @param numberOfNonZerosInMatrix number of non-zero values in the initial LP instance matrix
     */
    GLPBase(int numberOfRows, int numberOfColumns, int numberOfNonZerosInMatrix) :
            m_lp(glp_create_prob()), m_totalColNr(0), m_totalRowNr(0) {
        glp_term_out(GLP_OFF);
        initVec(m_row, numberOfNonZerosInMatrix);
        initVec(m_col, numberOfNonZerosInMatrix);
        initVec(m_val, numberOfNonZerosInMatrix);
        initVec(m_newRowCol);
        initVec(m_newRowVal);
        glp_init_smcp(&m_glpkControl);
        m_glpkControl.msg_lev = GLP_MSG_OFF;
    }

    /**
     * Constructor of an LP instance with no zero values in the problem matrix.
     *
     * @param numberOfRows initial number of rows in the LP instance
     * @param numberOfColumns initial number of columns in the LP instance
     */
    GLPBase(int numberOfRows = 0, int numberOfColumns = 0) : GLPBase(numberOfRows, numberOfColumns, numberOfRows * numberOfColumns)  {}

    /**
     * Destructor.
     */
    ~GLPBase() {
        glp_delete_prob(m_lp);
    }

    /**
     * Frees GLPK resources, common for all LP instances.
     * Should be called after all LP instances are destructed.
     */
    static void freeEnv() {
        glp_free_env();
    }

    /**
     * Sets the name of the LP instance.
     */
    void setLPName(const std::string & s){
        glp_set_prob_name(m_lp, s.c_str());
    }

    /**
     * Sets the problem to be an minimization problem.
     */
    void setMinObjFun() {
        glp_set_obj_dir(m_lp, GLP_MIN);
    }

    /**
     * Sets the problem to be an maximization problem.
     */
    void setMaxObjFun() {
        glp_set_obj_dir(m_lp, GLP_MAX);
    }

    /**
     * Solves the LP using the primal simplex method.
     */
    ProblemType solveToExtremePointPrimal() {
        m_glpkControl.meth = GLP_PRIMAL;
        return runSimplex();
    }

    /**
     * Solves the LP using the dual simplex method.
     */
    ProblemType solveToExtremePointDual() {
        m_glpkControl.meth = GLP_DUAL;
        return runSimplex();
    }

    /**
     * Resolves the LP (starting from the previously found solution)
     * using the primal simplex method.
     */
    ProblemType resolveToExtremePointPrimal() {
        m_glpkControl.meth = GLP_PRIMAL;
        return runSimplex(true);
    }

    /**
     * Resolves the LP (starting from the previously found solution)
     * using the dual simplex method.
     */
    ProblemType resolveToExtremePointDual() {
        m_glpkControl.meth = GLP_DUAL;
        return runSimplex(true);
    }

    /**
     * Returns the found objective function value.
     */
    double getObjValue() const {
        return glp_get_obj_val(m_lp);
    }


    /**
     * Adds a new column to the LP.
     *
     * @param costCoef coefficient of the column in the objective function
     * @param b column bound type
     * @param lb column lower bound value
     * @param ub column upper bound value
     * @param name column symbolic name
     *
     * @return column identifier
     */
    ColId addColumn(double costCoef = 0, BoundType b = LO, double lb = 0, double ub = 0, const std::string & name = "") {
        int colNr = glp_add_cols(m_lp, 1);
        if(name != "") {
            glp_set_col_name(m_lp, colNr, name.c_str());
        }
        glp_set_col_bnds(m_lp, colNr, boundTypeToGLP(b), lb, ub);
        glp_set_obj_coef(m_lp, colNr, costCoef);
        ++m_totalColNr;
        m_colIdx.add(m_totalColNr - 1);
        return ColId(m_totalColNr - 1);
    }

    /**
     * Adds a new row to the LP.
     *
     * @param b row bound type
     * @param lb row lower bound value
     * @param ub row upper bound value
     * @param name row symbolic name
     *
     * @return row identifier
     */
    RowId addRow(BoundType b=UP, double lb=0, double ub=0, const std::string & name = "") {
        int rowNr = glp_add_rows(m_lp, 1);
        if(name != "") {
            glp_set_row_name(m_lp, rowNr, name.c_str());
        }
        glp_set_row_bnds(m_lp, rowNr, boundTypeToGLP(b), lb, ub);
        ++m_totalRowNr;
        m_rowIdx.add(m_totalRowNr - 1);
        return RowId(m_totalRowNr - 1);
    }

    /**
     * Adds a new coefficient of the LP constraint matrix.
     *
     * The coefficient for a given (row, column) pair should not be added more then once.
     * All non-zero coefficients have to be added before calling the loadMatrix() method.
     *
     * @param row row identifier
     * @param col column identifier
     * @param coef coefficient value
     */
    void addConstraintCoef(RowId row, ColId col, double coef = 1) {
        m_row.push_back(getRow(row));
        m_col.push_back(getCol(col));
        m_val.push_back(coef);
    }

    /**
     * Loads the LP constraint matrix.
     *
     * This method must be called after adding a certain initial amount of rows and columns
     * using addRow() and addColumn() methods and setting the non-zero constraint matrix coefficients
     * using addConstraintCoef() method.
     */
    void loadMatrix() {
        glp_load_matrix(m_lp, m_row.size() - 1, &m_row[0], &m_col[0], &m_val[0]);
    }


    /**
     * Adds a new coefficient of the LP constraint matrix for a row added
     * to the LP after loading the initial matrix using loadMatrix() method.
     *
     * The coefficient for a given (row, column) pair should not be added more then once.
     * All non-zero coefficients have to be added before calling the loadNewRow() method.
     *
     * @param col column identifier
     * @param coef coefficient value
     */
    void addNewRowCoef(ColId col, double coef = 1) {
        m_newRowCol.push_back(getCol(col));
        m_newRowVal.push_back(coef);
    }

    /**
     * Loads a new LP row coefficients.
     *
     * This method must be called after adding a new row to the LP (after the initial loadMatrix() call)
     * using addRow() methods and setting the non-zero row coefficients
     * using addNewRowCoef() method.
     */
    void loadNewRow() {
        int rowNr = glp_get_num_rows(m_lp);
        glp_set_mat_row(m_lp, rowNr, m_newRowCol.size() - 1, &m_newRowCol[0], &m_newRowVal[0]);
        m_newRowCol.clear();
        m_newRowVal.clear();
        initVec(m_newRowCol);
        initVec(m_newRowVal);
    }

    /**
     * Sets the bounds of an existing LP row.
     *
     * @param row row identifier
     * @param b row bound type
     * @param lb row lower bound value
     * @param ub row upper bound value
     */
    void setRowBounds(RowId row, BoundType b, double lb, double ub) {
        glp_set_row_bnds(m_lp, getRow(row), boundTypeToGLP(b), lb, ub);
    }

    /**
     * Sets the bounds of an existing LP column.
     *
     * @param col column identifier
     * @param b row bound type
     * @param lb row lower bound value
     * @param ub row upper bound value
     */
    void setColBounds(ColId col, BoundType b, double lb, double ub) {
        glp_set_col_bnds(m_lp, getCol(col), boundTypeToGLP(b), lb, ub);
    }

protected:

    /**
     * Optimizes the LP using the simplex method.
     *
     * @return solution status
     */
    ProblemType runSimplex(bool resolve = false) {
        if (!resolve) {
            glp_adv_basis(m_lp, 0);
        }
        int ret = glp_simplex(m_lp, &m_glpkControl);
        if (resolve && ret != 0) {
            // if basis is not valid, create basis and try again
            glp_adv_basis(m_lp, 0);
            ret = glp_simplex(m_lp, &m_glpkControl);
        }
        assert(ret == 0);
        return getPrimalType();
    }

    /**
     * Converts the GLPK soltion status into paal::lp::ProblemType.
     *
     * @return solution status
     */
    ProblemType getPrimalType() {
        if (glp_get_status(m_lp) == GLP_OPT) {
            return OPTIMAL;
        }

        switch (glp_get_prim_stat(m_lp)) {
            case GLP_UNDEF:
                return UNDEFINED;
            case GLP_NOFEAS:
                return INFEASIBLE;
            case GLP_FEAS:
            case GLP_INFEAS:
                if (glp_get_dual_stat(m_lp) == GLP_NOFEAS) {
                    return UNBOUNDED;
                }
                else {
                    return UNDEFINED;
                }
            default:
                assert(false);
                return UNDEFINED;
        }
    }

    /**
     * Converts column identifier to the GLPK column index.
     */
    int getCol(ColId col) const {
        return m_colIdx.getIdx(col.get()) + 1;
    }

    /**
     * Converts row identifier to the GLPK row index.
     */
    int getRow(RowId row) const {
        return m_rowIdx.getIdx(row.get()) + 1;
    }

    /**
     * Converts GLPK column index to the column identifier.
     */
    ColId getColIdx(int col) const {
        return ColId(m_colIdx.getVal(col - 1));
    }

    /**
     * Converts GLPK row index to the row identifier.
     */
    RowId getRowIdx(int row) const {
        return RowId(m_rowIdx.getVal(row - 1));
    }

    typedef std::vector<int> Ids;
    typedef std::vector<double> Vals;

    /**
     * Initializes a given vector.
     */
    template <typename Vec>
    static void initVec(Vec & v, int numberOfNonZerosInMatrix = 0) {
        v.reserve(++numberOfNonZerosInMatrix);
        v.push_back(0);
    }

    /**
     * Converts paal::lp:BoundType to the GLPK bound type.
     */
    static int boundTypeToGLP(BoundType b) {
        switch(b) {
            case FR:
                return GLP_FR; //free
            case LO:
                return GLP_LO; //lower bound
            case UP:
                return GLP_UP; //upper bound
            case DB:
                return GLP_DB; //lower and upper bound
            case FX:
                return GLP_FX; //exact value
            default:
                assert(false);
                return GLP_FR;
        }
    }

    /**
     * Converts the GLPK bound type to paal::lp:BoundType.
     */
    static BoundType glpToBoundType(int b) {
        switch(b) {
            case GLP_FR:
                return FR;
            case GLP_LO:
                return LO;
            case GLP_UP:
                return UP;
            case GLP_DB:
                return DB;
            case GLP_FX:
                return FX;
            default:
                assert(false);
                return FR;
        }
    }

    /// GLPK problem object
    glp_prob * m_lp;
    /// mapping between GLPK column numbers and column IDs
    data_structures::EraseableBiMap<int> m_colIdx;
    /// mapping between GLPK row numbers and column IDs
    data_structures::EraseableBiMap<int> m_rowIdx;

private:

    GLPBase(GLPBase &&) {}
    GLPBase(const GLPBase &) {}

    Ids m_row;
    Ids m_col;
    Vals m_val;
    Ids m_newRowCol;
    Vals m_newRowVal;
    glp_smcp m_glpkControl;
    int m_totalColNr;
    int m_totalRowNr;
};



/**
 * @class GLP
 * @brief The class used for solving LP problems using the GLPK solver.
 *
 * Usage example:
 * @snippet glp_example.cpp GLP Example
 * Complete example can be found in glp_example.cpp
 */
class GLP : public GLPBase {
    typedef decltype(std::bind(&GLP::getRowIdx, std::declval<const GLP *>(), std::placeholders::_1)) RowTrans;
    typedef decltype(std::bind(&GLP::getColIdx, std::declval<const GLP *>(), std::placeholders::_1)) ColTrans;
    typedef typename boost::transform_iterator<RowTrans, typename Ids::iterator, RowId> TransformRow;
    typedef typename boost::transform_iterator<ColTrans, typename Ids::iterator, ColId> TransformCol;
    typedef boost::zip_iterator<boost::tuple<TransformRow,
                                             typename Vals::iterator>> RowsInColumnIterator;
    typedef boost::zip_iterator<boost::tuple<TransformCol,
                                             typename Vals::iterator>> ColsInRowIterator;
public:
    /**
     * Constructor.
     *
     * @param numberOfRows initial number of rows in the LP instance
     * @param numberOfColumns initial number of columns in the LP instance
     * @param numberOfNonZerosInMatrix number of non-zero values in the initial LP instance matrix
     */
    GLP(int numberOfRows, int numberOfColumns, int numberOfNonZerosInMatrix) :
            GLPBase(numberOfRows, numberOfColumns, numberOfNonZerosInMatrix) {
        int maxRowCol = std::max(numberOfRows, numberOfColumns);
        initVec(m_idxTmp, maxRowCol);
        initVec(m_valTmp, maxRowCol);
    }

    /**
     * Constructor of an LP instance with no zero values in the problem matrix.
     *
     * @param numberOfRows initial number of rows in the LP instance
     * @param numberOfColumns initial number of columns in the LP instance
     */
    GLP(int numberOfRows = 0, int numberOfColumns = 0) :
        GLP(numberOfRows, numberOfColumns, numberOfRows * numberOfColumns)  {}

    /**
     * Output stream operator for printing debug information.
     */
    template <typename ostream>
    friend ostream & operator<<(ostream & o, const  GLP & glp) {
        o << "Problem name: " << glp_get_prob_name(glp.m_lp) << std::endl << "Obj function" << std::endl;

        for(ColId col : boost::make_iterator_range(glp.getColumns())) {
            o << glp_get_obj_coef(glp.m_lp, glp.getCol(col)) << ", ";
        }
        o << std::endl << "Rows" << std::endl;

        for(RowId row : boost::make_iterator_range(glp.getRows())) {
            auto cols = glp.getColumnsInRow(row);
            if(cols.first == cols.second) {
                continue;
            }
            o << "Row " << glp.getRowName(row) << std::endl;
            o << "Bounds " << "type =  " << glp.getRowBoundType(row) << " lb = " << glp.getRowLb(row) << " ub = " << glp.getRowUb(row) << std::endl;
            for(auto colAndVal : boost::make_iterator_range(cols)) {
                ColId  col = boost::get<0>(colAndVal);
                double val = boost::get<1>(colAndVal);
                o << "(col = " << col.get() << " name = " << glp.getColName(col) << ", coef = " << val << ") - ";
            }
            o << std::endl;
        }
        o << "Current solution: "<<std::endl;
        for(ColId col : boost::make_iterator_range(glp.getColumns())) {
            o  << glp.getColPrim(col) << ", ";
        }
        o << std::endl;

        return o;
    }

    /**
     * Adds a new column to the LP.
     *
     * @param costCoef coefficient of the column in the objective function
     * @param b column bound type
     * @param lb column lower bound value
     * @param ub column upper bound value
     * @param name column symbolic name
     *
     * @return column identifier
     */
    ColId addColumn(double costCoef = 0, BoundType b = LO, double lb = 0, double ub = 0, const std::string & name = "") {
        resizeTmp();
        return GLPBase::addColumn(costCoef, b, lb, ub, name);
    }

    /**
     * Adds a new row to the LP.
     *
     * @param b row bound type
     * @param lb row lower bound value
     * @param ub row upper bound value
     * @param name row symbolic name
     *
     * @return row identifier
     */
    RowId addRow(BoundType b = UP, double lb = 0, double ub = 0, const std::string & name = "") {
        resizeTmp();
        return GLPBase::addRow(b, lb, ub, name);
    }

    /**
     * Returns column primal value.
     */
    double getColPrim(ColId col) const {
        return glp_get_col_prim(m_lp, getCol(col));
    }

    /**
     * Returns the number of columns in the LP.
     */
    int colSize() const {
        return glp_get_num_cols(m_lp);
    }

    /**
     * Returns the number of rows in the LP.
     */
    int rowSize() const {
        return glp_get_num_rows(m_lp);
    }

    /**
     * Removes a row form the LP.
     *
     * @param row identifier of the row to be removed
     */
    void deleteRow(RowId row) {
        int arr[2];
        arr[1] = getRow(row);
        m_rowIdx.erase(row.get());
        glp_del_rows(m_lp, 1, arr);
    }

    /**
     * Removes a column form the LP.
     *
     * @param col identifier of the column to be removed
     */
    void deleteCol(ColId col) {
        int arr[2];
        arr[1] = getCol(col);
        m_colIdx.erase(col.get());
        glp_del_cols(m_lp, 1, arr);
    }

    //TODO rewrite
    /**
     * Clears the LP instance.
     */
    void clear() {
        while (true) {
            auto rows = getRows();
            if (rows.first == rows.second) break;
            RowId row = *rows.first;
            deleteRow(row);
        }
        while (true) {
            auto cols = getColumns();
            if (cols.first == cols.second) break;
            ColId col = *cols.first;
            deleteCol(col);
        }
    }

    /**
     * Returns the number of non-zero coefficients in the given LP matrix column.
     */
    int getColDegree(ColId col) const {
        return glp_get_mat_col(m_lp, getCol(col), &m_idxTmp[0], &m_valTmp[0]);
    }

    /**
     * Returns the number of non-zero coefficients in the given LP matrix row.
     */
    int getRowDegree(RowId row) const {
        return glp_get_mat_row(m_lp, getRow(row), &m_idxTmp[0], &m_valTmp[0]);
    }

    /**
     * Returns the sum of the values of those columns, which have got non-zero coefficients
     * in the given LP row.
     */
    double getRowSum(RowId row) const {
        int size = glp_get_mat_row(m_lp, getRow(row), &m_idxTmp[0], NULL);
        return getSolSumForIds(m_idxTmp.begin() + 1, m_idxTmp.begin() + size + 1);
    }

    /**
     * Returns the column symbolic name.
     */
    std::string getColName(ColId col) const {
        const char * name = glp_get_col_name(m_lp, getCol(col));
        if(name == NULL) {
            return "";
        }
        return name;
    }

    /**
     * Returns the column upper bound.
     */
    double getColUb(ColId col) const {
        return glp_get_col_ub(m_lp, getCol(col));
    }

    /**
     * Returns the column lower bound.
     */
    double getColLb(ColId col) const {
        return glp_get_col_lb(m_lp, getCol(col));
    }

    /**
     * Returns the row symbolic name.
     */
    std::string getRowName(RowId row) const {
        const char * name = glp_get_row_name(m_lp, getRow(row));
        if(name == NULL) {
            return "";
        }
        return name;
    }

    /**
     * Returns the row upper bound.
     */
    double getRowUb(RowId row) const {
        return glp_get_row_ub(m_lp, getRow(row));
    }

    /**
     * Returns the row lower bound.
     */
    double getRowLb(RowId row) const {
        return glp_get_row_lb(m_lp, getRow(row));
    }

    /**
     * Returns the row bound type.
     */
    BoundType getRowBoundType(RowId row) const {
        return glpToBoundType(glp_get_row_type(m_lp, getRow(row)));
    }

    /**
     * Returns the column bound type.
     */
    BoundType getColBoundType(ColId col) const {
        return glpToBoundType(glp_get_col_type(m_lp, getCol(col)));
    }


    /**
     * Returns the idetifiers and coefficients of all rows in a given column,
     * which constraint matrix coefficient is non-zero (as an iterator range).
     */
    std::pair<RowsInColumnIterator, RowsInColumnIterator>
            getRowsInColumn(ColId col) const {
        int size = glp_get_mat_col(m_lp, getCol(col), &m_idxTmp[0], &m_valTmp[0]);
        return std::make_pair(
                boost::make_zip_iterator(boost::make_tuple(TransformRow(m_idxTmp.begin() + 1, std::bind(&GLP::getRowIdx, this, std::placeholders::_1)), m_valTmp.begin() + 1)),
                boost::make_zip_iterator(boost::make_tuple(TransformRow(m_idxTmp.begin() + size + 1, std::bind(&GLP::getRowIdx, this, std::placeholders::_1)), m_valTmp.begin() + 1 + size)));
    }

    /**
     * Returns the idetifiers and coefficients of all columns in a given row,
     * which constraint matrix coefficient is non-zero (as an iterator range).
     */
    std::pair<ColsInRowIterator, ColsInRowIterator>
            getColumnsInRow(RowId row) const {
        int size = glp_get_mat_row(m_lp, getRow(row), &m_idxTmp[0], &m_valTmp[0]);
        return std::make_pair(
                boost::make_zip_iterator(boost::make_tuple(TransformCol(m_idxTmp.begin() + 1, std::bind(&GLP::getColIdx, this, std::placeholders::_1)), m_valTmp.begin() + 1)),
                boost::make_zip_iterator(boost::make_tuple(TransformCol(m_idxTmp.begin() + size + 1, std::bind(&GLP::getColIdx, this, std::placeholders::_1)), m_valTmp.begin() + 1 + size)));
    }

    typedef boost::transform_iterator<ColTrans, decltype(boost::begin(boost::irange(0,0))), ColId> ColIter;
    /**
     * Returns all column identifiers (as an iterator range).
     */
    std::pair<ColIter, ColIter> getColumns() const {
        auto range = boost::irange(1, glp_get_num_cols(m_lp) + 1);
        return std::make_pair(ColIter(boost::begin(range), std::bind(&GLP::getColIdx, this, std::placeholders::_1)),
                              ColIter(boost::end(range)  , std::bind(&GLP::getColIdx, this, std::placeholders::_1)));
    }

    typedef boost::transform_iterator<RowTrans, decltype(boost::begin(boost::irange(0,0))), RowId> RowIter;
    /**
     * Returns all row identifiers (as an iterator range).
     */
    std::pair<RowIter, RowIter> getRows() const {
        auto range = boost::irange(1, glp_get_num_rows(m_lp) + 1);
        return std::make_pair(RowIter(boost::begin(range), std::bind(&GLP::getRowIdx, this, std::placeholders::_1)),
                              RowIter(boost::end(range)  , std::bind(&GLP::getRowIdx, this, std::placeholders::_1)));
    }

private:

    template <typename Iter>
    double getSolSumForIds(Iter begin, Iter end) const  {
        return std::accumulate(begin, end, 0., [=](double sum, int u){
            return sum + glp_get_col_prim(m_lp, u);
        });
    }

    void resizeTmp() {
        m_idxTmp.push_back(0);
        m_valTmp.push_back(0);
    }

    mutable Ids m_idxTmp;
    mutable Vals m_valTmp;
};

} //lp
} //paal

#endif /* LPBASE_HPP */
