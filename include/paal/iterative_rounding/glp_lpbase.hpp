/**
 * @file glp_lpbase.hpp
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
#include "bound_type.hpp"

namespace paal {
namespace ir {

class GLPBase {
public:
    GLPBase(int numberOfRows, int numberOfColumns, int numberOfNonZerosInMatrix) :
        m_lp(glp_create_prob()) {
        glp_create_index(m_lp);
        initVec(m_row, numberOfNonZerosInMatrix);
        initVec(m_col, numberOfNonZerosInMatrix);
        initVec(m_val, numberOfNonZerosInMatrix);
        initVec(m_newRowCol);
        initVec(m_newRowVal);
    }
    
    GLPBase(int numberOfRows = 0, int numberOfColumns = 0) : GLPBase(numberOfRows, numberOfColumns, numberOfRows * numberOfColumns)  {}
    
    ~GLPBase() {
        glp_delete_prob(m_lp);
    }
     
    void setLPName(const std::string &s ){
        glp_set_prob_name(m_lp, s.c_str());
    }
    
    void setMinObjFun() {
        glp_set_obj_dir(m_lp, GLP_MIN);
    }
    
    double solve() {
        glp_adv_basis(m_lp, 0);
        glp_simplex(m_lp, NULL);
        return glp_get_obj_val(m_lp);
    }
    

    int addColumn(double costCoef, BoundType b, double lb, double ub, const std::string & name) {
        glp_add_cols(m_lp, 1);
        glp_set_col_name(m_lp, m_colNr, name.c_str());
        glp_set_col_bnds(m_lp, m_colNr, boundType2GLP(b), lb, ub);
        glp_set_obj_coef(m_lp, m_colNr, costCoef);
        return m_colNr++;
    }
    
    //add column with default for name
    int addColumn(double costCoef = 0, BoundType b = LO, double lb = 0, double ub = 0) {
        return addColumn(costCoef, b, lb, ub, std::to_string(m_colNr).c_str());
    }
   
    int addRow(BoundType b, double lb, double ub, const std::string & name) {
        glp_add_rows(m_lp, 1);
        glp_set_row_name(m_lp, m_rowNr, name.c_str());
        glp_set_row_bnds(m_lp, m_rowNr, boundType2GLP(b), lb, ub);
        return m_rowNr++;
    }
    
    void deleteRow() {
        --m_rowNr;
    }

    //add row with default for name
    int addRow(BoundType b = UP, double lb = 0, double ub = 0) {
        return addRow(b, lb, ub, std::to_string(m_rowNr).c_str());
    }
    
    void addConstraintCoef(int row, int col, double coef = 1) {
        m_row.push_back(row);
        m_col.push_back(col);
        m_val.push_back(coef);
    }
    
    void loadMatrix() {
        glp_load_matrix(m_lp, m_row.size() - 1, &m_row[0], &m_col[0], &m_val[0]);
    }
    
    void addNewRowCoef(int col, double coef = 1) {
        m_newRowCol.push_back(col);
        m_newRowVal.push_back(coef);
    }
    
    void loadNewRow() {
        glp_set_mat_row(m_lp, m_rowNr - 1, m_newRowCol.size() - 1, &m_newRowCol[0], &m_newRowVal[0]);
        m_newRowCol.clear();
        m_newRowVal.clear();
        initVec(m_newRowCol);
        initVec(m_newRowVal);
    }

/*    template <typename RoundCondition>
    bool roundGen(RoundCondition  rc) {
        int delelted(0);
        int size = glp_get_num_cols(m_lp);
        for(int i = 1; i <= size; ++i) {
            double x = glp_get_col_prim(m_lp, i);
            auto doRound = rc(x, i);
            if(doRound.first) {
                roundColToValue(i, doRound.second);
                --size;
                --i;
            }
        }
        
        return delelted > 0;
    }

    
    template <typename RelaxCondition>
    bool relaxGen(RelaxCondition  rc) {
        int delelted(0);
        int size = glp_get_num_rows(m_lp);
        for(int i = 1; i <= size; ++i) {
            double x = glp_get_row_prim(m_lp, i);
            if(rc(x,i)) {
                glp_del_rows(m_lp, 1, &i-1);
                --size;
                --i;
            }
        }
        
        return delelted > 0;
    }*/

    void setRowBounds(int row, BoundType b, double lb, double ub) {
        glp_set_row_bnds(m_lp, row, boundType2GLP(b), lb, ub);
    }
protected:
    typedef std::vector<int> Ids;
    typedef std::vector<double> Vals;

    template <typename Vec>
    void initVec(Vec & v, int numberOfNonZerosInMatrix = 0) {
        v.reserve(++numberOfNonZerosInMatrix);
        v.push_back(0);
    }

    static int boundType2GLP(BoundType b) {
        switch(b) {
            case FR:
                return GLP_FR;
            case LO:
                return GLP_LO;
            case UP:
                return GLP_UP;
            case DB: 
                return GLP_DB;
            case FX:
                return GLP_FX;
            default:
                assert(false);
                return GLP_FR;
        }
    }
    
    static BoundType glp2BoundType(int b) {
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
    
    glp_prob * m_lp;
    int m_rowNr = 1;
    int m_colNr = 1;
private:
    GLPBase(GLPBase &&) {} 
    GLPBase(const GLPBase &) {}

    Ids m_row;
    Ids m_col;
    Vals m_val;
    
    Ids m_newRowCol;
    Vals m_newRowVal;
};



