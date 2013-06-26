/**
 * @file generalised_assignment.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-05-06
 */
#ifndef GENERALISED_ASSIGNMENT_HPP
#define GENERALISED_ASSIGNMENT_HPP 
#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/ir_components.hpp"

namespace paal {
namespace ir {

template <typename MachineIter, typename JobIter, typename Cost, typename ProceedingTime, typename MachineAvailableTime>
class GeneralAssignment : public IRComponents<> {
public:
    typedef typename utils::IterToElem<JobIter>::type Job;
    typedef typename utils::IterToElem<MachineIter>::type Machine;
    typedef std::map<Job, Machine> JobsToMachines;

    GeneralAssignment(MachineIter mbegin, MachineIter mend, 
                      JobIter jbegin, JobIter jend,
                      const Cost & c, const ProceedingTime & t, const  MachineAvailableTime & T) : 
                m_mCnt(std::distance(mbegin, mend)), m_jCnt(std::distance(jbegin, jend)),
                m_jbegin(jbegin), m_jend(jend), m_mbegin(mbegin), m_mend(mend),
                m_c(c), m_t(t), m_T(T) {}


    typedef typename std::iterator_traits<JobIter>::reference JobRef;
    typedef typename std::iterator_traits<MachineIter>::reference MachineRef;
    typedef utils::Compare<double> Compare;
    
    template <typename LP>
    bool relaxCondition(const LP & lp, RowId row) {
        return isMachineName(lp.getRowName(row)) && 
                        (
                          lp.getRowDegree(row) <= 1 ||
                                (
                                  lp.getRowDegree(row) == 2 &&
                                  m_compare.ge(lp.getRowSum(row),1)
                                )
                        );
    };

    template <typename LP>
    void init(LP & lp) {
        lp.setLPName("generalized assignment problem");
        lp.setMinObjFun(); 

        addVariables(lp);
        addConstraintsForJobs(lp);
        addConstraintsForMachines(lp);
        lp.loadMatrix();
    }

    template <typename GiveSolution, typename LP>
    JobsToMachines & getSolution(const GiveSolution & sol, const LP &) {
        if(!m_solutionGenerated) {
            m_solutionGenerated = true;
            for(int idx : boost::irange(0, int(m_colIdx.size()))) { 
                if(m_compare.e(sol(m_colIdx[idx]), 1)) {
                    m_jobToMachine.insert(std::make_pair(*(m_jbegin + getJIdx(idx)), *(m_mbegin + getMIdx(idx))));
                }
            }
        }
        return m_jobToMachine;
    }
private:

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
    template <typename LP>
    void addVariables(LP &lp) {
        m_colIdx.reserve(m_mCnt * m_jCnt);
        for(JobRef j : utils::make_range(m_jbegin, m_jend)) {
            for(MachineRef m : utils::make_range(m_mbegin, m_mend)) {
                m_colIdx.push_back(lp.addColumn(m_c(j,m)));
            }
        }
    }
        
    //constraints for job
    template <typename LP>
    void addConstraintsForJobs(LP & lp) {
        for(int jIdx : boost::irange(0, m_jCnt)) {
            RowId rowIdx = lp.addRow(FX, 1.0, 1.0, getJobDesc(jIdx));
            for(int mIdx : boost::irange(0, m_mCnt)) {
                lp.addConstraintCoef(rowIdx, m_colIdx[idx(jIdx,mIdx)]);
                ++mIdx;
            }
        }
    }
        
    //constraints for machines
    template <typename LP>
    void addConstraintsForMachines(LP & lp)  {
        int mIdx(0);
        for(MachineRef m : utils::make_range(m_mbegin, m_mend)) {
            RowId rowIdx = lp.addRow(UP, 0.0, m_T(m), getMachineDesc(mIdx));
            int jIdx(0);

            for(JobRef j : utils::make_range(m_jbegin, m_jend)) {
                assert(m_t(j, m) <= m_T(m));
                lp.addConstraintCoef(rowIdx, m_colIdx[idx(jIdx, mIdx)], m_t(j, m));
                ++jIdx;
            }
            ++mIdx;
        }
    }

    int idx(int jIdx, int mIdx) {
        return jIdx * m_mCnt + mIdx;
    }
    
    int getJIdx(int idx) {
        return idx / m_mCnt;
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
    const Cost & m_c; 
    const ProceedingTime & m_t; 
    const MachineAvailableTime & m_T; 
    JobsToMachines m_jobToMachine;
    const Compare m_compare;
    std::vector<ColId> m_colIdx;
    bool m_solutionGenerated = false;
};

template <typename MachineIter, typename JobIter, typename Cost, typename ProceedingTime, typename MachineAvailableTime>
GeneralAssignment<MachineIter, JobIter, Cost, ProceedingTime, MachineAvailableTime>
make_GeneralAssignment(MachineIter mbegin, MachineIter mend, 
                      JobIter jbegin, JobIter jend,
                      const Cost & c, const  ProceedingTime & t, const  MachineAvailableTime & T) {
    return  GeneralAssignment<MachineIter, JobIter, Cost, ProceedingTime, MachineAvailableTime>(
                mbegin, mend, jbegin, jend, c, t, T);
}


} //ir
} //paal
#endif /* GENERALISED_ASSIGNMENT_HPP */
