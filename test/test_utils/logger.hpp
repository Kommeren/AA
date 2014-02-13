//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file logger.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-13
 */
#ifndef PAAL_LOGGER_HPP
#define PAAL_LOGGER_HPP

#ifdef LOGGER_ON
#include <iostream>
#include "paal/utils/type_functions.hpp"

template <typename V, typename W, typename Stream>
Stream &operator<<(Stream &o, const std::pair<V, W> &p) {
    o << "{" << p.first << ", " << p.second << "} ";
}

#define LOGLN(x) std::cout << x << std::endl << std::flush
#define LOG(x) std::cout << x << std::flush
#define ON_LOG(x) x
#define LOG_COPY(b, e)                                                         \
    std::copy(b, e,                                                            \
              std::ostream_iterator<                                           \
                  typename std::iterator_traits<decltype(b)>::value_type>(     \
                  std::cout, "\n"));                                           \
    LOG("")
#define LOG_COPY_DEL(b, e, del)                                                \
    std::copy(b, e,                                                            \
              std::ostream_iterator<                                           \
                  typename std::iterator_traits<decltype(b)>::value_type>(     \
                  std::cout, del));                                            \
    LOG("")
#define LOG_COPY_RANGE_DEL(range, del)                                         \
    LOG_COPY_DEL(std::begin(range), std::end(range), del)
#else
#define LOGLN(x)
#define LOG(x)
#define ON_LOG(x)
#define LOG_COPY(b, e)
#define LOG_COPY_DEL(b, e, del)
#define LOG_COPY_RANGE_DEL(range, del)
#endif

#endif // PAAL_LOGGER_HPP
