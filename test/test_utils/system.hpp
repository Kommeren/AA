//=======================================================================
// Copyright (c) 2014 Karol Wegrzycki
//
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

/**
 * @file system.hpp
 * @brief Filesystem interface for testing bins.
 * @author Karol Wegrzycki
 * @version 1.0
 * @date 2014-12-23
 */

#ifndef PAAL_SYSTEM_HPP
#define PAAL_SYSTEM_HPP

#include "test_utils/get_test_dir.hpp"

#include <string>
#include <cstdlib>

namespace paal {
namespace system {

//TODO move functions from test/bin here

std::string temp = get_temp_dir();

/**
 * @brief Function for creating the file with specified text
 *
 * @param file_name of created file
 * @param text of creaated file
 *
 * @return std::string which contains path to the created file.
 */
std::string create_file(std::string file_name, std::string text) {
    // TODO use boost filesystem
    std::system(("echo \"" + text + "\" > " + temp + file_name).c_str());
    return temp + file_name;
}

} //! system

} //! paal
#endif // PAAL_SYSTEM_HPP
