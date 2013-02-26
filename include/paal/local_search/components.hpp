/**
 * @file components.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-26
 */
#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP 

template <typename Gain, typename Dist>
class GainCutSmallImproves {
public:    
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
#endif /* COMPONENTS_HPP */
