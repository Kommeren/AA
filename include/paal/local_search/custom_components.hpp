/**
 * @file custom_components.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-26
 */
#ifndef CUSTOM_COMPONENTS_HPP
#define CUSTOM_COMPONENTS_HPP 

namespace paal {
namespace local_search {

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

private:
    Gain m_gain;
    ValueType & m_val;
};



} // local_search
} // paal

#endif /* CUSTOM_COMPONENTS_HPP */