class GLP : public GLPBase {
    typedef boost::zip_iterator<boost::tuple<typename Ids::iterator, 
                                             typename Vals::iterator>> ColumnIterator;
public:
    GLP(int numberOfRows, int numberOfColumns, int numberOfNonZerosInMatrix) : 
            GLPBase(numberOfRows, numberOfColumns, numberOfNonZerosInMatrix) {
        int maxRowCol = std::max(numberOfRows,  numberOfColumns);
        initVec(m_idxTmp, maxRowCol);
        initVec(m_valTmp, maxRowCol);
    }
    
    GLP(int numberOfRows = 0, int numberOfColumns = 0) : 
        GLP(numberOfRows, numberOfColumns, numberOfRows * numberOfColumns)  {}
    
    int addColumn(double costCoef, BoundType b, double lb, double ub, const std::string & name) {
        resizeTmp();
        return GLPBase::addColumn(costCoef, b, lb, ub, name);
    }
    
    //add column with default for name
    int addColumn(double costCoef = 0, BoundType b = LO, double lb = 0, double ub = 0) {
        return addColumn(costCoef, b, lb, ub, std::to_string(m_colNr).c_str());
    }
   
    int addRow(BoundType b, double lb, double ub, const std::string & name) {
        resizeTmp();
        return GLPBase::addRow(b, lb, ub, name);
    }

    //add row with default for name
    int addRow(BoundType b = UP, double lb = 0, double ub = 0) {
        return addRow(b, lb, ub, std::to_string(m_rowNr).c_str());
    }

    double getColPrim(int col) const {
        return glp_get_col_prim(m_lp, col);
    }

    int colSize() const {
        return glp_get_num_cols(m_lp); 
    }
    
    int rowSize() const {
        return glp_get_num_rows(m_lp); 
    }

    void deleteRow(int row) {
        GLPBase::deleteRow();
        glp_del_rows(m_lp, 1, &row-1);
    }
    
    void deleteCol(int col) {
        glp_del_cols(m_lp, 1, &col-1);
    }
    
    int getColDegree(int col) const {
        return glp_get_mat_col(m_lp, col, &m_idxTmp[0], &m_valTmp[0]); 
    }
    
    int getRowDegree(int row) const {
        return glp_get_mat_row(m_lp, row, &m_idxTmp[0], &m_valTmp[0]); 
    }
    
    int getColSum(int col) const {
        int size =  glp_get_mat_col(m_lp, col, &m_idxTmp[0], &m_valTmp[0]); 
        return std::accumulate(m_valTmp.begin(), m_valTmp.begin() + size + 1, 0.);
    }
    
    int getRowSum(int col) const {
        int size =  glp_get_mat_row(m_lp, col, &m_idxTmp[0], &m_valTmp[0]); 
        return std::accumulate(m_valTmp.begin(), m_valTmp.begin() + size + 1, 0.);
    }

    std::string getColName(int col) const {
        return glp_get_col_name(m_lp, col);
    }
    
    int getColByName(const std::string & colName) const {
        return glp_find_col(m_lp, colName.c_str());
    }
    
    double getColUb(int col) const {
        return glp_get_col_ub(m_lp, col);
    }
    
    double getColLb(int col) const {
        return glp_get_col_lb(m_lp, col);
    }
    
    std::string getRowName(int row) const {
        return glp_get_row_name(m_lp, row);
    }
    
    int getRowByName(const std::string & rowName) const {
        return glp_find_row(m_lp, rowName.c_str());
    }
    
    double getRowUb(int row) const {
        return glp_get_row_ub(m_lp, row);
    }
    
    double getRowLb(int row) const {
        return glp_get_row_lb(m_lp, row);
    }
    
    BoundType getRowBoundType(int row) const {
        return glp2BoundType(glp_get_row_type(m_lp, row));
    }
    
    std::pair<ColumnIterator, ColumnIterator>
            getColumn(int col) {
        int size = glp_get_mat_col(m_lp, col, &m_idxTmp[0], &m_valTmp[0]);
        return std::make_pair(
                boost::make_zip_iterator(boost::make_tuple(m_idxTmp.begin() + 1, m_valTmp.begin() + 1)),
                boost::make_zip_iterator(boost::make_tuple(m_idxTmp.begin() + size + 1, m_valTmp.begin() + 1 + size)));
    }

private:
    
    void resizeTmp() {
        m_idxTmp.push_back(0);
        m_valTmp.push_back(0);
    }

    mutable Ids m_idxTmp;
    mutable Vals m_valTmp;
};


} //ir
} //paal

#endif /* LPBASE_HPP */
