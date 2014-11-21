//=======================================================================
// Copyright (c) 2013 Robert Rosolek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file fractional_winner_determination_in_MUCA_long_test.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-06-10
 */

#include "test_utils/fractional_winner_determination_in_MUCA_utils.hpp"
#include "test_utils/parse_file.hpp"
#include "test_utils/winner_determination_in_MUCA_utils.hpp"
#include "test_utils/winner_determination_in_MUCA_long_test.hpp"

#include "paal/lp/glp.hpp"
#include "paal/utils/functors.hpp"

#include <boost/property_map/vector_property_map.hpp>
#include <boost/range/irange.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

BOOST_AUTO_TEST_CASE(testFracDetermineWinnersLong)
{

   paal::parse(test_dir + cases_file, [](std::string test_name, paal::utils::ignore_param) {
      Value opt;
      std::vector<Bids> bids;
      std::vector<int> item_count;
      int bidders_num, items_num;
      if (test_name.find("large") != std::string::npos) return;
      std::tie(opt, bids, item_count, bidders_num, items_num) = read_auction(test_name, ".frac.sol");

      // TODO once C++14 arrives create and return these functors in read_auction
      auto bidders = boost::irange(0, bidders_num);
      auto items = boost::irange(0, items_num);
      auto get_bids = [&](Bidder bidder) -> const Bids& { return bids.at(bidder); };
      auto get_value = [](const Bid& bid) { return bid.second; };
      auto get_items = [](const Bid& bid) -> const Items& { return bid.first; };
      auto get_copies_num = paal::utils::make_array_to_functor(item_count);
      check_fractional_determine_winners_in_demand_query_auction(
         bidders,
         items,
         get_bids,
         get_value,
         get_items,
         get_copies_num,
         opt,
         boost::vector_property_map<paal::lp::col_id>(),
         1e-8
      );
   });
}
