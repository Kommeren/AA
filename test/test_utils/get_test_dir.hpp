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

inline std::string get_test_dir(std::string path_name) {
    return PROJECT_DIR"test/data/" + path_name + "/";
}

#endif // PAAL_GET_TEST_DIR
