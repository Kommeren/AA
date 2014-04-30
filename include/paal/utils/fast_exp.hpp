/**
 * @file fast_exp.hpp
 * @brief idea is NOT mine, found in the web
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-02-19
 */

#ifndef FAST_EXP_HPP
#define FAST_EXP_HPP

#include <cstdint>

namespace paal {

/**
 * @brief fast Power of 2
 *
 * @param p
 *
 * @return
 */
inline float fast_pow2 (float p)
{
    float clipp = (p < -126) ? -126.0f : p;
    union { uint32_t i; float f; } v = { static_cast<uint32_t> (
            (1 << 23) * (clipp + 126.94269504f) ) };
    return v.f;
}

/**
 * @brief fast power of e.
 *
 * @param p
 *
 * @return
 */
inline float fast_exp(float p)
{
    static const float multiplier = 1 / std::log(2);
    return fast_pow2 (multiplier * p);
}
}//!paal

#endif /* FAST_EXP_HPP */
