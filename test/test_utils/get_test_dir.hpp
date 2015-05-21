//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file get_test_dir.hpp
 * @brief
 * @author Karol Wegrzycki
 * @version 1.0
 * @date 2014-11-25
 */

#ifndef PAAL_GET_TEST_DIR
#define PAAL_GET_TEST_DIR

#define BOOST_ERROR_CODE_HEADER_ONLY
#define BOOST_SYSTEM_NO_DEPRECATED

#include "test_utils/system.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <cassert>
#include <fstream>
#include <string>

namespace paal {
namespace system {

/// Returns directory with test data
inline std::string get_test_data_dir(std::string const &path) {
    return build_path(PROJECT_DIR, "test/data/" + path);
}

/// Returns build dir
inline std::string get_build_dir(std::string const &program_name) {
    return build_path(BUILD_DIR, program_name);
}

/// Returns static temporary directory
inline std::string get_static_temp_dir() {
    using namespace boost::filesystem;
    static path temp_dir = unique_path(temp_directory_path().append("paal_tests-%%%%%%%"));
    return temp_dir.native();
}

/// Return file path for temporary file
inline std::string get_temp_file_path(std::string const &file_name) {
    static std::string const temp_dir = get_static_temp_dir();
    return build_path(temp_dir, file_name);
}

/**
 * @brief Function for creating the file with specified text
 *
 * @param file_name of created file
 * @param text of created file
 *
 * @return std::string which contains path to the created file.
 */
inline std::string create_tmp_file(std::string const &file_name, std::string const &text) {
    // files created by this method will be removed after all tests execution
    static std::string const temp = get_static_temp_dir();
    std::string path = build_path(temp, file_name);

    std::ofstream ofs(path);
    ofs << text;

    return path;
}

/// Removes temporary directory or file
inline void remove_tmp_path(std::string const &path) {
    static std::string const temp_path{get_static_temp_dir()};

    assert(boost::starts_with(path, temp_path));
    boost::filesystem::remove_all(boost::filesystem::path(path));
}

} //! system
} //! paal

#endif // PAAL_GET_TEST_DIR
