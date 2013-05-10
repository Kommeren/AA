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
#include "paal/iterative_rounding/default_engine.hpp"

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
    std::pair<bool, double> roundCondition(const LP & lp, int col) {
        auto ret = IRComponents<>::roundCondition(lp, col);
        if(ret.first && ret.second == 1) {
             int idx = std::stoi(lp.getColName(col));
              m_jobToMachine.insert(std::make_pair(*(m_jbegin +  (getJIdx(idx) - 1)), *(m_mbegin +  (getMIdx(idx) - 1))));
        }
        return ret;
    };
    
    template <typename LP>
    bool relaxCondition(const LP & lp, int row) {
        return isMachineName(lp.getRowName(row)) && 
               lp.getRowDegree(row) <= 2 && 
               Compare::ge(lp.getRowSum(row),1); 
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

    JobsToMachines & getSolution() {
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
        int jIdx(1);
        for(JobRef j : utils::make_range(m_jbegin, m_jend)) {
            int mIdx(1);
            for(MachineRef m : utils::make_range(m_mbegin, m_mend)) {
                lp.addColumn(m_c(j,m));
                ++mIdx;
            }
            ++jIdx;
        }
    }
        
    //constraints for job
    template <typename LP>
    void addConstraintsForJobs(LP & lp) {
        for(int jIdx : boost::irange(1, m_jCnt + 1)) {
            int rowIdx = lp.addRow(FX, 1.0, 1.0, getJobDesc(jIdx));
            for(int mIdx : boost::irange(1, m_mCnt + 1)) {
                lp.addConstraintCoef(rowIdx, idx(jIdx,mIdx));
                ++mIdx;
            }
        }
    }
        
    //constraints for machines
    template <typename LP>
    void addConstraintsForMachines(LP & lp)  {
        int mIdx(1);
        for(MachineRef m : utils::make_range(m_mbegin, m_mend)) {
            int rowIdx = lp.addRow(UP, 0.0, m_T(m), getMachineDesc(mIdx));
            int jIdx(1);

            for(JobRef j : utils::make_range(m_jbegin, m_jend)) {
                lp.addConstraintCoef(rowIdx, idx(jIdx, mIdx), m_t(j, m));
                ++jIdx;
            }
            ++mIdx;
        }
    }

    int idx(int jIdx, int mIdx) {
        return (jIdx - 1) * m_mCnt + mIdx;
    }
    
    int getJIdx(int idx) {
        return (idx - 1) / m_mCnt + 1;
    }
    
    int getMIdx(int idx) {
        return (idx - 1) % m_mCnt + 1;
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
