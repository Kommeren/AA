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

namespace paal {
namespace local_search {

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
    GainCutSmallImproves() = default; 
    GainCutSmallImproves(GainCutSmallImproves && ) = default; 
    GainCutSmallImproves(const GainCutSmallImproves & ) = default; 
    GainCutSmallImproves & operator=(GainCutSmallImproves && ) = default; 
    GainCutSmallImproves & operator=(const GainCutSmallImproves & ) = default; 

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
    bool operator()(Args&&... ) {
        return m_cnt++ >= m_limit;
    }
private:
    unsigned m_cnt;
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
class ComputeGainWrapper {
public:
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



} // local_search
} // paal

#endif /* CUSTOM_COMPONENTS_HPP */
