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
struct ga_relax_condition {
    /**
     * Checks if a given row of the LP corresponds to a machine and can be relaxed.
     */
    template <typename Problem, typename LP>
    bool operator()(Problem & problem, const LP & lp, lp::row_id row) {
        auto && machine_rows = problem.get_machine_rows();
        if (machine_rows.find(row) == machine_rows.end()) {
            return false;
        }
        auto row_deg = lp.get_row_degree(row);
        return  row_deg <= 1 ||
                    (
                     row_deg == 2 &&
                     problem.get_compare().ge(lp.get_row_sum(row), 1)
                    )
            ;
    }
};

/**
 * Set Solution component of the IR Generalised Assignment algorithm.
 */
struct ga_set_solution {
    /**
     * Creates the result assignment form the LP (all edges with value 1).
     */
    template <typename Problem, typename GetSolution>
    void operator()(Problem & problem, const GetSolution & solution) {
        auto jbegin = problem.get_jobs().first;
        auto mbegin = problem.get_machines().first;
        auto & colIdx = problem.get_col_idx();
        auto jobToMachine = problem.get_job_to_machines();

        for(int idx : boost::irange(0, int(colIdx.size()))) {
            if(problem.get_compare().e(solution(colIdx[idx]), 1)) {
                *jobToMachine = std::make_pair(*(jbegin + problem.get_j_idx(idx)),
                                                  *(mbegin + problem.get_m_idx(idx)));
                ++jobToMachine;
            }
        }
    }
};

/**
 * Initialization of the IR Generalised Assignment algorithm.
 */
class ga_init {
    public:
        /**
         * Initializes the LP: variables for edges, constraints for jobs and machines.
         */
        template <typename Problem, typename LP>
        void operator()(Problem & problem, LP & lp) {
            lp.set_lp_name("generalized assignment problem");
            lp.set_optimization_type(lp::MINIMIZE);

            add_variables(problem, lp);
            add_constraints_for_jobs(problem, lp);
            add_constraints_for_machines(problem, lp);
        }

    private:
        /**
         * Adds a variable to the LP for each (machine, job) edge, unless the
         * job proceeding time is greater than machine available time. Binds the
         * LP columns to the (machine, job) pairs.
         */
        template <typename Problem, typename LP>
        void add_variables(Problem & problem, LP & lp) {
            auto & colIdx = problem.get_col_idx();
            colIdx.reserve(problem.get_machines_cnt() * problem.get_jobs_cnt());
            for(auto && j : boost::make_iterator_range(problem.get_jobs())) {
                for(auto && m : boost::make_iterator_range(problem.get_machines())) {
                    if (problem.get_proceeding_time()(j, m) <= problem.get_machine_available_time()(m)) {
                        colIdx.push_back(lp.add_column(problem.get_cost()(j,m)));
                    }
                    else {
                        colIdx.push_back(lp.add_column(problem.get_cost()(j,m), 0, 0));
                    }
                }
            }
        }

        //constraints for job
        template <typename Problem, typename LP>
        void add_constraints_for_jobs(Problem & problem, LP & lp) {
            auto & colIdx = problem.get_col_idx();
            for (int jIdx : boost::irange(0, problem.get_jobs_cnt())) {
                lp::linear_expression expr;
                for (int mIdx : boost::irange(0, problem.get_machines_cnt())) {
                    expr += colIdx[problem.idx(jIdx,mIdx)];
                    ++mIdx;
                }
                lp.add_row(std::move(expr) == 1.0);
            }
        }

        //constraints for machines
        template <typename Problem, typename LP>
        void add_constraints_for_machines(Problem & problem, LP & lp)  {
            auto & colIdx = problem.get_col_idx();
            int mIdx(0);
            for(auto && m : boost::make_iterator_range(problem.get_machines())) {
                auto T = problem.get_machine_available_time()(m);
                int jIdx(0);
                lp::linear_expression expr;

                for(auto && j : boost::make_iterator_range(problem.get_jobs())) {
                    auto t = problem.get_proceeding_time()(j, m);
                    auto x = colIdx[problem.idx(jIdx, mIdx)];
                    expr += x * t;
                    ++jIdx;
                }
                auto row = lp.add_row(std::move(expr) <= T);
                problem.get_machine_rows().insert(row);
                ++mIdx;
            }
        }
};

template <typename Init = ga_init,
         typename RoundCondition = default_round_condition,
         typename RelaxContition = ga_relax_condition,
         typename SetSolution = ga_set_solution>
             using ga_ir_components = IRcomponents<Init, RoundCondition, RelaxContition, SetSolution>;


