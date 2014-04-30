/**
 * @file read_ss.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-23
 */

#ifndef READ_SS_HPP
#define READ_SS_HPP
#include "utils/logger.hpp"

#include <vector>
#include <string>
#include <istream>

namespace paal {
namespace{
typedef std::vector<std::string> Words;

Words read_SS(std::istream & ist) {
    int n;
    ist >> n;
    Words m_words;
    while(n--){
        std::string word;
        ist >> word;
        m_words.push_back(std::move(word));
    }
    return m_words;
}
}
}//!paal
#endif /* READ_SS_HPP */
