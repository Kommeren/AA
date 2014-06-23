/**
 * @file parse_file.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-12-12
 */
#ifndef PARSE_FILE_HPP
#define PARSE_FILE_HPP

#include <fstream>
#include <cassert>
#include <limits>

namespace paal {
template <typename Functor> void parse(const std::string &filename, Functor f) {
    std::ifstream is_test_cases(filename);
    assert(is_test_cases.good());
    while (is_test_cases.good()) {
        std::string line;
        is_test_cases >> line;
        if (line == "") {
            return;
        }
        if (line[0] == '#') {
            is_test_cases.ignore(std::numeric_limits<std::streamsize>::max(),
                                 '\n');
            continue;
        }
        f(line, is_test_cases);
    }
}

} //! paal
#endif /* PARSE_FILE_HPP */
