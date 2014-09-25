//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file winner_determination_in_multi_unit_CAs_example.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-1-9
 */

#include "paal/auctions/auction_components.hpp"
#include "paal/auctions/xor_bids.hpp"
#include "paal/auctions/winner_determination_in_multi_unit_CAs/winner_determination_in_multi_unit_CAs.hpp"

#include <boost/function_output_iterator.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

//! [Winner Determination In Multi Unit CAs Example]

int main()
{
   using Bidder = std::string;
   using Item = std::string;
   using Items = std::unordered_set<Item>;
   using Value = double;
   using Bid = std::pair<Items, Value>;
   using Bids = std::vector<Bid>;

   // create auction
   std::unordered_map<Bidder, Bids> bids {
      {"John", {
         {{"ball", "kite"}, 2},
         {{"umbrella"}, 3},
         {{"orange"}, 1.75},
         {{"ball", "kite", "umbrella"}, 5},
         {{"ball", "kite", "orange", "umbrella"}, 6.75},
      }},
      {"Bob", {
         {{"orange"}, 1.5},
         {{"apple"}, 2.0},
         {{"apple", "orange"}, 4},
      }},
      {"Steve", {
         {{"apple"}, 1},
         {{"umbrella"}, 4},
         {{"apple", "umbrella"}, 5},
      }},
   };
   std::vector<Bidder> bidders {"John", "Bob", "Steve"};
   std::vector<Item> items {"apple", "ball", "orange", "kite", "umbrella"};
   auto get_bids = [&](const Bidder& bidder) -> const Bids& { return bids.at(bidder); };
   auto get_value = [](const Bid& bid) { return bid.second; };
   auto get_items = [](const Bid& bid) -> const Items& { return bid.first; };
   auto auction = paal::auctions::make_xor_bids_to_gamma_oracle_auction(
      bidders, items, get_bids, get_value, get_items
   );

   // determine winners
   std::unordered_map<Bidder, Items> assignment;
   paal::auctions::determine_winners_in_gamma_oracle_auction(
      auction,
      boost::make_function_output_iterator([&](const std::pair<Bidder, Item>& p)
         {
            auto bidder = p.first;
            auto item = p.second;
            std::cout << bidder << " got " << item << std::endl;
            assignment[bidder].insert(item);
         }
      )
   );

   // calculate social welfare
   Value social_welfare = 0;
   auto valuation = paal::auctions::make_xor_bids_to_value_query_auction(
      bidders, items, get_bids, get_value, get_items
   );
   for (auto bidder: bidders)
      social_welfare += valuation.call<paal::auctions::value_query>(bidder, assignment[bidder]);
   std::cout << "Social welfare: " << social_welfare << std::endl;

   return 0;
}

//! [Winner Determination In Multi Unit CAs Example]
