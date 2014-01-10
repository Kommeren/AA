/**
 * @file custom_components.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-26
 */
#ifndef CUSTOM_COMPONENTS_HPP
#define CUSTOM_COMPONENTS_HPP 

#include <chrono>

#include "paal/utils/functors.hpp"

namespace paal {
namespace local_search {

    /**
     * @brief if the  condition is not fulfilled this gain adaptor returns 0
     *
     * @tparam Gain
     * @tparam Condition
     */
template <typename Gain = utils::ReturnOneFunctor, typename Condition = utils::ReturnTrueFunctor>
struct ConditionalGainAdaptor {

    /**
     * @brief constructor
     *
     * @param gain
     * @param cond
     */
    ConditionalGainAdaptor(Gain gain = Gain(), Condition cond = Condition()) :
        m_gain(std::move(gain)), m_condition(std::move(cond))
    { }
    
    /**
     * @brief 
     *
     * @tparam Args to be forwarded
     *
     * @return 
     */
    template <typename... Args> 
    auto operator()(Args&&... args) -> decltype(std::declval<Gain>()(std::forward<Args>(args)...)) {
        if(!m_condition(std::forward<Args>(args)...)) {
            return 0;
        }
        return m_gain(std::forward<Args>(args)...);
    }

private:
    Gain m_gain;
    Condition m_condition;
};

/**
 * @brief make for ConditionalGainAdaptor
 *
 * @tparam Gain
 * @tparam Condition
 * @param gain
 * @param cond
 *
 * @return 
 */
template <typename Gain = utils::ReturnOneFunctor, typename Condition = utils::ReturnTrueFunctor>
ConditionalGainAdaptor<Gain, Condition>
make_ConditionalGainAdaptor(Gain gain = Gain(), Condition cond = Condition()) {
    return ConditionalGainAdaptor<Gain, Condition>(std::move(gain), std::move(cond));
}

/**
 * @brief This is the gain adapter which accepts gains improving the current solution by more than epsilon.
 *        This adapter should be used only when ChooseFirstBetter strategy is applied.
 *
 * @tparam Gain
 * @tparam ValueType
 */
template <typename Gain, typename ValueType>
class GainCutSmallImproves {
public:    
    /**
     * @brief Constructor, 
     *
     * @param gain - original gain functor
     * @param currOpt - current optimum
     * @param epsilon - gain limit, gains smaller than epsilon * currOpt are cut
     */
    GainCutSmallImproves(Gain gain, ValueType currOpt, double epsilon) :
                m_gain(std::move(gain)), m_currOpt(currOpt), m_epsilon(epsilon)  {}

    /**
     * @brief transfers arguments to original gain, if the value is to small it is changed to 0. 
     *
     * @tparam Args
     * @param args
     *
     * @return 
     */
    template <typename... Args> ValueType 
        operator()(Args&&... args) { 
        ValueType dist = m_gain(std::forward<Args>(args)...);
        if(dist > m_epsilon * m_currOpt) {
            m_currOpt -= dist;
            return dist;
        }
        return 0;
    }

    /**
     * @brief sets epsilon
     *
     * @param e
     */
    void setEpsilon(double e) {
        m_epsilon = e;
    }
    
    /**
     * @brief sets current Opt
     *
     * @param opt
     */
    void setCurrentOpt(ValueType opt) {
        m_currOpt = opt;
    }

private:
    Gain m_gain;
    ValueType m_currOpt;    
    double m_epsilon;    
};


/**
 * @brief This is custom StopCondition (it can be used in GlobalStopCondition and StepStopCondition) , it returns true after given count limit
 */
class StopConditionCountLimit {
public:
    /**
     * @brief Constructor
     *
     * @param limit given count limit
     */
    StopConditionCountLimit(unsigned limit) : m_cnt(0), m_limit(limit) {}

    /**
     * @brief increment the counter and checks if the given limit is reached. 
     *
     * @tparam Args
     *
     * @return 
     */
    template <typename... Args> 
    bool operator()(Args&&... ) const {
        return m_cnt++ >= m_limit;
    }
private:
    mutable unsigned m_cnt;
    const unsigned m_limit;

};

/**
 * @brief This is custom StopCondition, it returns true after given time limit
 */
template <typename duration = std::chrono::duration<int, std::chrono::seconds>, 
          typename clock    = std::chrono::system_clock>
class StopConditionTimeLimit {
public:
    /**
     * @brief Constructor
     *
     * @param d - time to wait
     */
    StopConditionTimeLimit(duration d) :  m_duration(d), m_start(clock::now()) {}

    /**
     * @brief Checks if the time is up 
     *
     * @tparam Args
     * @param ...
     *
     * @return true if the time is up
     */
    template <typename... Args> 
    bool operator()(Args&&... ) {
        return m_start + m_duration < clock::now() ;
    }

    /**
     * @brief resets the start point
     */
    void restart() {
        m_start = clock::now();
    }
private:
    typename clock::time_point m_start;
    const duration m_duration;
};


/**
 * @brief This wrapper counts sum of the improvements. 
 * It makes sense to use it only when ChooseFirstBetter strategy is applied.
 *          
 *
 * @tparam Gain
 * @tparam ValueType
 */
template <typename Gain, typename ValueType>
struct ComputeGainWrapper {
    /**
     * @brief Constructor
     *
     * @param gain
     * @param val
     */
    ComputeGainWrapper(ComputeGainWrapper gain, ValueType & val) : m_gain(gain), m_val(val) {}

