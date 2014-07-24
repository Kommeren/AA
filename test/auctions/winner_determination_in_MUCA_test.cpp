//=======================================================================
// Copyright (c) 2013 Robert Rosolek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file winner_determination_in_MUCA_test.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-3-27
 */

#include "utils/winner_determination_in_MUCA.hpp"

#include "paal/auctions/winner_determination_in_MUCA/winner_determination_in_MUCA.hpp"

#include <boost/mem_fn.hpp>
#include <boost/test/unit_test.hpp>

#include <functional>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

BOOST_AUTO_TEST_CASE(testDetermineWinners)
{
   using Bidder = std::string;
   using Item = std::string;
   using Items = std::set<Item>;
   using Bid = std::pair<Items, double>;
   using Bids = std::vector<Bid>;
   std::set<Bidder> bidders {"John", "Bob", "Steve"};
   std::map<Bidder, Bids> bids {
      {"John", {
         {Items{}, 0},
         {{"apple", "ball"}, 1.7},
      }},
      {"Bob", {
         {{"lemon", "orange", "sugar", "knife"}, 2.7},
         {{"apple", "lemon", "knife"}, 1.8},
         {{"apple", "ball", "orange", "sugar", "lemon", "knife"}, 4},
      }},
      {"Steve", {
         {{"lemon"}, 4},
         {{"apple"}, 4.6},
         {{"ball"}, 3.1},
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
   auto gamma = 1.5;
   auto opt = 1.7 + 2.7 + 4.0;
   check_determine_winners_in_gamma_oracle(
      bidders, items, get_bids, get_value, get_items, get_copies_num, gamma, opt
   );
}
