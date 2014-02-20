/**
 * @file floating.hpp
 * @brief
 * @author Maciej Andrejczuk, Piotr Wygocki
 * @version 1.0
 * @date 2013-09-01
 */
#ifndef UTILS_RANDOM_HPP
#define UTILS_RANDOM_HPP

namespace paal {
namespace utils {

/**
 * Selects one element according to probability.
 */
template <bool FailIfEmpty = true, typename InputIterator>
static InputIterator randomSelect(InputIterator iBegin, InputIterator iEnd) {
    typedef typename std::iterator_traits<InputIterator>::value_type Dist;
    Dist total = std::accumulate(iBegin, iEnd, Dist());
    Dist r = total * (rand() / (double(RAND_MAX) + 1));
    total = Dist();
    InputIterator selected = iBegin;
    while (selected != iEnd) {
        if (total <= r && total + *selected > r) {
            break;
        }
        total += *selected;
        selected++;
    }
    assert(selected != iEnd || !FailIfEmpty);
    return selected;
}


///Class for comparing floating point
template <typename T>
class Compare {
public:
    ///constructor
    Compare(T epsilon = std::numeric_limits<T>::epsilon()): m_epsilon(epsilon) {}

    /**
     * @brief equals
     *
     * @param a
     * @param b
     *
     * @return
     */
    bool e(T a, T b) const {
        return std::abs(a - b) < m_epsilon;
        //return abs(a -b ) < m_epsilon; //this line breaks generalised_assignment_long_test TODO investigate
    }

    /**
     * @brief greater
     *
     * @param a
     * @param b
     *
     * @return
     */
    bool g(T a, T b) const {
        return a > b + m_epsilon;
    }

    /**
     * @brief greater equals
     *
     * @param a
     * @param b
     *
     * @return
     */
    bool ge(T a, T b) const {
        return a >= b - m_epsilon;
    }

    /**
     * @brief less equals
     *
     * @param a
     * @param b
     *
     * @return
     */
    bool le(T a, T b) const {
        return a <= b + m_epsilon;
    }

    /**
     * @brief getEpsilon used in comparison
     *
     * @return
     */
    double getEpsilon() const {
        return m_epsilon;
    }

    /**
     * @brief returns default epsilon equals the smallest possible difference on doubles
     *
     * @return
     */
    static T defaultEpsilon() {
        return std::numeric_limits<T>::epsilon();
    }

private:
    const T m_epsilon;
};

} //utils
} //paal

#endif // UTILS_RANDOM_HPP
