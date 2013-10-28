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

namespace paal {
namespace ir {

   
/**
 * class GLPBase
 * @brief This class contains member functions needed to initialize LP
 */


class GLPBase {
public:
    GLPBase(int numberOfRows, int numberOfColumns, int numberOfNonZerosInMatrix) :
        m_lp(glp_create_prob()), m_totalColNr(0), m_totalRowNr(0) {
        glp_create_index(m_lp);
        initVec(m_row, numberOfNonZerosInMatrix);
        initVec(m_col, numberOfNonZerosInMatrix);
        initVec(m_val, numberOfNonZerosInMatrix);
        initVec(m_newRowCol);
        initVec(m_newRowVal);
        glp_init_smcp(&m_glpkControl);
        m_glpkControl.msg_lev = GLP_MSG_OFF;
    }
    
    GLPBase(int numberOfRows = 0, int numberOfColumns = 0) : GLPBase(numberOfRows, numberOfColumns, numberOfRows * numberOfColumns)  {}
    
    ~GLPBase() {
        glp_delete_prob(m_lp);
    }
     
    void setLPName(const std::string & s){
        glp_set_prob_name(m_lp, s.c_str());
    }
    
    void setMinObjFun() {
        glp_set_obj_dir(m_lp, GLP_MIN);
    }
    
    double solveToExtremePoint() {
        glp_adv_basis(m_lp, 0);
        int ret = glp_simplex(m_lp, &m_glpkControl);
        assert(ret == 0);
        return glp_get_obj_val(m_lp);
    }
    

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
    
    void addConstraintCoef(RowId row, ColId col, double coef = 1) {
        m_row.push_back(getRow(row));
        m_col.push_back(getCol(col));
        m_val.push_back(coef);
    }
    
    void loadMatrix() {
        glp_load_matrix(m_lp, m_row.size() - 1, &m_row[0], &m_col[0], &m_val[0]);
        m_originalColSize = glp_get_num_cols(m_lp);
        m_originalRowSize = glp_get_num_rows(m_lp);
    }
   

    void addNewRowCoef(ColId col, double coef = 1) {
        m_newRowCol.push_back(getCol(col));
        m_newRowVal.push_back(coef);
    }
    
    void loadNewRow() {
        int rowNr = glp_get_num_rows(m_lp);
        glp_set_mat_row(m_lp, rowNr, m_newRowCol.size() - 1, &m_newRowCol[0], &m_newRowVal[0]);
        m_newRowCol.clear();
        m_newRowVal.clear();
        initVec(m_newRowCol);
        initVec(m_newRowVal);
    }

    void setRowBounds(RowId row, BoundType b, double lb, double ub) {
        glp_set_row_bnds(m_lp, getRow(row), boundTypeToGLP(b), lb, ub);
    }
    
    void setColBounds(ColId col, BoundType b, double lb, double ub) {
        glp_set_col_bnds(m_lp, getCol(col), boundTypeToGLP(b), lb, ub);
    }
    
protected:

    int getCol(ColId col) const {
        return m_colIdx.getIdx(col.get()) + 1;
    }
    
    int getRow(RowId row) const {
        return m_rowIdx.getIdx(row.get()) + 1;
    }
    
    ColId getColIdx(int col) const {
        return ColId(m_colIdx.getVal(col - 1));
    }

    RowId getRowIdx(int row) const {
        return RowId(m_rowIdx.getVal(row - 1));
    }

    typedef std::vector<int> Ids;
    typedef std::vector<double> Vals;

    template <typename Vec>
    void initVec(Vec & v, int numberOfNonZerosInMatrix = 0) {
        v.reserve(++numberOfNonZerosInMatrix);
        v.push_back(0);
    }

