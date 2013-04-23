/* sample.c */
#include <glpk.h>
#include <cstdlib>
#include <string>

#include <boost/range/irange.hpp>

#include "paal/utils/type_functions.hpp"
#include "paal/utils/iterator_utils.hpp"

namespace paal {

class LPBase {
public:
    LPBase(int initResSize) :
        m_lp(glp_create_prob()) {
        initVec(m_row, initResSize);
        initVec(m_col, initResSize);
        initVec(m_val, initResSize);
    }
    ~LPBase() {
        glp_delete_prob(m_lp);
    }
protected:
    template <typename Vec>
    void initVec(Vec & v, int initResSize) {
        v.reserve(initResSize);
        v.push_back(0);
    }

    glp_prob * m_lp; 
    std::vector<int> m_row;
    std::vector<int> m_col;
    std::vector<double> m_val;
};


template <typename MachineIter, typename JobIter, typename Cost, typename ProceedingTime, typename MachineAvailableTime>
class GeneralAssignement : public LPBase {
public:
    GeneralAssignement(MachineIter mbegin, MachineIter mend, 
                      JobIter jbegin, JobIter jend,
                      Cost c, ProceedingTime t, MachineAvailableTime T) : LPBase(2 * (std::distance(mbegin, mend)* std::distance(jbegin, jend) + 1)),  
                        mNr(std::distance(mbegin, mend)), jNr(std::distance(jbegin, jend)), idx{mNr} {
        glp_set_prob_name(m_lp, "generalized assignement problem");
        glp_set_obj_dir(m_lp, GLP_MIN);

        addVariables(mbegin, mend, jbegin, jend, c);
        addConstraintsForJobs();
        addConstraintsForMachines(mbegin, mend, jbegin, jend, t, T);
   
        LOG("size " << m_row.size() -1);
        glp_load_matrix(m_lp, m_row.size() - 1, &m_row[0], &m_col[0], &m_val[0]);

    }

    double solve() {
        glp_simplex(m_lp, NULL);
        return glp_get_obj_val(m_lp);
    }

protected:
    typedef typename std::iterator_traits<JobIter>::reference JobRef;
    typedef typename std::iterator_traits<MachineIter>::reference MachineRef;

    //adding varables
    void addVariables(MachineIter mbegin, MachineIter mend, 
                     JobIter jbegin, JobIter jend, Cost c) {
        glp_add_cols(m_lp, mNr * jNr);
        int jIdx(1);
        for(JobRef j : utils::make_range(jbegin, jend)) {
            int mIdx(1);
            for(MachineRef m : utils::make_range(mbegin, mend)) {
                int i = idx(jIdx, mIdx);
                LOG("column = " << i );
                glp_set_col_name(m_lp, i, std::string("x " + std::to_string(jIdx) + " "+ std::to_string(mIdx)).c_str());
                glp_set_col_bnds(m_lp, i, GLP_LO, 0.0, 0.0);
                glp_set_obj_coef(m_lp, i, c(j, m));
                ++mIdx;
            }
            ++jIdx;
        }
    }
        
    //constraints for job
    int addConstraintsForJobs() {
        glp_add_rows(m_lp, jNr);
        int rowIdx(m_row.back() + 1);
        for(int jIdx : boost::irange(1, jNr + 1)) {
            glp_set_row_name(m_lp, rowIdx, std::string("job constraint " + std::to_string(jIdx)).c_str());
            glp_set_row_bnds(m_lp, rowIdx, GLP_FX, 1.0, 1.0);

            for(int mIdx : boost::irange(1, mNr + 1)) {
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
    int addConstraintsForMachines(MachineIter mbegin, MachineIter mend,
                                  JobIter jbegin, JobIter jend, 
                                  ProceedingTime t, MachineAvailableTime T)  {
        int rowIdx(m_row.back() + 1);
        glp_add_rows(m_lp, jNr);
        int mIdx(1);
        for(MachineRef m : utils::make_range(mbegin, mend)) {
            int jIdx(1);
            glp_set_row_name(m_lp, rowIdx, std::string("machine constraint " + std::to_string(mIdx)).c_str());
            glp_set_row_bnds(m_lp, rowIdx, GLP_UP, 0.0, T(m));

            for(JobRef j : utils::make_range(jbegin, jend)) {
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

    class Idx {
    public:
        Idx(int mNr) : m_mNr(mNr) {}
        int operator()(int jIdx, int mIdx) {
            return (jIdx - 1) * m_mNr + mIdx;
        }

    private:
        int m_mNr;
    };
   const int mNr;
   const int jNr;
   Idx idx;


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


