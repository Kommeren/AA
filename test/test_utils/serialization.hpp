//=======================================================================
// Copyright (c) 2015
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file serialization.hpp
 * @brief
 * @author Tomasz Strozak
 * @version 1.0
 * @date 2015-06-23
 */

#ifndef PAAL_SERIALIZATION_HPP
#define PAAL_SERIALIZATION_HPP

#include "test_utils/get_test_dir.hpp"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <string>

template <typename Model>
void serialize_test(Model const &model, std::string const &filename) {
    auto fname = paal::system::get_temp_file_path(filename);

    {
        std::ofstream ofs(fname);
        boost::archive::binary_oarchive oa(ofs);
        oa << model;
    }

    Model model_test;
    std::ifstream ifs(fname);
    boost::archive::binary_iarchive ia(ifs);
    ia >> model_test;

    BOOST_CHECK(model == model_test);
    paal::system::remove_tmp_path(fname);
}

#endif // PAAL_SERIALIZATION_HPP