    static int boundTypeToGLP(BoundType b) {
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
   
    int m_originalColSize;
    int m_originalRowSize;
    glp_prob * m_lp;
    data_structures::EraseableBiMap<int> m_colIdx;
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
    GLP(int numberOfRows, int numberOfColumns, int numberOfNonZerosInMatrix) : 
            GLPBase(numberOfRows, numberOfColumns, numberOfNonZerosInMatrix) {
        int maxRowCol = std::max(numberOfRows,  numberOfColumns);
        initVec(m_idxTmp, maxRowCol);
        initVec(m_valTmp, maxRowCol);
    }
    
    GLP(int numberOfRows = 0, int numberOfColumns = 0) : 
        GLP(numberOfRows, numberOfColumns, numberOfRows * numberOfColumns)  {}
    
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
    
    ColId addColumn(double costCoef = 0, BoundType b= LO, double lb = 0, double ub = 0, const std::string & name = "") {
        resizeTmp();
        return GLPBase::addColumn(costCoef, b, lb, ub, name);
    }
   
    RowId addRow(BoundType b = UP, double lb = 0, double ub = 0, const std::string & name = "") {
        resizeTmp();
        return GLPBase::addRow(b, lb, ub, name);
    }

    double getColPrim(ColId col) const {
        return glp_get_col_prim(m_lp, getCol(col));
    }

    int colSize() const {
        return glp_get_num_cols(m_lp); 
    }
    
    int rowSize() const {
        return glp_get_num_rows(m_lp); 
    }

    void deleteRow(RowId row) {
        int arr[2];
        arr[1] = getRow(row);
        m_rowIdx.erase(row.get());
        glp_del_rows(m_lp, 1, arr);
    }
    
    void deleteCol(ColId col) {
        int arr[2];
        arr[1] = getCol(col);
        m_colIdx.erase(col.get());
        glp_del_cols(m_lp, 1, arr);
    }
   
    //TODO rewrite
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

    int getColDegree(ColId col) const {
        return glp_get_mat_col(m_lp, getCol(col), &m_idxTmp[0], &m_valTmp[0]); 
    }
    
    int getRowDegree(RowId row) const {
        return glp_get_mat_row(m_lp, getRow(row), &m_idxTmp[0], &m_valTmp[0]); 
    }
    
    double getColSum(ColId col) const {
        int size =  glp_get_mat_col(m_lp, getCol(col), &m_idxTmp[0], NULL); 
        return getSolSumForIds(m_idxTmp.begin() + 1, m_idxTmp.begin() + size + 1);
    }
    
    double getRowSum(RowId row) const {
        int size =  glp_get_mat_row(m_lp, getRow(row), &m_idxTmp[0], NULL); 
        return getSolSumForIds(m_idxTmp.begin() + 1, m_idxTmp.begin() + size + 1);
    }

    std::string getColName(ColId col) const {
        const char * name = glp_get_col_name(m_lp, getCol(col));
        if(name == NULL) {
            return "";
        }
        return name;
    }
    
    ColId getColByName(const std::string & colName) const {
        return getColIdx(glp_find_col(m_lp, colName.c_str()));
    }
    
    double getColUb(ColId col) const {
        return glp_get_col_ub(m_lp, getCol(col));
    }
    
    double getColLb(ColId col) const {
        return glp_get_col_lb(m_lp, getCol(col));
    }
    
    std::string getRowName(RowId row) const {
        const char * name = glp_get_row_name(m_lp, getRow(row));
        if(name == NULL) {
            return "";
        }
        return name;
    }
    
    int getRowByName(const std::string & rowName) const {
        return glp_find_row(m_lp, rowName.c_str());
    }
    
    double getRowUb(RowId row) const {
        return glp_get_row_ub(m_lp, getRow(row));
    }
    
    double getRowLb(RowId row) const {
        return glp_get_row_lb(m_lp, getRow(row));
    }
    
    BoundType getRowBoundType(RowId row) const {
        return glpToBoundType(glp_get_row_type(m_lp, getRow(row)));
    }
    
    BoundType getColBoundType(ColId col) const {
        return glpToBoundType(glp_get_col_type(m_lp, getCol(col)));
    }
   

    std::pair<RowsInColumnIterator, RowsInColumnIterator>
            getRowsInColumn(ColId col) const {
        int size = glp_get_mat_col(m_lp, getCol(col), &m_idxTmp[0], &m_valTmp[0]);
        return std::make_pair(
                boost::make_zip_iterator(boost::make_tuple(TransformRow(m_idxTmp.begin() + 1, std::bind(&GLP::getRowIdx, this, std::placeholders::_1)), m_valTmp.begin() + 1)),
                boost::make_zip_iterator(boost::make_tuple(TransformRow(m_idxTmp.begin() + size + 1, std::bind(&GLP::getRowIdx, this, std::placeholders::_1)), m_valTmp.begin() + 1 + size)));
    }
    
    std::pair<ColsInRowIterator, ColsInRowIterator>
            getColumnsInRow(RowId row) const {
        int size = glp_get_mat_row(m_lp, getRow(row), &m_idxTmp[0], &m_valTmp[0]);
        return std::make_pair(
                boost::make_zip_iterator(boost::make_tuple(TransformCol(m_idxTmp.begin() + 1, std::bind(&GLP::getColIdx, this, std::placeholders::_1)), m_valTmp.begin() + 1)),
                boost::make_zip_iterator(boost::make_tuple(TransformCol(m_idxTmp.begin() + size + 1, std::bind(&GLP::getColIdx, this, std::placeholders::_1)), m_valTmp.begin() + 1 + size)));
    }

    typedef boost::transform_iterator<ColTrans, decltype(boost::begin(boost::irange(0,0))), ColId> ColIter;
    std::pair<ColIter, ColIter> getColumns() const {
        auto range = boost::irange(1, glp_get_num_cols(m_lp) + 1);
        return std::make_pair(ColIter(boost::begin(range), std::bind(&GLP::getColIdx, this, std::placeholders::_1)),
                              ColIter(boost::end(range)  , std::bind(&GLP::getColIdx, this, std::placeholders::_1)));
    }
    
    typedef boost::transform_iterator<RowTrans, decltype(boost::begin(boost::irange(0,0))), RowId> RowIter;
    std::pair<RowIter, RowIter> getRows() const {
        auto range = boost::irange(1, glp_get_num_rows(m_lp) + 1);
        return std::make_pair(RowIter(boost::begin(range), std::bind(&GLP::getRowIdx, this, std::placeholders::_1)),
                              RowIter(boost::end(range)  , std::bind(&GLP::getRowIdx, this, std::placeholders::_1)));
    }
    
    typedef boost::transform_iterator<decltype(&make_ColId), decltype(boost::begin(boost::irange(0,0))), ColId> OrigColIter;
    std::pair<OrigColIter, OrigColIter> getOriginalColumns() const {
        auto range = boost::irange(1, m_originalColSize +  1);
        return std::make_pair(OrigColIter(boost::begin(range), &make_ColId),
                              OrigColIter(boost::end(range)  , &make_ColId));
    }
    
/*    typedef boost::transform_iterator<&Row, decltype(boost::begin(boost::irange(0,0))), RowId> OrigRowIter;
    std::pair<OrigRowIter, OrigRowIter> getOriginalRows() const {
        auto range = boost::irange(1, m_originalRowSize + 1);
        return std::make_pair(OrigRowIter(boost::begin(range), std::bind(&GLP::getRowIdx, this, std::placeholders::_1)),
                              OrigRowIter(boost::end(range)  , std::bind(&GLP::getRowIdx, this, std::placeholders::_1)));
    }*/

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


} //ir
} //paal

#endif /* LPBASE_HPP */
