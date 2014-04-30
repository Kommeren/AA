/**
 * @file simulated_annealing.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-01-31
 */
#ifndef SIMULATED_ANNEALING_HPP
#define SIMULATED_ANNEALING_HPP


#include "paal/utils/fast_exp.hpp"
#include "paal/local_search/search_traits.hpp"


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
struct exponential_cooling_schema_dependant_on_time {

    /**
     * @brief Constructor
     *
     * @param duration
     * @param startTemperature
     * @param endTemperature
     */
    exponential_cooling_schema_dependant_on_time(Duration duration, double startTemperature, double endTemperature) :
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
 * @brief make function for exponential_cooling_schema_dependant_on_time
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
exponential_cooling_schema_dependant_on_time<Duration, Clock>
make_exponential_cooling_schema_dependant_on_time(Duration duration, double startTemperature, double endTemperature) {
    return exponential_cooling_schema_dependant_on_time<Duration, Clock>(duration, startTemperature, endTemperature);
}


    /**
     * @brief This functors returns potential (temperature) using the following
     * schema. The start potential equals given startTemperature, once per numberOFRoundsWithSameTemperature
     * the temperature is multiplied by given multiplier
     */
struct exponential_cooling_schema_dependant_on_iteration {

    /**
     * @brief Constructor
     *
     * @param startTemperature
     * @param multiplier
     * @param numberOFRoundsWithSameTemperature
     */
    exponential_cooling_schema_dependant_on_iteration(
            double startTemperature, double multiplier, double numberOFRoundsWithSameTemperature = 1) :
        m_temperature(startTemperature),
        m_multiplier(multiplier),
        m_number_of_rounds_with_same_temperature(numberOFRoundsWithSameTemperature) {}


    /**
     * @brief operator(), return Temperature at given time point point
     *
     * @return
     */
    double operator()() {
        if(++m_cnt_from_last_multiply == m_number_of_rounds_with_same_temperature) {
            m_temperature *= m_multiplier;
            m_cnt_from_last_multiply = 0;
        }
        return m_temperature;
    }

private:
    double m_temperature;
    double m_multiplier;
    int    m_number_of_rounds_with_same_temperature;
    int    m_cnt_from_last_multiply = 0;
};


/**
 * @brief This function takes gain functor which is assumed to be probabilistic
 * and dependent on one nonnegative double called temperature.
 * It calculates starting temperature for Simmulated Annealing. Using binary search it looks for temperature that percent of accepted moves is as close to acceptance_rate as possible.
 * This function manipulates temperature using SetTemperature.
 * The gain is assumed to be monotonic in temperature. The 0 temperature means no bad moves are accepted.
 *
 * @tparam Solution
 * @tparam ProbabilisticGain
 * @tparam GetMoves
 * @tparam SetTemperature
 * @param solution
 * @param gain
 * @param get_moves
 * @param set_temperature functor, takes temperature, no return
 * @param acceptance_rate acceptance rate to achive
 * @param repeats_number denotes number of iterations needed to compute success rate for given temperature
 * @param epsilon used to compare doubles.
 *
 * @return
 */
template <typename Solution, typename ProbabilisticGain, typename GetMoves, typename SetTemperature>
double start_temperature(
        Solution & solution,
        ProbabilisticGain gain,
        GetMoves get_moves,
        SetTemperature set_temperature,
        double acceptance_rate = 0.4,
        int repeats_number = 1000,
        double epsilon = 0.0001) {
    assert(acceptance_rate >= 0. && acceptance_rate <= 1.);
    using MoveRef = typename move_type_from_get_moves<GetMoves, Solution>::reference;

    auto get_success_rate = [&](int t) {
        set_temperature(t);
        int number_of_success = 0;
        int total = 0;
        for(int i = 0; i < repeats_number; ++i) {
            for(MoveRef move : get_moves(solution)) {
                ++total;
                if(gain(solution, move) > 0) {
                    ++number_of_success;
                }
            }
        }
        return double(number_of_success) / total;
    };

    //compute lower and upper bound
    double t = 1.;
    double success_rate = get_success_rate(t);
    double t_lower_bound{t}, t_upper_bound{t};
    if(success_rate > acceptance_rate) {
        do{
            t /= 2;
            success_rate = get_success_rate(t);
            if(t < epsilon) {
                return 0.;
            }
        } while(success_rate > acceptance_rate);
        t_lower_bound = t;
        t_upper_bound = 2 * t;
    } else if(success_rate < acceptance_rate) {
        do{
            t *= 2;
            success_rate = get_success_rate(t);
        } while(success_rate < acceptance_rate);
        t_lower_bound = t / 2;
        t_upper_bound = t;
    } else {
        return t;
    }

    //binary search to find appropriate temperature
    while(t_upper_bound - t_lower_bound > epsilon ) {
        t = (t_upper_bound + t_lower_bound) / 2;
        success_rate = get_success_rate(t);
        if(success_rate > acceptance_rate) {
            t_upper_bound = t;
        } else if(success_rate < acceptance_rate) {
            t_lower_bound = t;
        } else {
            return t;
        }
    }

    return (t_upper_bound + t_lower_bound) / 2;
}


/**
 * @brief This adaptor takes Gain functor and adopts it to simulated annealing.
 *        For each move, if it has positive gain it is chosen otherwise the move is chosen
 *        wit probability e^(movesGain/Temperature). Temperature is given by getTemperature functor.
 *
 * @tparam Gain
 * @tparam GetTemperature
 * @tparam random_generator
 */
template <typename Gain, typename GetTemperature, typename random_generator = std::default_random_engine>
struct simulated_annealing_gain_adaptor {
    /**
     * @brief constructor
     *
     * @param gain
     * @param getTemperature
     * @param rand
     */
    simulated_annealing_gain_adaptor(Gain gain,
            GetTemperature getTemperature,
            random_generator rand = random_generator()) :
        m_gain(gain), m_get_temperature(getTemperature),
        m_rand(rand), m_distribution(0.0, 1.0) {}

