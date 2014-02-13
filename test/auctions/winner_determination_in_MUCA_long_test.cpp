//=======================================================================
// Copyright (c) 2013 Robert Rosolek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file winner_determination_in_MUCA_long_test.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-3-20
 */

#include "test_utils/parse_file.hpp"
#include "test_utils/winner_determination_in_MUCA.hpp"
#include "test_utils/winner_determination_in_MUCA_long_test.hpp"

#include "paal/utils/functors.hpp"

#include <boost/range/irange.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/property_map/vector_property_map.hpp>

#include <fstream>
#include <iterator>
#include <random>
#include <string>
#include <utility>
#include <vector>

BOOST_AUTO_TEST_CASE(testDetermineWinnersLong)
{
   std::default_random_engine generator(7);
   std::uniform_real_distribution<Value> distribution(1.0, 2.0);

   paal::parse(test_dir + cases_file, [&](std::string test_name, paal::utils::ignore_param) {
      Value lower_bound;
      std::vector<Bids> bids;
      std::vector<int> item_count;
      int bidders_num, items_num;
      std::tie(lower_bound, bids, item_count, bidders_num, items_num) = read_auction(test_name);

      auto bidders = boost::irange(0, bidders_num);
      auto items = boost::irange(0, items_num);
      auto get_bids = [&](Bidder bidder) -> const Bids& { return bids.at(bidder); };
      auto get_value = [](const Bid& bid) { return bid.second; };
      auto get_items = [](const Bid& bid) -> const Items& { return bid.first; };
      auto get_copies_num = paal::utils::make_array_to_functor(item_count);
      auto gamma = distribution(generator);
      check_determine_winners_in_gamma_oracle(
         bidders,
         items,
         get_bids,
         get_value,
         get_items,
         get_copies_num,
         gamma,
         lower_bound,
         boost::vector_property_map<Value>(),
         1e-8
      );
   });
}
