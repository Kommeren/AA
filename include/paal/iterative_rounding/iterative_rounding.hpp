/* sample.c */
#include <glpk.h>
#include <cstdlib>
#include <string>
#include <map>
#include <set>

#include <boost/range/irange.hpp>

#include "paal/utils/type_functions.hpp"
#include "paal/utils/iterator_utils.hpp"
#include "paal/utils/double_rounding.hpp"

namespace paal {

class LPBase {
public:
    LPBase(int numberOfRows, int numberOfColumns, int numberOfNonZerosInMatrix) :
        m_lp(glp_create_prob()) {
        glp_create_index(m_lp);
        initVec(m_row, numberOfNonZerosInMatrix);
        initVec(m_col, numberOfNonZerosInMatrix);
        initVec(m_val, numberOfNonZerosInMatrix);
        int maxRowCol = std::max(numberOfRows,  numberOfColumns);
        initVec(m_idxTmp, maxRowCol);
        initVec(m_valTmp, maxRowCol);
    }
    
    LPBase(int numberOfRows, int numberOfColumns) : LPBase(numberOfRows, numberOfColumns, numberOfRows * numberOfColumns)  {}
    
    ~LPBase() {
        glp_delete_prob(m_lp);
    }
    
    double solve() {
        glp_adv_basis(m_lp, 0);
        glp_simplex(m_lp, NULL);
        int colNr = glp_get_num_cols(m_lp);
        LOG("Result:");
        for(int i : boost::irange(1, 1 + colNr)) {
            LOG("col " << i << " val " << glp_get_col_prim(m_lp, i));
        }
        return glp_get_obj_val(m_lp);
    }
protected:


    void loadMatrix() {
        glp_load_matrix(m_lp, m_row.size() - 1, &m_row[0], &m_col[0], &m_val[0]);
    }

    template <typename RoundCondition>
    bool roundGen(RoundCondition  rc) {
        int delelted(0);
        int size = glp_get_num_cols(m_lp);
        LOG("roundGen");
        LOG("size = " << size);
        for(int i = 1; i <= size; ++i) {
            double x = glp_get_col_prim(m_lp, i);
            auto doRound = rc(x, i);
            if(doRound.first) {
                LOG("rounduje = " << i);
                LOG("size policznoy = " << glp_get_num_cols(m_lp));
                roundColToValue(i, doRound.second);
                LOG("porounduje = " << i);
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
                LOG("RELAKSUJE " << i);
                glp_del_rows(m_lp, 1, &i-1);
                --size;
                --i;
            }
        }
        
        return delelted > 0;
    }

    void roundColToValue(int col, double value) {
        LOG("round na col = " << col << " val =  " << value ); 
        int size = glp_get_mat_col(m_lp, col, &m_idxTmp[0], &m_valTmp[0]); 
        for(int i : boost::irange(1, size + 1)) {
            int row = m_idxTmp[i];
            double currUb = glp_get_row_ub(m_lp, row);
            double currLb = glp_get_row_lb(m_lp, row);
            int currType = glp_get_row_type(m_lp, row);
            double diff = m_valTmp[i] * value;
            LOG("Usataiwam bounda na wiersz " << row << " diff = " << diff); 
            glp_set_row_bnds(m_lp, row, currType, currLb - diff, currUb - diff);
        }
        glp_del_cols(m_lp, 1, &col-1);
    }

    int getColDegree(int col) {
        return glp_get_mat_col(m_lp, col, &m_idxTmp[0], &m_valTmp[0]); 
    }
    
    int getRowDegree(int row) {
        return glp_get_mat_row(m_lp, row, &m_idxTmp[0], &m_valTmp[0]); 
    }
    
    int getColSum(int col) {
        int size =  glp_get_mat_col(m_lp, col, &m_idxTmp[0], &m_valTmp[0]); 
        return std::accumulate(m_valTmp.begin(), m_valTmp.begin() + size + 1, 0.);
    }
    
    int getRowSum(int col) {
        int size =  glp_get_mat_row(m_lp, col, &m_idxTmp[0], &m_valTmp[0]); 
        return std::accumulate(m_valTmp.begin(), m_valTmp.begin() + size + 1, 0.);
    }

    std::string getColName(int col) {
        return glp_get_col_name(m_lp, col);
    }
    
    std::string getRowName(int row) {
        return glp_get_row_name(m_lp, row);
    }


    glp_prob * m_lp; 
    std::vector<int> m_row;
    std::vector<int> m_col;
    std::vector<double> m_val;

private:
    template <typename Vec>
    void initVec(Vec & v, int numberOfNonZerosInMatrix = 0) {
        v.reserve(++numberOfNonZerosInMatrix);
        v.push_back(0);
    }


    std::vector<int> m_rowTmp;
    std::vector<int> m_idxTmp;
    std::vector<double> m_valTmp;
};


template <typename MachineIter, typename JobIter, typename Cost, typename ProceedingTime, typename MachineAvailableTime>
class GeneralAssignement : public LPBase {
public:
    typedef typename utils::IterToElem<JobIter>::type Job;
    typedef typename utils::IterToElem<MachineIter>::type Machine;

    GeneralAssignement(MachineIter mbegin, MachineIter mend, 
                      JobIter jbegin, JobIter jend,
                      Cost c, ProceedingTime t, MachineAvailableTime T) : 
        LPBase(std::distance(jbegin, jend) + std::distance(mbegin, mend), 
               std::distance(jbegin, jend) * std::distance(mbegin, mend), 
               2 * (std::distance(mbegin, mend) * std::distance(jbegin, jend))),  
                m_mCnt(std::distance(mbegin, mend)), m_jCnt(std::distance(jbegin, jend)),
                m_jbegin(jbegin), m_jend(jend), m_mbegin(mbegin), m_mend(mend) {
        glp_set_prob_name(m_lp, "generalized assignement problem");
        glp_set_obj_dir(m_lp, GLP_MIN);

        addVariables(c);
        addConstraintsForJobs();
        addConstraintsForMachines(t, T);
   
        LOG("size " << m_row.size() -1);
        loadMatrix();
    }



