/**
 * @file generalised_assignment.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-05-06
 */
#ifndef GENERALISED_ASSIGNMENT_HPP
#define GENERALISED_ASSIGNMENT_HPP 

#include "paal/iterative_rounding/ir_components.hpp"
#include "paal/iterative_rounding/iterative_rounding.hpp"

namespace paal {
namespace ir {

struct GARelaxCondition {
    template <typename Solution, typename LP>
        bool operator()(Solution & solution, const LP & lp, RowId row) {
            auto & machineRows = solution.getMachineRows();
            return machineRows.find(row) != machineRows.end() && 
                (
                 lp.getRowDegree(row) <= 1 ||
                 (
                  lp.getRowDegree(row) == 2 &&
                  solution.getCompare().ge(lp.getRowSum(row), 1)
                 )
                );
        }
};

struct GASetSolution {
    template <typename Solution, typename GetSolution>
        void operator()(Solution & solution, const GetSolution & getsol) {
            auto jbegin = solution.getJobs().first;
            auto mbegin = solution.getMachines().first;
            auto & colIdx = solution.getColIdx();
            auto & jobToMachine = solution.getJobToMachines();

            for(int idx : boost::irange(0, int(colIdx.size()))) { 
                if(solution.getCompare().e(getsol(colIdx[idx]), 1)) {
                    jobToMachine.insert(std::make_pair(*(jbegin + solution.getJIdx(idx)), 
                                                         *(mbegin + solution.getMIdx(idx))));
                }
            }
        }
};

class GAInit {
    public:
        template <typename Solution, typename LP>
            void operator()(Solution & solution, LP & lp) {
                lp.setLPName("generalized assignment problem");
                lp.setMinObjFun(); 

                addVariables(solution, lp);
                addConstraintsForJobs(solution, lp);
                addConstraintsForMachines(solution, lp);
                lp.loadMatrix();
            }

    private:
        //adding varables
        template <typename Solution, typename LP>
            void addVariables(Solution & solution, LP &lp) {
                auto & colIdx = solution.getColIdx();
                colIdx.reserve(solution.getMachinesCnt() * solution.getJobsCnt());
                for(typename Solution::JobRef j : utils::make_range(solution.getJobs())) {
                    for(typename Solution::MachineRef m : utils::make_range(solution.getMachines())) {
                        colIdx.push_back(lp.addColumn(solution.getCost()(j,m)));
                    }
                }
            }

        //constraints for job
        template <typename Solution, typename LP>
            void addConstraintsForJobs(Solution & solution, LP & lp) {
                auto & colIdx = solution.getColIdx();
                for(int jIdx : boost::irange(0, solution.getJobsCnt())) {
                    RowId rowIdx = lp.addRow(FX, 1.0, 1.0);
                    for(int mIdx : boost::irange(0, solution.getMachinesCnt())) {
                        lp.addConstraintCoef(rowIdx, colIdx[solution.idx(jIdx,mIdx)]);
                        ++mIdx;
                    }
                }
            }

        //constraints for machines
        template <typename Solution, typename LP>
            void addConstraintsForMachines(Solution & solution, LP & lp)  {
                auto & colIdx = solution.getColIdx();
                int mIdx(0);
                for(typename Solution::MachineRef m : utils::make_range(solution.getMachines())) {
                    auto T = solution.getMachineAvailableTime()(m);
                    RowId rowIdx = lp.addRow(UP, 0.0, T);
                    solution.getMachineRows().insert(rowIdx);
                    int jIdx(0);

                    for(typename Solution::JobRef j : utils::make_range(solution.getJobs())) {
                        auto t = solution.getProceedingTime()(j, m);
                        assert(t <= T);
                        lp.addConstraintCoef(rowIdx, colIdx[solution.idx(jIdx, mIdx)], t);
                        ++jIdx;
                    }
                    ++mIdx;
                }
            }
};

template <typename SolveLPToExtremePoint = DefaultSolveLPToExtremePoint, 
         typename RoundCondition = DefaultRoundCondition, 
         typename RelaxContition = GARelaxCondition, 
         typename Init = GAInit,
         typename SetSolution = GASetSolution>
             using  GeneralAssignmentIRComponents = IRComponents<SolveLPToExtremePoint, RoundCondition, RelaxContition, Init, SetSolution>;


template <typename MachineIter, typename JobIter, typename Cost, typename ProceedingTime, typename MachineAvailableTime, typename JobsToMachines>
    class GeneralAssignment {
        public:
            typedef typename utils::IterToElem<JobIter>::type Job;
            typedef typename utils::IterToElem<MachineIter>::type Machine;