/**
 * @class generalised_assignment
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
class generalised_assignment {
    public:
        using Job = typename std::iterator_traits<JobIter>::value_type;
        using Machine = typename std::iterator_traits<MachineIter>::value_type;

        using Compare = utils::Compare<double>;
        using MachineRows = std::unordered_set<lp::row_id>;
        using ColIdx = std::vector<lp::col_id>;

        /**
         * Constructor.
         */
        generalised_assignment(MachineIter mbegin, MachineIter mend,
                JobIter jbegin, JobIter jend,
                const Cost & c, const ProceedingTime & t, const MachineAvailableTime & T,
                JobsToMachinesOutputIterator jobToMachines) :
            m_m_cnt(std::distance(mbegin, mend)), m_j_cnt(std::distance(jbegin, jend)),
            m_jbegin(jbegin), m_jend(jend), m_mbegin(mbegin), m_mend(mend),
            m_c(c), m_t(t), m_T(T), m_job_to_machine(jobToMachines) {}

        using ErrorMessage = boost::optional<std::string>;

        /**
         * Checks if input is valid.
         */
        ErrorMessage check_input_validity() {
            return ErrorMessage{};
        }

        /**
         * Returns the index of the edge between a given job and a given machine.
         */
        int idx(int jIdx, int mIdx) {
            return jIdx * m_m_cnt + mIdx;
        }

        /**
         * Returns the index of a job given the index of the edge between the job and a machine.
         */
        int get_j_idx(int idx) {
            return idx / m_m_cnt;
        }

        /**
         * Returns the index of a machine given the index of the edge between a job and the machine.
         */
        int get_m_idx(int idx) {
            return idx % m_m_cnt;
        }

        /**
         * Returns the LP rows corresponding to the machines.
         */
        MachineRows & get_machine_rows() {
            return m_machine_rows;
        }

        /**
         * Returns the double comparison object.
         */
        Compare get_compare() {
            return m_compare;
        }

        /**
         * Returns the number of machines in the problem.
         */
        int get_machines_cnt() const {
            return m_m_cnt;
        }

        /**
         * Returns the number of jobs in the problem.
         */
        int get_jobs_cnt() const {
            return m_j_cnt;
        }

        /**
         * Returns the machines iterator range.
         */
        std::pair<MachineIter, MachineIter> get_machines() {
            return std::make_pair(m_mbegin, m_mend);
        }

        /**
         * Returns the jobs iterator range.
         */
        std::pair<JobIter, JobIter> get_jobs() {
            return std::make_pair(m_jbegin, m_jend);
        }

        /**
         * Returns the vector of LP column IDs.
         */
        ColIdx & get_col_idx() {
            return m_col_idx;
        }

        /**
         * Returns the result output iterator.
         */
        JobsToMachinesOutputIterator get_job_to_machines() {
            return m_job_to_machine;
        }

        /**
         * Returns the proceeding time function (function from (job, machine)
         * pairs into the proceeding time of the job on the machine).
         */
        const ProceedingTime & get_proceeding_time() {
            return m_t;
        }

        /**
         * Returns the machine available time function (function returning
         * the time available on a given machine).
         */
        const MachineAvailableTime & get_machine_available_time() {
            return m_T;
        }

        /**
         * Returns the cost function (function from (job, machine)
         * pairs into the cost of executing the job on the machine).
         */
        const Cost & get_cost() const {
            return m_c;
        }

    private:

        const int m_m_cnt;
        const int m_j_cnt;
        JobIter m_jbegin;
        JobIter m_jend;
        MachineIter m_mbegin;
        MachineIter m_mend;
        const Cost & m_c;
        const ProceedingTime & m_t;
        const MachineAvailableTime & m_T;
        JobsToMachinesOutputIterator m_job_to_machine;
        const Compare m_compare;
        ColIdx m_col_idx;
        MachineRows m_machine_rows;
};

/**
 * @brief Creates a generalised_assignment object.
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
 * @return generalised_assignment object
 */
template <typename MachineIter, typename JobIter, typename Cost,
          typename ProceedingTime, typename MachineAvailableTime,
          typename JobsToMachinesOutputIterator>
generalised_assignment<MachineIter, JobIter, Cost, ProceedingTime,
                  MachineAvailableTime, JobsToMachinesOutputIterator>
make_generalised_assignment(MachineIter mbegin, MachineIter mend,
            JobIter jbegin, JobIter jend,
            const Cost & c, const  ProceedingTime & t, const  MachineAvailableTime & T,
            JobsToMachinesOutputIterator jobsToMachines) {
    return generalised_assignment<MachineIter, JobIter, Cost, ProceedingTime,
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
          typename JobsToMachinesOutputIterator,
          typename Components = ga_ir_components<>,
          typename Visitor = trivial_visitor>
IRResult generalised_assignment_iterative_rounding(MachineIter mbegin, MachineIter mend,
                JobIter jbegin, JobIter jend,
                const Cost & c, const ProceedingTime & t, const  MachineAvailableTime & T,
                JobsToMachinesOutputIterator jobsToMachines,
                Components components = Components(), Visitor visitor = Visitor()) {
    auto gaSolution = make_generalised_assignment(
            mbegin, mend, jbegin, jend,
            c, t, T, jobsToMachines);
    return solve_iterative_rounding(gaSolution, std::move(components), std::move(visitor));
}


} //ir
} //paal
#endif /* GENERALISED_ASSIGNMENT_HPP */