    /**
     * @brief forwards args to original gain. Sum up the improvements.
     *
     * @tparam Args
     * @param args
     *
     * @return 
     */
    template <typename... Args> 
    bool operator()(Args&&... args) {
        auto diff = m_gain(std::forward<Args>(args)...);
        m_val += diff;
        return diff;
    }

    /**
     * @brief Returns sum of the improvements
     *
     * @return 
     */
    ValueType getVal() const {
        return m_val;
    }

private:
    Gain m_gain;
    ValueType & m_val;
};

/**
 * @brief Adapts gain to implement tabu search
 *
 * @tparam TabuList
 * @tparam Gain
 * @tparam AspirationCriteria
 */
template <typename TabuList, 
          typename Gain = utils::ReturnOneFunctor, 
          typename AspirationCriteria = utils::ReturnTrueFunctor>
struct TabuGainAdaptor {

    /**
     * @brief constructor
     *
     * @param tabuList
     * @param gain
     * @param aspirationCriteria 
     */
    TabuGainAdaptor(TabuList tabuList = TabuList(), Gain gain = Gain(), 
                    AspirationCriteria aspirationCriteria 
                        = AspirationCriteria()) :
       m_tabuList(std::move(tabuList)), m_aspirationCriteriaGain(std::move(gain), std::move(aspirationCriteria))
    { }

    /**
     * @brief operator()
     *
     * @tparam Args args to be forwarded
     *
     * @return 
     */
    template <typename... Args> 
    auto operator()(Args&&... args) -> decltype(std::declval<Gain>()(std::forward<Args>(args)...)) {
        if(!m_tabuList.isTabu(std::forward<Args>(args)...)) {
            auto diff = m_aspirationCriteriaGain(std::forward<Args>(args)...);
            if(diff > 0) {
                m_tabuList.accept(std::forward<Args>(args)...);
            }
            return diff;
        }
        return 0;
    }

private:
    TabuList m_tabuList;
    ConditionalGainAdaptor<Gain, AspirationCriteria> m_aspirationCriteriaGain;
};

/**
 * @brief make function for TabuGainAdaptor
 *
 * @tparam TabuList
 * @tparam Gain
 * @tparam AspirationCriteria
 * @param tabuList
 * @param gain
 * @param aspirationCriteria
 *
 * @return 
 */
template <typename TabuList, 
          typename Gain = utils::ReturnTrueFunctor, 
          typename AspirationCriteria = utils::ReturnTrueFunctor>
TabuGainAdaptor<TabuList, Gain, AspirationCriteria>
make_TabuGainAdaptor(TabuList tabuList, Gain gain = Gain(), AspirationCriteria aspirationCriteria = AspirationCriteria()) {
    return TabuGainAdaptor<TabuList, Gain, AspirationCriteria>(std::move(tabuList), std::move(gain), std::move(aspirationCriteria));
}

/**
 * @brief This is adaptor on Commit which allows to record solution basing on condition
 *        It is particularly useful in tabu search  and simulated annealing 
 *        in which we'd like to store the best found solution
 *
 * @tparam Commit
 * @tparam Solution
 * @tparam Condition
 */
template <typename Commit, typename Solution, typename Condition>
struct RecordSolutionCommitAdapter {

    /**
     * @brief constructor
     *
     * @param solution
     * @param commit
     * @param cond
     */
    RecordSolutionCommitAdapter(Solution & solution, 
                                Commit commit = Commit(), 
                                Condition cond = Condition()) : 
        m_solution(&solution), m_commit(std::move(commit)), 
        m_condition(std::move(cond)) { }

    /**
     * @brief operator
     *
     * @tparam Move
     * @param sol
     * @param move
     */
    template <typename Move>
    void operator()(Solution & sol, const Move & move) {
        m_commit(sol, move);
        if(m_condition(sol, *m_solution)) {
            *m_solution = sol;
        }
    }
    
    /**
     * @brief Access to the stored solution (const version)
     *
     * @return 
     */
    const Solution & getSolution() const {
        return *m_solution;
    }
    
    /**
     * @brief Access to the stored solution (non-const version)
     *
     * @return 
     */
    Solution & getSolution() {
        return *m_solution;
    }

private:
    Solution * m_solution;
    Commit m_commit;
    Condition m_condition;
};

/**
 * @brief make function for RecordSolutionCommitAdapter
 *
 * @tparam Commit
 * @tparam Solution
 * @tparam Condition
 * @param s
 * @param commit
 * @param c
 *
 * @return 
 */
template <typename Commit, typename Solution, typename Condition>
RecordSolutionCommitAdapter<Commit, Solution, Condition> 
make_RecordSolutionCommitAdapter(Solution & s, Commit commit, Condition c) {
    return RecordSolutionCommitAdapter<Commit, Solution, Condition>(s, std::move(commit), std::move(c));
}


} // local_search
} // paal

#endif /* CUSTOM_COMPONENTS_HPP */
