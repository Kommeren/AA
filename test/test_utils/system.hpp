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

#define BOOST_ERROR_CODE_HEADER_ONLY
#define BOOST_SYSTEM_NO_DEPRECATED

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>

#include <cassert>
#include <cstdlib>
#include <string>

namespace paal {
namespace system {

/// Function fixes path separators to native and creates path
inline std::string build_path(std::string const &prefix, std::string const &suffix) {
    boost::filesystem::path res{prefix};
    res.append(suffix);
    return res.make_preferred().native();
}

/// Creates directory
inline void create_directory(std::string const &path) {
    bool created = boost::filesystem::create_directory(boost::filesystem::path(path));
    assert(created);
}

/// Execs command
inline int exec(std::string const &cmd,
                bool discard_output = true,
                std::string const &stdout = "/dev/null",
                std::string const &stderr = "/dev/null") {
    //TODO OS portable
    std::string const redirections = " >/dev/null 2>&1";
    std::string optional_redirections = discard_output ? redirections : " >" + stdout + " 2>" + stderr;

    return std::system((cmd + optional_redirections).c_str());
}


} //! system

} //! paal
#endif // PAAL_SYSTEM_HPP
