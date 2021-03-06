//=======================================================================
// Copyright (c) 2013 Robert Rosolek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file fractional_winner_determination_in_MUCA_test.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-07-24
 */

#include "test_utils/fractional_winner_determination_in_MUCA_test_utils.hpp"

#include "paal/auctions/fractional_winner_determination_in_MUCA/fractional_winner_determination_in_MUCA.hpp"
#include "paal/auctions/xor_bids.hpp"

#include <boost/mem_fn.hpp>
#include <boost/test/unit_test.hpp>

#include <functional>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

BOOST_AUTO_TEST_CASE(testFracDetermineWinners)
{
   using Bidder = std::string;
   using Item = std::string;
   using Items = std::set<Item>;
   using Bid = std::pair<Items, double>;
   using Bids = std::vector<Bid>;
   const std::vector<Bidder> bidders {"John", "Bob"};
   const std::map<Bidder, Bids> bids {
      {"John", {
         {Items{}, 0},
         {{"lemon", "orange"}, 1},
         {{"apple", "ball"}, 1},
      }},
      {"Bob", {
         {{"lemon", "apple"}, 1},
         {{"orange", "ball"}, 1},
      }},
   };
   auto get_bids = [&](const Bidder& bidder) -> const Bids& { return bids.at(bidder); };
   auto get_value = boost::mem_fn(&Bid::second);
   auto get_items = boost::mem_fn(&Bid::first);
   auto get_copies_num = [](const Item& item) { return item == "lemon" ? 2 : 1; };
   Items items;
   paal::auctions::extract_items_from_xor_bids(
      bidders, get_bids, get_items, std::inserter(items, items.begin())
   );
   auto opt = 0.5 * (1 + 1 + 1 + 1);
   check_fractional_determine_winners_in_demand_query_auction(
      bidders, items, get_bids, get_value, get_items, get_copies_num, opt
   );
}
