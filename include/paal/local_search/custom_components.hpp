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
 * @brief This is the gain adapter which accepts gains improving the current solution by more than epsilon
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

    GainCutSmallImproves(Gain ic, ValueType currOpt, double epsilon) :
                m_improveChecker(std::move(ic)), m_currOpt(currOpt), m_epsilon(epsilon)  {}
    template <typename... Args> ValueType 
        operator()(Args&&... args) { 
        ValueType dist = m_improveChecker(std::forward<Args>(args)...);
        if(dist > m_epsilon * m_currOpt) {
            m_currOpt -= dist;
            return dist;
        }
        return 0;
    }

    void setEpsilon(double e) {
        m_epsilon = e;
    }
    
    void setCurrentOpt(ValueType opt) {
        m_currOpt = opt;
    }

private:
    Gain m_improveChecker;
    ValueType m_currOpt;    
    double m_epsilon;    
};


/**
 * @brief This is custom StopCondition, it returns true after given count limit
 */
class StopConditionCountLimit {
public:
    StopConditionCountLimit(unsigned limit) : m_cnt(0), m_limit(limit) {}

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
    StopConditionTimeLimit(duration d) :  m_duration(d), m_start(clock::now()) {}

    template <typename... Args> 
    bool operator()(Args&&... ) {
        return m_start + m_duration < clock::now() ;
    }

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
    ComputeGainWrapper(ComputeGainWrapper gain, ValueType & val) : m_gain(gain), m_val(val) {}

    template <typename... Args> 
    bool operator()(Args&&... args) {
        auto diff = m_gain(std::forward<Args>(args)...);
        m_val += diff;
        return diff;
    }

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