            typedef typename std::iterator_traits<JobIter>::reference JobRef;
            typedef typename std::iterator_traits<MachineIter>::reference MachineRef;
            typedef utils::Compare<double> Compare;
            typedef std::set<RowId> MachineRows;
            typedef std::vector<ColId> ColIdx;

            GeneralAssignment(MachineIter mbegin, MachineIter mend, 
                    JobIter jbegin, JobIter jend,
                    const Cost & c, const ProceedingTime & t, const  MachineAvailableTime & T, JobsToMachines & jobToMachines) : 
                m_mCnt(std::distance(mbegin, mend)), m_jCnt(std::distance(jbegin, jend)),
                m_jbegin(jbegin), m_jend(jend), m_mbegin(mbegin), m_mend(mend),
                m_c(c), m_t(t), m_T(T), m_jobToMachine(jobToMachines) {}

            int idx(int jIdx, int mIdx) {
                return jIdx * m_mCnt + mIdx;
            }

            int getJIdx(int idx) {
                return idx / m_mCnt;
            }

            int getMIdx(int idx) {
                return idx % m_mCnt;
            }

            MachineRows & getMachineRows() {
                return m_machineRows;
            }
            
            Compare getCompare() {
                return m_compare;
            }

            int getMachinesCnt() const {
                return m_mCnt;
            }
            
            int getJobsCnt() const {
                return m_jCnt;
            }

            std::pair<MachineIter, MachineIter> getMachines() {
                return std::make_pair(m_mbegin, m_mend);
            }

            std::pair<JobIter, JobIter> getJobs() {
                return std::make_pair(m_jbegin, m_jend);
            }

            ColIdx & getColIdx() {
                return m_colIdx;
            }

            JobsToMachines & getJobToMachines() {
                return m_jobToMachine;
            }

            const ProceedingTime & getProceedingTime() {
                return m_t;
            }
            
            const MachineAvailableTime & getMachineAvailableTime() {
                return m_T;
            }

            const Cost & getCost() const {
                return m_c;
            }

        private:

            const int m_mCnt;
            const int m_jCnt;
            JobIter m_jbegin;
            JobIter m_jend;
            MachineIter m_mbegin;
            MachineIter m_mend;
            const Cost & m_c; 
            const ProceedingTime & m_t; 
            const MachineAvailableTime & m_T; 
            JobsToMachines & m_jobToMachine;
            const Compare m_compare;
            ColIdx m_colIdx;
            std::set<RowId> m_machineRows;
    };

template <typename MachineIter, typename JobIter, typename Cost, typename ProceedingTime, typename MachineAvailableTime, typename JobsToMachines>
    GeneralAssignment<MachineIter, JobIter, Cost, ProceedingTime, MachineAvailableTime, JobsToMachines>
    make_GeneralAssignment(MachineIter mbegin, MachineIter mend, 
            JobIter jbegin, JobIter jend,
            const Cost & c, const  ProceedingTime & t, const  MachineAvailableTime & T, JobsToMachines & jobsToMachines) {
        return  GeneralAssignment<MachineIter, JobIter, Cost, ProceedingTime, MachineAvailableTime, JobsToMachines>(
                mbegin, mend, jbegin, jend, c, t, T, jobsToMachines);
    }

template <typename MachineIter, typename JobIter, typename Cost, 
          typename ProceedingTime, typename MachineAvailableTime, 
          typename JobsToMachines, typename Components, typename Visitor = TrivialVisitor>
void generalised_assignment_iterative_rounding(MachineIter mbegin, MachineIter mend, 
                    JobIter jbegin, JobIter jend,
                    const Cost & c, const ProceedingTime & t, const  MachineAvailableTime & T, 
                    JobsToMachines & jobToMachines, Components comps, Visitor vis = Visitor()) {
    auto gaSolution = make_GeneralAssignment(
            mbegin, mend, jbegin, jend, 
            c, t, T, jobToMachines);
    solve_iterative_rounding(gaSolution, std::move(comps), std::move(vis));
}


} //ir
} //paal
#endif /* GENERALISED_ASSIGNMENT_HPP */
