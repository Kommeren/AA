/**
 * @file random.hpp
 * @brief 
 * @author Maciej Andrejczuk
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

} //utils
} //paal

#endif // UTILS_RANDOM_HPP
