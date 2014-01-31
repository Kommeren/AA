/**
 * @file simulated_annealing.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-01-31
 */
#ifndef SIMULATED_ANNEALING_HPP
#define SIMULATED_ANNEALING_HPP 

#include <chrono>

namespace paal {
namespace local_search {

    /**
     * @brief This functors returns potential (temperature) using the following
     * schema. The start potential equals given startTemperature, the end temperature (after given duration) equals 
     * given endTemperature. In the beetween potential decreases (increase when startTemperature < endTemperature, 
     * which is not typical use case)
     * in exponential manner
     *
     * @tparam Duration
     * @tparam Clock 
     */
template <typename Duration = std::chrono::seconds, 
          typename Clock    = std::chrono::system_clock>
struct ExponentialCoolingSchema {

    /**
     * @brief Constructor 
     *
     * @param duration 
     * @param startTemperature
     * @param endTemperature
     */
    ExponentialCoolingSchema(Duration duration, double startTemperature, double endTemperature) :  
        m_duration(duration), m_start(Clock::now()), 
        m_multiplier(startTemperature), 
        m_base(std::pow(endTemperature/startTemperature, 1. / duration.count())) {}
    
    /**
     * @brief resets the start point
     */
    void restart() {
        m_start = Clock::now();
    }

    /**
     * @brief operator(), return Temperature at given time point point
     *
     * @return 
     */
    double operator()() const {
        return m_multiplier * std::pow(m_base, 
                std::chrono::duration_cast<Duration>(
                    Clock::now() - m_start).count()); 
    }

private:
    Duration m_duration;
    typename Clock::time_point m_start;
    double m_multiplier;
    double m_base;
};

/**
 * @brief make function for ExponentialCoolingSchema
 *
 * @tparam Clock 
 * @tparam Duration
 * @param duration
 * @param startTemperature
 * @param endTemperature
 *
 * @return 
 */
template <typename Clock    = std::chrono::system_clock,
          typename Duration>
ExponentialCoolingSchema<Duration, Clock>
make_ExponentialCoolingSchema(Duration duration, double startTemperature, double endTemperature) {
    return ExponentialCoolingSchema<Duration, Clock>(duration, startTemperature, endTemperature);
}




/**
 * @brief This adaptor takes Gain functor and adopts it to simulated annealing.
 *        For each move, if it has positive gain it is chosen otherwise the move is chosen
 *        wit probability e^(movesGain/Temperature). Temperature is given by getTemperature functor.
 *
 * @tparam Gain
 * @tparam GetTemperature
 * @tparam RandomGenerator
 */
template <typename Gain, typename GetTemperature, typename RandomGenerator = std::default_random_engine>
struct SimulatedAnnealingGainAdaptor {
    /**
     * @brief constructor
     *
     * @param gain
     * @param getTemperature
     * @param rand
     */
    SimulatedAnnealingGainAdaptor(Gain gain, 
            GetTemperature getTemperature, 
            RandomGenerator rand = RandomGenerator()) : 
        m_gain(gain), m_getTemperature(getTemperature), 
        m_rand(rand), m_distribution(0.0, 1.0) {}

    /**
     * @brief This object represents Delta computed by gain and the decision if the object is taken or not.
     *
     * @tparam Delta
     */
    template <typename Delta>
    struct IsChosen {
        /**
         * @brief creates IsChosen for Move  object
         *
         * @param d
         *
         * @return 
         */
        static IsChosen makeChosen(Delta d) {
            return IsChosen(true, d);
        }
        
        /**
         * @brief creates IsChosen fo Move which is not chosen
         *
         * @param d
         *
         * @return 
         */
        static IsChosen makeUnchosen(Delta d) {
            return IsChosen(false, d);
        }
        
        /**
         * @brief constructor (from 0) represents the largest possible value of unchosen element
         *        If some element is bigger than zero, then this element is chosen.
         *
         * @param i given int must be 0
         */
        IsChosen(int i) : 
            m_isChosen(false), 
            m_delta(std::numeric_limits<Delta>::max()) {assert(i == 0);}

        /**
         * @brief operator<. Chosen move is allways biger then unchosen
         *
         * @param other
         *
         * @return 
         */
        bool operator<(IsChosen other) const {
            if(m_isChosen != other.m_isChosen) {
                return m_isChosen < other.isChosen;
            } else {
                return m_delta < other.m_delta;
            }
        }

        /**
         * @brief conversion to delta operator
         *
         * @return 
         */
        operator Delta() {
            return m_delta;
        }

    private:
        /**
         * @brief constructor, use makeChosen and makeUnchosen for construction.
         *
         * @param isChosen
         * @param d
         */
        IsChosen(bool isChosen, Delta d) :
            m_isChosen(isChosen), m_delta(d) {}
        bool m_isChosen;
        Delta m_delta;
    };

    /**
     * @brief operator(), returns original gain with information 
     *                    indicating if the move is chosen
     *
     * @tparam Args
     * @param args
     *
     * @return 
     */
    template <typename... Args>
    auto operator()(Args&&... args) -> 
        IsChosen<typename std::result_of<Gain(Args...)>::type> {
            auto delta =  m_gain(std::forward<Args>(args)...);
            using Delta = decltype(delta);
            if(delta > 0) {
                return IsChosen<Delta>::makeChosen(delta);
            } else {
                if( m_distribution(m_rand) >  exp(double(delta) / m_getTemperature())) {
                    return IsChosen<Delta>::makeChosen(delta);
                } else {
                    return IsChosen<Delta>::makeUnchosen(delta);
                }
            }
    }
    

private:
    Gain m_gain;
    GetTemperature m_getTemperature;
    RandomGenerator m_rand;
    std::uniform_real_distribution<double> m_distribution;
};


/**
 * @brief make function for SimulatedAnnealingGainAdaptor
 *
 * @tparam Gain
 * @tparam GetTemperature
 * @tparam RandomGenerator
 * @param gain
 * @param getTemperature
 * @param rand
 *
 * @return 
 */
template <typename Gain, typename GetTemperature, typename RandomGenerator = std::default_random_engine>
SimulatedAnnealingGainAdaptor<Gain, GetTemperature, RandomGenerator>
make_SimulatedAnnealingGainAdaptor( 
         Gain gain, GetTemperature getTemperature, 
         RandomGenerator rand = RandomGenerator()) {
     return SimulatedAnnealingGainAdaptor
                <Gain, GetTemperature, RandomGenerator>(
                    std::move(gain), std::move(getTemperature), std::move(rand));
}



} //!local_search
} //!paal

#endif /* SIMULATED_ANNEALING_HPP */