    bool round() {
        return roundGen(&GeneralAssignement::roundCondition);
    }

    bool relax() {
        return relaxGen(std::bind(&GeneralAssignement::relaxCondition, this, std::placeholders::_1, std::placeholders::_2));
    }

protected:
    typedef typename std::iterator_traits<JobIter>::reference JobRef;
    typedef typename std::iterator_traits<MachineIter>::reference MachineRef;
    typedef utils::Compare<double> Compare;

    static std::pair<bool, double> roundCondition(double x, int col) {
        LOG("SPRAWDZAM CZY ZAOKRAGLAC: Columna = " << col << " val =  " << x);
        for(int j = 0; j < 2; ++j) {
            if(Compare::e(x,j)) {
                LOG("ZAOKRAGLAM");
                return std::make_pair(true, j);
            }
        }
        LOG("NIE ZAOKRAGLAM");
        return std::make_pair(false, -1);
    };
    
    bool relaxCondition(double x, int row) {
        return isMachineName(getRowName(row)) && 
               getRowDegree(row) <= 2 && 
               Compare::ge(getRowSum(row),1); 
    };

    std::string getMachinePrefix() const {
        return "machine ";
    }

    std::string getJobDesc(int jIdx) {
        return "job " + std::to_string(jIdx);
    }
    
    std::string getMachineDesc(int mIdx) {
        return getMachinePrefix() + std::to_string(mIdx);
    }

    bool isMachineName(const std::string & s) {
        return s.compare(0, getMachinePrefix().size(), getMachinePrefix()) == 0;
    }

    //adding varables
    void addVariables(Cost & c) {
        glp_add_cols(m_lp, m_mCnt * m_jCnt);
        int jIdx(1);
        for(JobRef j : utils::make_range(m_jbegin, m_jend)) {
            int mIdx(1);
            for(MachineRef m : utils::make_range(m_mbegin, m_mend)) {
                int i = idx(jIdx, mIdx);
                LOG("column = " << i );
                glp_set_col_name(m_lp, i, std::to_string(i).c_str());
                glp_set_col_bnds(m_lp, i, GLP_LO, 0.0, 0.0);
                glp_set_obj_coef(m_lp, i, c(j, m));
                ++mIdx;
            }
            ++jIdx;
        }
    }
        
    //constraints for job
    int addConstraintsForJobs() {
        glp_add_rows(m_lp, m_jCnt);
        int rowIdx(m_row.back() + 1);
        for(int jIdx : boost::irange(1, m_jCnt + 1)) {
            glp_set_row_name(m_lp, rowIdx, getJobDesc(jIdx).c_str());
            glp_set_row_bnds(m_lp, rowIdx, GLP_FX, 1.0, 1.0);

            for(int mIdx : boost::irange(1, m_mCnt + 1)) {
                LOG("row = " << rowIdx << " idx " << idx(jIdx, mIdx));
                m_row.push_back(rowIdx);
                m_col.push_back(idx(jIdx, mIdx));
                m_val.push_back(1);
                ++mIdx;
            }
            ++rowIdx;
        }
        return rowIdx;
    }
        
    //constraints for machines
    int addConstraintsForMachines(ProceedingTime & t, MachineAvailableTime & T)  {
        int rowIdx(m_row.back() + 1);
        glp_add_rows(m_lp, m_mCnt);
        int mIdx(1);
        for(MachineRef m : utils::make_range(m_mbegin, m_mend)) {
            int jIdx(1);
            glp_set_row_name(m_lp, rowIdx, getMachineDesc(mIdx).c_str());
            glp_set_row_bnds(m_lp, rowIdx, GLP_UP, 0.0, T(m));

            for(JobRef j : utils::make_range(m_jbegin, m_jend)) {
                LOG("row = " << rowIdx << " idx " << idx(jIdx, mIdx));
                m_row.push_back(rowIdx);
                m_col.push_back(idx(jIdx, mIdx));
                m_val.push_back(t(j, m));
                ++jIdx;
            }
            ++mIdx;
            ++rowIdx;
        }
        return rowIdx;
    }

    int idx(int jIdx, int mIdx) {
        return (jIdx - 1) * m_mCnt + mIdx;
    }
    
    int getJIdx(int idx) {
        return idx / m_mCnt + 1;
    }
    
    int getMIdx(int idx) {
        return idx % m_mCnt;
    }

    const int m_mCnt;
    const int m_jCnt;
    JobIter m_jbegin;
    JobIter m_jend;
    MachineIter m_mbegin;
    MachineIter m_mend;
    std::map<Job, Machine> m_jobToMachine;
};


template <typename MachineIter, typename JobIter, typename Cost, typename ProceedingTime, typename MachineAvailableTime>
GeneralAssignement<MachineIter, JobIter, Cost, ProceedingTime, MachineAvailableTime>
make_GeneralAssignement(MachineIter mbegin, MachineIter mend, 
                      JobIter jbegin, JobIter jend,
                      Cost c, ProceedingTime t, MachineAvailableTime T) {
    return  GeneralAssignement<MachineIter, JobIter, Cost, ProceedingTime, MachineAvailableTime>(
                mbegin, mend, jbegin, jend, c, t, T);
}

} //paal


