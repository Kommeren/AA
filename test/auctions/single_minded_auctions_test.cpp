//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file single_minded_auctions_test.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-2-5
 */

#include "paal/auctions/auction_components.hpp"
#include "paal/auctions/single_minded_auctions.hpp"
#include "paal/data_structures/fraction.hpp"
#include "paal/utils/functors.hpp"
#include "utils/auctions.hpp"

#include <boost/test/unit_test.hpp>

#include <iterator>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>

BOOST_AUTO_TEST_CASE(testSingleMindedAuction)
{
   using Bidder = std::string;
   using Bidders = std::unordered_set<Bidder>;
   using Item = std::string;
   using Items = std::unordered_set<Item>;
   using Value = double;
   using Bid = std::pair<Items, Value>;
   namespace pa = paal::auctions;

   // input data
   Bidders bidders {"John", "Bob", "Steve"};
   std::unordered_map<Bidder, Bid> bids {
      {"John", {Items{}, 0.0}},
      {"Bob", {{"apple", "orange", "knife"}, 4.5}},
      {"Steve", {{"apple", "ball"}, 10.1}}
   };
   auto get_value = [&](const Bidder& bidder) { return bids[bidder].second; };
   auto get_items = [&](const Bidder& bidder) -> const Items& { return bids[bidder].first; };
   auto get_copies_num = [](const Item& item) { return item == "apple" ? 2 : 1; };

   Items items;
   pa::extract_items_from_single_minded(
      bidders, get_items, std::inserter(items, items.begin())
   );

   Items want_items = {"apple", "ball", "knife", "orange"};

   // value query auction
   {
      auto auction = pa::make_single_minded_to_value_query_auction(
         bidders, items, get_value, get_items, get_copies_num
      );
      BOOST_CHECK(bidders == Bidders(
         std::begin(auction.get<pa::bidders>()), std::end(auction.get<pa::bidders>())));
      BOOST_CHECK(want_items == Items(
         std::begin(auction.get<pa::items>()), std::end(auction.get<pa::items>())));
      BOOST_CHECK(auction.call<pa::get_copies_num>("apple") == 2);
      BOOST_CHECK(auction.call<pa::get_copies_num>("knife") == 1);
      const auto tests = {
         std::make_tuple("John", Items{}, 0.),
         std::make_tuple("John", Items{"apple", "knife"}, 0.),
         std::make_tuple("Bob", Items{"apple", "ball", "orange"}, 0.),
         std::make_tuple("Bob", Items{"apple", "knife", "orange"}, 4.5),
         std::make_tuple("Steve", Items{"apple", "orange", "knife"}, 0.),
         std::make_tuple("Steve", Items{"apple", "orange", "knife", "ball"}, 10.1),
      };
      for (auto t: tests)
         BOOST_CHECK_EQUAL(
            auction.call<pa::value_query>(std::get<0>(t), std::get<1>(t)), std::get<2>(t)
         );
   }

   // gamma oracle auction
   {
      using Frac = paal::data_structures::fraction<Value, Value>;
      using Result = boost::optional<Frac>;

      auto auction = pa::make_single_minded_to_gamma_oracle_auction(
         bidders, items, get_value, get_items, get_copies_num
      );
      BOOST_CHECK(bidders == Bidders(
         std::begin(auction.get<pa::bidders>()), std::end(auction.get<pa::bidders>())));
      BOOST_CHECK(want_items == Items(
         std::begin(auction.get<pa::items>()), std::end(auction.get<pa::items>())));
      BOOST_CHECK(auction.call<pa::get_copies_num>("apple") == 2);
      BOOST_CHECK(auction.call<pa::get_copies_num>("orange") == 1);
      BOOST_CHECK(auction.get<pa::gamma>() == 1);
      auto test = [&](Bidder bidder, Value threshold, Result want_frac, Items want_items = Items{}) {
         return check_gamma_oracle<Items>(
            auction, bidder, paal::utils::return_one_functor(), threshold, want_frac, want_items
         );
      };
      const Result none;
      BOOST_CHECK(test("John", 1., none));
      BOOST_CHECK(test("Bob", 4.5, none));
      BOOST_CHECK(test("Bob", 4., Frac(3.0, 0.5), Items{"apple", "orange", "knife"}));
      BOOST_CHECK(test("Steve", 10.1, none));
      BOOST_CHECK(test("Steve", 10., Frac(2.0, 0.1), Items{"apple", "ball"}));
   }
}
