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

/**
 * Relax Condition of the IR Generalised Assignment algorithm.
 */
struct GARelaxCondition {
    /**
     * Checks if a given row of the LP corresponds to a machine and can be relaxed.
     */
    template <typename Problem, typename LP>
    bool operator()(Problem & problem, const LP & lp, lp::RowId row) {
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

/**
 * Set Solution component of the IR Generalised Assignment algorithm.
 */
struct GASetSolution {
    /**
     * Creates the result assignment form the LP (all edges with value 1).
     */
    template <typename Problem, typename Solution>
    void operator()(Problem & problem, const Solution & solution) {
        auto jbegin = problem.getJobs().first;
        auto mbegin = problem.getMachines().first;
        auto & colIdx = problem.getColIdx();
        auto jobToMachine = problem.getJobToMachines();

        for(int idx : boost::irange(0, int(colIdx.size()))) {
            if(problem.getCompare().e(solution(colIdx[idx]), 1)) {
                *jobToMachine = std::make_pair(*(jbegin + problem.getJIdx(idx)),
                                                  *(mbegin + problem.getMIdx(idx)));
                ++jobToMachine;
            }
        }
    }
};

/**
 * Initialization of the IR Generalised Assignment algorithm.
 */
class GAInit {
    public:
        /**
         * Initializes the LP: variables for edges, constraints for jobs and machines.
         */
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
        /**
         * Adds a variable to the LP for each (machine, job) edge, unless the
         * job proceeding time is greater than machine available time. Binds the
         * LP columns to the (machine, job) pairs.
         */
        template <typename Problem, typename LP>
        void addVariables(Problem & problem, LP & lp) {
            auto & colIdx = problem.getColIdx();
            colIdx.reserve(problem.getMachinesCnt() * problem.getJobsCnt());
            for(typename Problem::JobRef j : boost::make_iterator_range(problem.getJobs())) {
                for(typename Problem::MachineRef m : boost::make_iterator_range(problem.getMachines())) {
                    if (problem.getProceedingTime()(j, m) <= problem.getMachineAvailableTime()(m)) {
                        colIdx.push_back(lp.addColumn(problem.getCost()(j,m)));
                    }
                    else {
                        colIdx.push_back(lp.addColumn(problem.getCost()(j,m), lp::FX, 0, 0));
                    }
                }
            }
        }

        //constraints for job
        template <typename Problem, typename LP>
        void addConstraintsForJobs(Problem & problem, LP & lp) {
            auto & colIdx = problem.getColIdx();
            for(int jIdx : boost::irange(0, problem.getJobsCnt())) {
                lp::RowId rowIdx = lp.addRow(lp::FX, 1.0, 1.0);
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
                lp::RowId rowIdx = lp.addRow(lp::UP, 0.0, T);
                problem.getMachineRows().insert(rowIdx);
                int jIdx(0);

                for(typename Problem::JobRef j : boost::make_iterator_range(problem.getJobs())) {
                    auto t = problem.getProceedingTime()(j, m);
                    lp.addConstraintCoef(rowIdx, colIdx[problem.idx(jIdx, mIdx)], t);
                    ++jIdx;
                }
                ++mIdx;
            }
        }
};

template <typename SolveLPToExtremePoint = DefaultSolveLPToExtremePoint,
         typename ResolveLPToExtremePoint = DefaultResolveLPToExtremePoint,
         typename RoundCondition = DefaultRoundCondition,
         typename RelaxContition = GARelaxCondition,
         typename Init = GAInit,
         typename SetSolution = GASetSolution>
             using  GeneralAssignmentIRComponents = IRComponents<SolveLPToExtremePoint,
                ResolveLPToExtremePoint, RoundCondition, RelaxContition, Init, SetSolution>;


/**
 * @class GeneralisedAssignment
 * @brief The class for solving the Generalised Assignment problem using Iterative Rounding.
 *
 * @tparam MachineIter
 * @tparam JobIter
 * @tparam Cost
 * @tparam ProceedingTime
 * @tparam MachineAvailableTime
 * @tparam JobsToMachinesOutputIterator
 */
template <typename MachineIter, typename JobIter, typename Cost,
          typename ProceedingTime, typename MachineAvailableTime,
          typename JobsToMachinesOutputIterator>
class GeneralisedAssignment {
    public:
        typedef typename std::iterator_traits<JobIter>::value_type Job;
        typedef typename std::iterator_traits<MachineIter>::value_type Machine;

        typedef typename std::iterator_traits<JobIter>::reference JobRef;
        typedef typename std::iterator_traits<MachineIter>::reference MachineRef;
        typedef utils::Compare<double> Compare;
        typedef std::set<lp::RowId> MachineRows;
        typedef std::vector<lp::ColId> ColIdx;

        /**
         * Constructor.
         */
        GeneralisedAssignment(MachineIter mbegin, MachineIter mend,
                JobIter jbegin, JobIter jend,
                const Cost & c, const ProceedingTime & t, const MachineAvailableTime & T,
                JobsToMachinesOutputIterator jobToMachines) :
            m_mCnt(std::distance(mbegin, mend)), m_jCnt(std::distance(jbegin, jend)),
            m_jbegin(jbegin), m_jend(jend), m_mbegin(mbegin), m_mend(mend),
            m_c(c), m_t(t), m_T(T), m_jobToMachine(jobToMachines) {}

        typedef boost::optional<std::string> ErrorMessage;

        /**
         * Checks if input is valid.
         */
        ErrorMessage checkInputValidity() {
            return ErrorMessage();
        }

        /**
         * Returns the index of the edge between a given job and a given machine.
         */
        int idx(int jIdx, int mIdx) {
            return jIdx * m_mCnt + mIdx;
        }

        /**
         * Returns the index of a job given the index of the edge between the job and a machine.
         */
        int getJIdx(int idx) {
            return idx / m_mCnt;
        }

        /**
         * Returns the index of a machine given the index of the edge between a job and the machine.
         */
        int getMIdx(int idx) {
            return idx % m_mCnt;
        }

        /**
         * Returns the LP rows corresponding to the machines.
         */
        MachineRows & getMachineRows() {
            return m_machineRows;
        }

        /**
         * Returns the double comparison object.
         */
        Compare getCompare() {
            return m_compare;
        }

        /**
         * Returns the number of machines in the problem.
         */
        int getMachinesCnt() const {
            return m_mCnt;
        }

        /**
         * Returns the number of jobs in the problem.
         */
        int getJobsCnt() const {
            return m_jCnt;
        }

        /**
         * Returns the machines iterator range.
         */
        std::pair<MachineIter, MachineIter> getMachines() {
            return std::make_pair(m_mbegin, m_mend);
        }

        /**
         * Returns the jobs iterator range.
         */
        std::pair<JobIter, JobIter> getJobs() {
            return std::make_pair(m_jbegin, m_jend);
        }

        /**
         * Returns the vector of LP column IDs.
         */
        ColIdx & getColIdx() {
            return m_colIdx;
        }

        /**
         * Returns the result output iterator.
         */
        JobsToMachinesOutputIterator getJobToMachines() {
            return m_jobToMachine;
        }

        /**
         * Returns the proceeding time function (function from (job, machine)
         * pairs into the proceeding time of the job on the machine).
         */
        const ProceedingTime & getProceedingTime() {
            return m_t;
        }

        /**
         * Returns the machine available time function (function returning
         * the time available on a given machine).
         */
        const MachineAvailableTime & getMachineAvailableTime() {
            return m_T;
        }

        /**
         * Returns the cost function (function from (job, machine)
         * pairs into the cost of executing the job on the machine).
         */
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
        JobsToMachinesOutputIterator m_jobToMachine;
        const Compare m_compare;
        ColIdx m_colIdx;
        std::set<lp::RowId> m_machineRows;
};

/**
 * @brief Creates a GeneralisedAssignment object.
 *
 * @tparam MachineIter
 * @tparam JobIter
 * @tparam Cost
 * @tparam ProceedingTime
 * @tparam MachineAvailableTime
 * @tparam JobsToMachinesOutputIterator
 * @param mbegin begin machines iterator
 * @param mend end machines iterator
 * @param jbegin begin jobs iterator
 * @param jend end jobs iterator
 * @param c costs of assignments
 * @param t jobs proceeding times
 * @param T times available for the machines
 * @param jobsToMachines found assignment
 *
 * @return GeneralisedAssignment object
 */
template <typename MachineIter, typename JobIter, typename Cost,
          typename ProceedingTime, typename MachineAvailableTime,
          typename JobsToMachinesOutputIterator>
GeneralisedAssignment<MachineIter, JobIter, Cost, ProceedingTime,
                  MachineAvailableTime, JobsToMachinesOutputIterator>
make_GeneralisedAssignment(MachineIter mbegin, MachineIter mend,
            JobIter jbegin, JobIter jend,
            const Cost & c, const  ProceedingTime & t, const  MachineAvailableTime & T,
            JobsToMachinesOutputIterator jobsToMachines) {
    return GeneralisedAssignment<MachineIter, JobIter, Cost, ProceedingTime,
                        MachineAvailableTime, JobsToMachinesOutputIterator>(
            mbegin, mend, jbegin, jend, c, t, T, jobsToMachines);
}

/**
 * @brief Solves the Generalised Assignment problem using Iterative Rounding.
 *
 * @tparam MachineIter
 * @tparam JobIter
 * @tparam Cost
 * @tparam ProceedingTime
 * @tparam MachineAvailableTime
 * @tparam JobsToMachinesOutputIterator
 * @tparam Components
 * @tparam Visitor
 * @param mbegin begin machines iterator
 * @param mend end machines iterator
 * @param jbegin begin jobs iterator
 * @param jend end jobs iterator
 * @param c costs of assignments
 * @param t jobs proceeding times
 * @param T times available for the machines
 * @param jobsToMachines found assignment
 * @param components IR components
 * @param visitor
 *
 * @return solution status
 */
template <typename MachineIter, typename JobIter, typename Cost,
          typename ProceedingTime, typename MachineAvailableTime,
          typename JobsToMachinesOutputIterator, typename Components, typename Visitor = TrivialVisitor>
IRResult generalised_assignment_iterative_rounding(MachineIter mbegin, MachineIter mend,
                    JobIter jbegin, JobIter jend,
                    const Cost & c, const ProceedingTime & t, const  MachineAvailableTime & T,
                    JobsToMachinesOutputIterator jobToMachines, Components components,
                    Visitor visitor = Visitor()) {
    auto gaSolution = make_GeneralisedAssignment(
            mbegin, mend, jbegin, jend,
            c, t, T, jobToMachines);
    return solve_iterative_rounding(gaSolution, std::move(components), std::move(visitor));
}


} //ir
} //paal
#endif /* GENERALISED_ASSIGNMENT_HPP */
