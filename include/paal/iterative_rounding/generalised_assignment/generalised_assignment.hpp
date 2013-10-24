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
    template <typename Problem, typename LP>
        bool operator()(Problem & problem, const LP & lp, RowId row) {
            auto & machineRows = problem.getMachineRows();
            return machineRows.find(row) != machineRows.end() && 
                (
                 lp.getRowDegree(row) <= 1 ||
                 (
                  lp.getRowDegree(row) == 2 &&
                  problem.getCompare().ge(lp.getRowSum(row), 1)
                 )
                );
        }
};

struct GASetSolution {
    template <typename Problem, typename Solution>
        void operator()(Problem & problem, const Solution & solution) {
            auto jbegin = problem.getJobs().first;
            auto mbegin = problem.getMachines().first;
            auto & colIdx = problem.getColIdx();
            auto & jobToMachine = problem.getJobToMachines();

            for(int idx : boost::irange(0, int(colIdx.size()))) { 
                if(problem.getCompare().e(solution(colIdx[idx]), 1)) {
                    jobToMachine.insert(std::make_pair(*(jbegin + problem.getJIdx(idx)), 
                                                         *(mbegin + problem.getMIdx(idx))));
                }
            }
        }
};

class GAInit {
    public:
        template <typename Problem, typename LP>
            void operator()(Problem & problem, LP & lp) {
                lp.setLPName("generalized assignment problem");
                lp.setMinObjFun(); 

                addVariables(problem, lp);
                addConstraintsForJobs(problem, lp);
                addConstraintsForMachines(problem, lp);
                lp.loadMatrix();
            }

    private:
        //adding varables
        template <typename Problem, typename LP>
            void addVariables(Problem & problem, LP &lp) {
                auto & colIdx = problem.getColIdx();
                colIdx.reserve(problem.getMachinesCnt() * problem.getJobsCnt());
                for(typename Problem::JobRef j : boost::make_iterator_range(problem.getJobs())) {
                    for(typename Problem::MachineRef m : boost::make_iterator_range(problem.getMachines())) {
                        colIdx.push_back(lp.addColumn(problem.getCost()(j,m)));
                    }
                }
            }

        //constraints for job
        template <typename Problem, typename LP>
            void addConstraintsForJobs(Problem & problem, LP & lp) {
                auto & colIdx = problem.getColIdx();
                for(int jIdx : boost::irange(0, problem.getJobsCnt())) {
                    RowId rowIdx = lp.addRow(FX, 1.0, 1.0);
                    for(int mIdx : boost::irange(0, problem.getMachinesCnt())) {
                        lp.addConstraintCoef(rowIdx, colIdx[problem.idx(jIdx,mIdx)]);
                        ++mIdx;
                    }
                }
            }

        //constraints for machines
        template <typename Problem, typename LP>
            void addConstraintsForMachines(Problem & problem, LP & lp)  {
                auto & colIdx = problem.getColIdx();
                int mIdx(0);
                for(typename Problem::MachineRef m : boost::make_iterator_range(problem.getMachines())) {
                    auto T = problem.getMachineAvailableTime()(m);
                    RowId rowIdx = lp.addRow(UP, 0.0, T);
                    problem.getMachineRows().insert(rowIdx);
                    int jIdx(0);

                    for(typename Problem::JobRef j : boost::make_iterator_range(problem.getJobs())) {
                        auto t = problem.getProceedingTime()(j, m);
                        assert(t <= T);
                        lp.addConstraintCoef(rowIdx, colIdx[problem.idx(jIdx, mIdx)], t);
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
            typedef typename std::iterator_traits<JobIter>::value_type Job;
            typedef typename std::iterator_traits<MachineIter>::value_type Machine;

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
