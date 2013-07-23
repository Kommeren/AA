/**
 * @file components.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-26
 */
#ifndef CUSTOM_COMPONENTS_HPP
#define CUSTOM_COMPONENTS_HPP 

namespace paal {
namespace local_search {

template <typename Gain, typename Dist>
class GainCutSmallImproves {
public:    
    GainCutSmallImproves() = default; 
    GainCutSmallImproves(GainCutSmallImproves && ) = default; 
    GainCutSmallImproves(const GainCutSmallImproves & ) = default; 

    GainCutSmallImproves(Gain ic, Dist currOpt, double epsilon) :
                m_improveChecker(std::move(ic)), m_currOpt(currOpt), m_epsilon(epsilon)  {}
    template <typename... Args> Dist 
        operator()(Args&&... args) { 
        Dist dist = m_improveChecker(std::forward<Args>(args)...);
        if(dist > m_epsilon * m_currOpt) {
            m_currOpt -= dist;
            return dist;
        }
        return 0;
    }

    void setEpsilon(double e) {
        m_epsilon = e;
    }
    
    void setCurrentOpt(Dist opt) {
        m_currOpt = opt;
    }

private:
    Gain m_improveChecker;
    Dist m_currOpt;    
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

} // local_search
} // paal

#endif /* CUSTOM_COMPONENTS_HPP */
