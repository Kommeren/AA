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
    LPBase(int initResSize) :
        m_lp(glp_create_prob()) {
        glp_create_index(m_lp);
        initVec(m_row, initResSize);
        initVec(m_col, initResSize);
        initVec(m_val, initResSize);
        initVec(m_idxTmp, initResSize);
        initVec(m_valTmp, initResSize);
    }
    ~LPBase() {
        glp_delete_prob(m_lp);
    }
protected:


    void loadMatrix() {
        glp_load_matrix(m_lp, m_row.size() - 1, &m_row[0], &m_col[0], &m_val[0]);
    }

/*    double getCoef(int row, int col) {
        glp_get_mat_row(m_lp, row, &m_idxTmp[0], &m_valTmp[0]);
        auto idx = std::find_if(m_idxTmp.begin(), m_idxTmp.end(), [&](int i){return i == col;});
        assert(idx != m_idxTmp.end());
        return m_valTmp[idx - m_idxTmp.begin()];
    }*/
    
    template <typename RoundCondition>
    bool round(RoundCondition & rc) {
        int delelted(0);
        int size = glp_get_num_cols(m_lp);
        for(int i = 0; i < size; ++i) {
            double x = glp_get_col_prim(m_lp, i);
            std::string s = glp_get_col_name(m_lp, i);
            auto doRound = rc(x, s);
            if(doRound.first) {
                roundColToValue(i, doRound.second);
                --size;
                --i;
            }
        }
        
        return delelted > 0;
    }

    
    template <typename RelaxCondition>
    bool relax(RelaxCondition & rc) {
        int delelted(0);
        int size = glp_get_num_rows(m_lp);
        for(int i = 0; i < size; ++i) {
            double x = glp_get_row_prim(m_lp, i);
            std::string s = glp_get_row_name(m_lp, i);
            if(rc(x,s)) { 
                glp_del_rows(m_lp, 1, &i-sizeof(int));
                --size;
                --i;
            }
        }
        
        return delelted > 0;
    }

    void roundColToValue(int col, double value) {
        int size = glp_get_mat_col(m_lp, col, &m_idxTmp[0], &m_valTmp[0]); 
        for(int i : boost::irange(1, size + 1)) {
            double currUb = glp_get_row_ub(m_lp, m_idxTmp[i]);
            double currLb = glp_get_row_lb(m_lp, m_idxTmp[i]);
            int currType = glp_get_row_type(m_lp, m_idxTmp[i]);
            double diff = m_valTmp[i] * value;
            glp_set_row_bnds(m_lp, m_idxTmp[i], currType, currLb - diff, currUb - diff);
        }
        
        glp_del_cols(m_lp, 1, &col-sizeof(int));
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

    int getCol(std::string & name) {
        return glp_find_col(m_lp, name.c_str());
    }
    
    int getRow(std::string & name) {
        return glp_find_row(m_lp, name.c_str());
    }


    glp_prob * m_lp; 
    std::vector<int> m_row;
    std::vector<int> m_col;
    std::vector<double> m_val;

private:
    template <typename Vec>
    void initVec(Vec & v, int initResSize = 1) {
        v.reserve(initResSize);
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
                      Cost c, ProceedingTime t, MachineAvailableTime T) : LPBase(2 * (std::distance(mbegin, mend)* std::distance(jbegin, jend) + 1)),  
                        m_mCnt(std::distance(mbegin, mend)), m_jCnt(std::distance(jbegin, jend)),
                        m_jbegin(jbegin), m_jend(jend), m_mbegin(mbegin), m_mend(mend){
        glp_set_prob_name(m_lp, "generalized assignement problem");
        glp_set_obj_dir(m_lp, GLP_MIN);

        addVariables(c);
        addConstraintsForJobs();
        addConstraintsForMachines(t, T);
   
        LOG("size " << m_row.size() -1);
        loadMatrix();
    }

    double solve() {
        glp_simplex(m_lp, NULL);
        return glp_get_obj_val(m_lp);
    }


    bool round() {
        return round(&GeneralAssignement::roundCondition);
    }

    void relax() {
        return relax(std::bind(&GeneralAssignement::relaxCondition, this));
    }

protected:
    typedef typename std::iterator_traits<JobIter>::reference JobRef;
    typedef typename std::iterator_traits<MachineIter>::reference MachineRef;
    typedef utils::Compare<double> Compare;

    static std::pair<bool, double> roundCondition(double x, const std::string &) {
        for(int j = 0; j < 2; ++j) {
            if(Compare::e(x,j)) {
                return std::make_pair(true, j);
            }
        }
        return std::make_pair(false, -1);
    };
    
    bool relaxCondition(double x, const std::string &name) {
        int row = getRow(name);
        return getRowDegree(row) <= 2 && Compare::ge(getRowSum(row),1); 
    };

    std::string getJobDesc(int jIdx) {
        return "job " + std::to_string(jIdx);
    }
    
    std::string getMachineDesc(int mIdx) {
        return "machine " + std::to_string(mIdx);
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
        glp_add_rows(m_lp, m_jCnt);
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