    /**
     * @brief This object represents Delta computed by gain and the decision if the object is taken or not.
     *
     * @tparam Delta
     */
    template <typename Delta>
    struct is_chosen {
        /**
         * @brief creates is_chosen for Move  object
         *
         * @param d
         *
         * @return
         */
        static is_chosen make_chosen(Delta d) {
            return is_chosen(true, d);
        }

        /**
         * @brief creates is_chosen for Move which is not chosen
         *
         * @param d
         *
         * @return
         */
        static is_chosen make_unchosen(Delta d) {
            return is_chosen(false, d);
        }

        /**
         * @brief constructor (from 0) represents the largest possible value of unchosen element
         *        If some element is bigger than zero, then this element is chosen.
         *
         * @param i given int must be 0
         */
        is_chosen(int i) :
            m_is_chosen(false),
            m_delta(std::numeric_limits<Delta>::max()) {assert(i == 0);}

        /**
         * @brief operator<. Chosen move is always bigger then unchosen
         *
         * @param other
         *
         * @return
         */
        bool operator<(is_chosen other) const {
            if(m_is_chosen != other.m_is_chosen) {
                return m_is_chosen < other.m_is_chosen;
            } else {
                return m_delta < other.m_delta;
            }
        }

        /**
         * @brief operator>
         *
         * @param other
         *
         * @return
         */
        bool operator>(is_chosen other) const {
            if(m_is_chosen != other.m_is_chosen) {
                return m_is_chosen > other.m_is_chosen;
            } else {
                return m_delta > other.m_delta;
            }
        }

        /**
         * @brief operator< (int)
         *
         * @param i
         *
         * @return
         */
        bool operator<(int i) const {
            return *this < is_chosen(i);
        }

        /**
         * @brief operator>(int)
         *
         * @param i
         *
         * @return
         */
        bool operator>(int i) const {
            return *this > is_chosen(i);
        }

        /**
         * @brief operator>(int, is_chosen)
         *
         * @param i
         * @param ich
         *
         * @return
         */
        friend bool operator>(int i, is_chosen ich) {
            return ich < i;
        }

        /**
         * @brief operator<(int, is_chosen)
         *
         * @param i
         * @param ich
         *
         * @return
         */
        friend bool operator<(int i, is_chosen ich) {
            return ich > i;
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
         * @brief constructor, use make_chosen and make_unchosen for construction.
         *
         * @param isChosen
         * @param d
         */
        is_chosen(bool isChosen, Delta d) :
            m_is_chosen(isChosen), m_delta(d) {}
        bool m_is_chosen;
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
        is_chosen<typename std::result_of<Gain(Args...)>::type> {
            auto delta =  m_gain(std::forward<Args>(args)...);
            using Delta = decltype(delta);
            if(delta > 0) {
                return is_chosen<Delta>::make_chosen(delta);
            } else {
                if( m_distribution(m_rand) < fast_exp(double(delta) / m_get_temperature())) {
                    return is_chosen<Delta>::make_chosen(delta);
                } else {
                    return is_chosen<Delta>::make_unchosen(delta);
                }
            }
    }


private:
    Gain m_gain;
    GetTemperature m_get_temperature;
    random_generator m_rand;
    std::uniform_real_distribution<double> m_distribution;
};


/**
 * @brief make function for simulated_annealing_gain_adaptor
 *
 * @tparam Gain
 * @tparam GetTemperature
 * @tparam random_generator
 * @param gain
 * @param getTemperature
 * @param rand
 *
 * @return
 */
template <typename Gain, typename GetTemperature, typename random_generator = std::default_random_engine>
simulated_annealing_gain_adaptor<Gain, GetTemperature, random_generator>
make_simulated_annealing_gain_adaptor(
         Gain gain, GetTemperature getTemperature,
         random_generator rand = random_generator()) {
     return simulated_annealing_gain_adaptor
                <Gain, GetTemperature, random_generator>(
                    std::move(gain), std::move(getTemperature), std::move(rand));
}



} //!local_search
} //!paal

#endif /* SIMULATED_ANNEALING_HPP */
