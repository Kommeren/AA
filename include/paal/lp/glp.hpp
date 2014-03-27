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
#include <unordered_set>
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
 * @class glp_base
 * @brief This class contains member functions needed to initialize the GLPK LP solver.
 */
class glp_base {
public:
    /**
     * Constructor.
     *
     * @param numberOfRows initial number of rows in the LP instance
     * @param numberOfColumns initial number of columns in the LP instance
     * @param numberOfNonZerosInMatrix number of non-zero values in the initial LP instance matrix
     */
    glp_base(int numberOfRows, int numberOfColumns, int numberOfNonZerosInMatrix) :
            m_lp(glp_create_prob()), m_total_col_nr(0), m_total_row_nr(0) {
        glp_term_out(GLP_OFF);
        init_vec(m_row, numberOfNonZerosInMatrix);
        init_vec(m_col, numberOfNonZerosInMatrix);
        init_vec(m_val, numberOfNonZerosInMatrix);
        init_vec(m_new_row_col);
        init_vec(m_new_row_val);
        glp_init_smcp(&m_glpk_control);
        m_glpk_control.msg_lev = GLP_MSG_OFF;
    }

    /**
     * Constructor of an LP instance with no zero values in the problem matrix.
     *
     * @param numberOfRows initial number of rows in the LP instance
     * @param numberOfColumns initial number of columns in the LP instance
     */
    glp_base(int numberOfRows = 0, int numberOfColumns = 0) : glp_base(numberOfRows, numberOfColumns, numberOfRows * numberOfColumns)  {}

    /**
     * Destructor.
     */
    ~glp_base() {
        glp_delete_prob(m_lp);
    }

    /**
     * Frees GLPK resources, common for all LP instances.
     * Should be called after all LP instances are destructed.
     */
    static void free_env() {
        glp_free_env();
    }

    /**
     * Sets the name of the LP instance.
     */
    void set_lp_name(const std::string & s){
        glp_set_prob_name(m_lp, s.c_str());
    }

    /**
     * Sets the problem to be an minimization problem.
     */
    void set_min_obj_fun() {
        glp_set_obj_dir(m_lp, GLP_MIN);
    }

    /**
     * Sets the problem to be an maximization problem.
     */
    void set_max_obj_fun() {
        glp_set_obj_dir(m_lp, GLP_MAX);
    }

    /**
     * Solves the LP using the primal simplex method.
     */
    problem_type solve_to_extreme_point_primal() {
        m_glpk_control.meth = GLP_PRIMAL;
        return run_simplex();
    }

    /**
     * Solves the LP using the dual simplex method.
     */
    problem_type solve_to_extreme_point_dual() {
        m_glpk_control.meth = GLP_DUAL;
        return run_simplex();
    }

    /**
     * Resolves the LP (starting from the previously found solution)
     * using the primal simplex method.
     */
    problem_type resolve_to_extreme_point_primal() {
        m_glpk_control.meth = GLP_PRIMAL;
        return run_simplex(true);
    }

    /**
     * Resolves the LP (starting from the previously found solution)
     * using the dual simplex method.
     */
    problem_type resolve_to_extreme_point_dual() {
        m_glpk_control.meth = GLP_DUAL;
        return run_simplex(true);
    }

