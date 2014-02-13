//=======================================================================
// Copyright (c) 2103 Piotr Smulewicz
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file read_ss.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-23
 */

#ifndef PAAL_READ_SS_HPP
#define PAAL_READ_SS_HPP
#include "test_utils/logger.hpp"

#include <vector>
#include <string>
#include <istream>

namespace paal {
namespace {
typedef std::vector<std::string> Words;

Words read_SS(std::istream &ist) {
    int n;
    ist >> n;
    Words m_words;
    while (n--) {
        std::string word;
        ist >> word;
        m_words.push_back(std::move(word));
    }
    return m_words;
}
}
}      //!paal
#endif // PAAL_READ_SS_HPP