    /**
     * Returns the found objective function value.
     * Should be called only after the LP has been solved and if it
     * wasn't modified afterwards.
     */
    double get_obj_value() const {
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
    col_id add_column(double costCoef = 0, bound_type b = LO, double lb = 0, double ub = 0, const std::string & name = "") {
        int colNr = glp_add_cols(m_lp, 1);
        if(name != "") {
            glp_set_col_name(m_lp, colNr, name.c_str());
        }
        glp_set_col_bnds(m_lp, colNr, bound_type_to_glp(b), lb, ub);
        glp_set_obj_coef(m_lp, colNr, costCoef);
        ++m_total_col_nr;
        m_col_idx.add(m_total_col_nr - 1);
        return col_id(m_total_col_nr - 1);
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
    row_id add_row(bound_type b=UP, double lb=0, double ub=0, const std::string & name = "") {
        int rowNr = glp_add_rows(m_lp, 1);
        if(name != "") {
            glp_set_row_name(m_lp, rowNr, name.c_str());
        }
        glp_set_row_bnds(m_lp, rowNr, bound_type_to_glp(b), lb, ub);
        ++m_total_row_nr;
        m_row_idx.add(m_total_row_nr - 1);
        return row_id(m_total_row_nr - 1);
    }

    /**
     * Adds a new coefficient of the LP constraint matrix.
     *
     * The coefficient for a given (row, column) pair should not be added more then once.
     * All non-zero coefficients have to be added before calling the load_matrix() method.
     *
     * @param row row identifier
     * @param col column identifier
     * @param coef coefficient value
     */
    void add_constraint_coef(row_id row, col_id col, double coef = 1) {
        m_row.push_back(get_row(row));
        m_col.push_back(get_col(col));
        m_val.push_back(coef);
    }

    /**
     * Loads the LP constraint matrix.
     *
     * This method must be called after adding a certain initial amount of rows and columns
     * using add_row() and add_column() methods and setting the non-zero constraint matrix coefficients
     * using add_constraint_coef() method.
     */
    void load_matrix() {
        glp_load_matrix(m_lp, m_row.size() - 1, &m_row[0], &m_col[0], &m_val[0]);
    }


    /**
     * Adds a new coefficient of the LP constraint matrix for a row added
     * to the LP after loading the initial matrix using load_matrix() method.
     *
     * The coefficient for a given (row, column) pair should not be added more then once.
     * All non-zero coefficients have to be added before calling the load_new_row() method.
     *
     * @param col column identifier
     * @param coef coefficient value
     */
    void add_new_row_coef(col_id col, double coef = 1) {
        m_new_row_col.push_back(get_col(col));
        m_new_row_val.push_back(coef);
    }

    /**
     * Loads a new LP row coefficients.
     *
     * This method must be called after adding a new row to the LP (after the initial load_matrix() call)
     * using add_row() methods and setting the non-zero row coefficients
     * using add_new_row_coef() method.
     */
    void load_new_row() {
        int rowNr = glp_get_num_rows(m_lp);
        glp_set_mat_row(m_lp, rowNr, m_new_row_col.size() - 1, &m_new_row_col[0], &m_new_row_val[0]);
        m_new_row_col.clear();
        m_new_row_val.clear();
        init_vec(m_new_row_col);
        init_vec(m_new_row_val);
    }

    /**
     * Sets the bounds of an existing LP row.
     *
     * @param row row identifier
     * @param b row bound type
     * @param lb row lower bound value
     * @param ub row upper bound value
     */
    void set_row_bounds(row_id row, bound_type b, double lb, double ub) {
        glp_set_row_bnds(m_lp, get_row(row), bound_type_to_glp(b), lb, ub);
    }

    /**
     * Sets the bounds of an existing LP column.
     *
     * @param col column identifier
     * @param b row bound type
     * @param lb row lower bound value
     * @param ub row upper bound value
     */
    void set_col_bounds(col_id col, bound_type b, double lb, double ub) {
        glp_set_col_bnds(m_lp, get_col(col), bound_type_to_glp(b), lb, ub);
    }

protected:

    /**
     * Optimizes the LP using the simplex method.
     *
     * @return solution status
     */
    problem_type run_simplex(bool resolve = false) {
        if (!resolve) {
            glp_adv_basis(m_lp, 0);
        }
        int ret = glp_simplex(m_lp, &m_glpk_control);
        if (resolve && ret != 0) {
            // if basis is not valid, create basis and try again
            glp_adv_basis(m_lp, 0);
            ret = glp_simplex(m_lp, &m_glpk_control);
        }
        assert(ret == 0);
        return get_primal_type();
    }

    /**
     * Converts the GLPK soltion status into paal::lp::problem_type.
     *
     * @return solution status
     */
    problem_type get_primal_type() {
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
    int get_col(col_id col) const {
        return m_col_idx.get_idx(col.get()) + 1;
    }

    /**
     * Converts row identifier to the GLPK row index.
     */
    int get_row(row_id row) const {
        return m_row_idx.get_idx(row.get()) + 1;
    }

    /**
     * Converts GLPK column index to the column identifier.
     */
    col_id getcol_idx(int col) const {
        return col_id(m_col_idx.get_val(col - 1));
    }

    /**
     * Converts GLPK row index to the row identifier.
     */
    row_id get_row_idx(int row) const {
        return row_id(m_row_idx.get_val(row - 1));
    }

    typedef std::vector<int> Ids;
    typedef std::vector<double> Vals;

    /**
     * Initializes a given vector.
     */
    template <typename Vec>
    static void init_vec(Vec & v, int numberOfNonZerosInMatrix = 0) {
        v.reserve(++numberOfNonZerosInMatrix);
        v.push_back(0);
    }

    /**
     * Converts paal::lp:bound_type to the GLPK bound type.
     */
    static int bound_type_to_glp(bound_type b) {
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
     * Converts the GLPK bound type to paal::lp:bound_type.
     */
    static bound_type glp_tobound_type(int b) {
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
    data_structures::eraseable_bimap<int> m_col_idx;
    /// mapping between GLPK row numbers and column IDs
    data_structures::eraseable_bimap<int> m_row_idx;

private:

    glp_base(glp_base &&) {}
    glp_base(const glp_base &) {}

    Ids m_row;
    Ids m_col;
    Vals m_val;
    Ids m_new_row_col;
    Vals m_new_row_val;
    glp_smcp m_glpk_control;
    int m_total_col_nr;
    int m_total_row_nr;
};



/**
 * @class GLP
 * @brief The class used for solving LP problems using the GLPK solver.
 *
 * Usage example:
 * @snippet glp_example.cpp GLP Example
 * Complete example can be found in glp_example.cpp
 */
class GLP : public glp_base {
    typedef decltype(std::bind(&GLP::get_row_idx, std::declval<const GLP *>(), std::placeholders::_1)) RowTrans;
    typedef decltype(std::bind(&GLP::getcol_idx, std::declval<const GLP *>(), std::placeholders::_1)) ColTrans;
    typedef typename boost::transform_iterator<RowTrans, typename Ids::iterator, row_id> TransformRow;
    typedef typename boost::transform_iterator<ColTrans, typename Ids::iterator, col_id> TransformCol;
    typedef boost::zip_iterator<boost::tuple<TransformRow,
                                             typename Vals::iterator>> RowsInColumnIterator;
    typedef boost::zip_iterator<boost::tuple<TransformCol,
                                             typename Vals::iterator>> ColsInRowIterator;

    typedef std::unordered_set<row_id> RowSet;
    typedef std::unordered_set<col_id> ColSet;

public:

    typedef RowSet::const_iterator RowIter;
    typedef ColSet::const_iterator ColIter;

    /**
     * Constructor.
     *
     * @param numberOfRows initial number of rows in the LP instance
     * @param numberOfColumns initial number of columns in the LP instance
     * @param numberOfNonZerosInMatrix number of non-zero values in the initial LP instance matrix
     */
    GLP(int numberOfRows, int numberOfColumns, int numberOfNonZerosInMatrix) :
            glp_base(numberOfRows, numberOfColumns, numberOfNonZerosInMatrix) {
        int maxRowCol = std::max(numberOfRows, numberOfColumns);
        init_vec(m_idx_tmp, maxRowCol);
        init_vec(m_val_tmp, maxRowCol);
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

        for(col_id col : boost::make_iterator_range(glp.get_columns())) {
            o << glp.get_col_coef(col) << ", ";
        }
        o << std::endl << "Rows" << std::endl;

        for(row_id row : boost::make_iterator_range(glp.get_rows())) {
            auto cols = glp.get_columns_in_row(row);
            if(cols.first == cols.second) {
                continue;
            }
            o << "Row " << glp.get_row_name(row) << std::endl;
            o << "Bounds " << "type =  " << glp.get_rowbound_type(row) << " lb = " << glp.get_row_lb(row) << " ub = " << glp.get_row_ub(row) << std::endl;
            for(auto colAndVal : boost::make_iterator_range(cols)) {
                col_id  col = boost::get<0>(colAndVal);
                double val = boost::get<1>(colAndVal);
                o << "(col = " << col.get() << " name = " << glp.get_col_name(col) << ", coef = " << val << ") - ";
            }
            o << std::endl;
        }
        o << "Current solution: "<<std::endl;
        for(col_id col : boost::make_iterator_range(glp.get_columns())) {
            o  << glp.get_col_prim(col) << ", ";
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
    col_id add_column(double costCoef = 0, bound_type b = LO, double lb = 0, double ub = 0, const std::string & name = "") {
        resize_tmp();
        col_id colId = glp_base::add_column(costCoef, b, lb, ub, name);
        m_col_ids.insert(colId);
        return colId;
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
    row_id add_row(bound_type b = UP, double lb = 0, double ub = 0, const std::string & name = "") {
        resize_tmp();
        row_id rowId = glp_base::add_row(b, lb, ub, name);
        m_row_ids.insert(rowId);
        return rowId;
    }

    /**
     * Returns column primal value.
     */
    double get_col_prim(col_id col) const {
        return glp_get_col_prim(m_lp, get_col(col));
    }

    /**
     * Returns the number of columns in the LP.
     */
    int col_size() const {
        return glp_get_num_cols(m_lp);
    }

    /**
     * Returns the number of rows in the LP.
     */
    int row_size() const {
        return glp_get_num_rows(m_lp);
    }

    /**
     * Removes a row form the LP.
     *
     * @param row iterator of the row to be removed
     *
     * @return iterator following the removed row
     */
    RowIter delete_row(RowIter row) {
        int arr[2];
        arr[1] = get_row(*row);
        m_row_idx.erase(row->get());
        glp_del_rows(m_lp, 1, arr);
        return m_row_ids.erase(row);
    }

    /**
     * Removes a column form the LP.
     *
     * @param col iterator of the column to be removed
     *
     * @return iterator following the removed column
     */
    ColIter delete_col(ColIter col) {
        int arr[2];
        arr[1] = get_col(*col);
        m_col_idx.erase(col->get());
        glp_del_cols(m_lp, 1, arr);
        return m_col_ids.erase(col);
    }

    /**
     * Clears the LP instance.
     */
    void clear() {
        auto rows = get_rows();
        while (rows.first != rows.second) {
            rows.first = delete_row(rows.first);
        }
        auto cols = get_columns();
        while (cols.first != cols.second) {
            cols.first = delete_col(cols.first);
        }
    }

    /**
     * Returns the number of non-zero coefficients in the given LP matrix column.
     */
    int get_col_degree(col_id col) const {
        return glp_get_mat_col(m_lp, get_col(col), &m_idx_tmp[0], &m_val_tmp[0]);
    }

    /**
     * Returns the number of non-zero coefficients in the given LP matrix row.
     */
    int get_row_degree(row_id row) const {
        return glp_get_mat_row(m_lp, get_row(row), &m_idx_tmp[0], &m_val_tmp[0]);
    }

    /**
     * Returns the sum of the values of those columns, which have got non-zero coefficients
     * in the given LP row.
     */
    double get_row_sum(row_id row) const {
        int size = glp_get_mat_row(m_lp, get_row(row), &m_idx_tmp[0], NULL);
        return get_sol_sum_for_ids(m_idx_tmp.begin() + 1, m_idx_tmp.begin() + size + 1);
    }

    /**
     * Returns the column symbolic name.
     */
    std::string get_col_name(col_id col) const {
        const char * name = glp_get_col_name(m_lp, get_col(col));
        if(name == NULL) {
            return "";
        }
        return name;
    }

    /**
     * Returns the column cost function coefficient.
     */
    double get_col_coef(col_id col) const {
        return glp_get_obj_coef(m_lp, get_col(col));
    }

    /**
     * Returns the column upper bound.
     */
    double get_col_ub(col_id col) const {
        return glp_get_col_ub(m_lp, get_col(col));
    }

    /**
     * Returns the column lower bound.
     */
    double get_col_lb(col_id col) const {
        return glp_get_col_lb(m_lp, get_col(col));
    }

    /**
     * Returns the row symbolic name.
     */
    std::string get_row_name(row_id row) const {
        const char * name = glp_get_row_name(m_lp, get_row(row));
        if(name == NULL) {
            return "";
        }
        return name;
    }

    /**
     * Returns the row upper bound.
     */
    double get_row_ub(row_id row) const {
        return glp_get_row_ub(m_lp, get_row(row));
    }

    /**
     * Returns the row lower bound.
     */
    double get_row_lb(row_id row) const {
        return glp_get_row_lb(m_lp, get_row(row));
    }

    /**
     * Returns the row bound type.
     */
    bound_type get_rowbound_type(row_id row) const {
        return glp_tobound_type(glp_get_row_type(m_lp, get_row(row)));
    }

    /**
     * Returns the column bound type.
     */
    bound_type get_colbound_type(col_id col) const {
        return glp_tobound_type(glp_get_col_type(m_lp, get_col(col)));
    }


    /**
     * Returns the idetifiers and coefficients of all rows in a given column,
     * which constraint matrix coefficient is non-zero (as an iterator range).
     */
    std::pair<RowsInColumnIterator, RowsInColumnIterator>
            get_rows_in_column(col_id col) const {
        int size = glp_get_mat_col(m_lp, get_col(col), &m_idx_tmp[0], &m_val_tmp[0]);
        return std::make_pair(
                boost::make_zip_iterator(boost::make_tuple(TransformRow(m_idx_tmp.begin() + 1, std::bind(&GLP::get_row_idx, this, std::placeholders::_1)), m_val_tmp.begin() + 1)),
                boost::make_zip_iterator(boost::make_tuple(TransformRow(m_idx_tmp.begin() + size + 1, std::bind(&GLP::get_row_idx, this, std::placeholders::_1)), m_val_tmp.begin() + 1 + size)));
    }

    /**
     * Returns the idetifiers and coefficients of all columns in a given row,
     * which constraint matrix coefficient is non-zero (as an iterator range).
     */
    std::pair<ColsInRowIterator, ColsInRowIterator>
            get_columns_in_row(row_id row) const {
        int size = glp_get_mat_row(m_lp, get_row(row), &m_idx_tmp[0], &m_val_tmp[0]);
        return std::make_pair(
                boost::make_zip_iterator(boost::make_tuple(TransformCol(m_idx_tmp.begin() + 1, std::bind(&GLP::getcol_idx, this, std::placeholders::_1)), m_val_tmp.begin() + 1)),
                boost::make_zip_iterator(boost::make_tuple(TransformCol(m_idx_tmp.begin() + size + 1, std::bind(&GLP::getcol_idx, this, std::placeholders::_1)), m_val_tmp.begin() + 1 + size)));
    }

    /**
     * Returns all column identifiers (as an iterator range).
     */
    std::pair<ColIter, ColIter> get_columns() const {
        return std::make_pair(m_col_ids.begin(), m_col_ids.end());
    }

    /**
     * Returns all row identifiers (as an iterator range).
     */
    std::pair<RowIter, RowIter> get_rows() const {
        return std::make_pair(m_row_ids.begin(), m_row_ids.end());
    }

private:

    template <typename Iter>
    double get_sol_sum_for_ids(Iter begin, Iter end) const  {
        return std::accumulate(begin, end, 0., [=](double sum, int u){
            return sum + glp_get_col_prim(m_lp, u);
        });
    }

    void resize_tmp() {
        m_idx_tmp.push_back(0);
        m_val_tmp.push_back(0);
    }

    mutable Ids m_idx_tmp;
    mutable Vals m_val_tmp;

    ColSet m_col_ids;
    RowSet m_row_ids;
};

} //lp
} //paal

#endif /* LPBASE_HPP */
