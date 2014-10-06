//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file xor_bids_test.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-2-25
 */

#include "test_utils/auctions.hpp"

#include "paal/auctions/auction_components.hpp"
#include "paal/auctions/xor_bids.hpp"
#include "paal/data_structures/fraction.hpp"
#include "paal/utils/functors.hpp"

#include <boost/mem_fn.hpp>
#include <boost/none.hpp>
#include <boost/test/unit_test.hpp>

#include <functional>
#include <iterator>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

BOOST_AUTO_TEST_CASE(testXorBidsAuction)
{
   using Bidder = std::string;
   using Bidders = std::unordered_set<Bidder>;
   using Item = std::string;
   using Items = std::unordered_set<Item>;
   using Value = double;
   using Bid = std::pair<Items, Value>;
   using Bids = std::vector<Bid>;
   namespace pu = paal::utils;
   namespace pa = paal::auctions;

   //input data
   Bidders bidders {"John", "Bob", "Steve"};
   std::unordered_map<Bidder, Bids> bids {
      {"John", {
         {Items{}, 0},
         {{"apple", "knife"}, 1.7},
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
   auto get_copies_num = [](const Item& item) { return item == "knife" ? 1 : 3; };
   Items items;
   pa::extract_items_from_xor_bids(
      bidders, get_bids, get_items, std::inserter(items, items.begin())
   );

   Items want_items {"apple", "ball", "knife", "lemon", "orange", "sugar"};

   // value query auction
   {
      auto auction = pa::make_xor_bids_to_value_query_auction(
         bidders, items, get_bids, get_value, get_items, get_copies_num
      );
      // TODO this can be refactored even more into lambda
      // check_bidders_and_items() defined for all auction
      // types when C++ polymorphic lambdas arrive
      check_auction_bidders_and_items(auction, bidders, want_items);
      BOOST_CHECK_EQUAL(auction.call<pa::get_copies_num>("apple"), 3);
      BOOST_CHECK_EQUAL(auction.call<pa::get_copies_num>("knife"), 1);
      const auto tests = {
         std::make_tuple("John", Items{"apple", "ball"}, 0.),
         std::make_tuple("John", Items{"apple", "knife"}, 1.7),
         std::make_tuple("John", Items{"knife", "orange", "lemon"}, 0.),
         std::make_tuple("Bob", Items{}, 0.),
         std::make_tuple("Bob", Items{"apple", "lemon", "orange", "sugar", "knife"}, 2.7),
         std::make_tuple("Bob", Items{"apple", "lemon", "sugar", "knife", "ball"}, 1.8),
         std::make_tuple("Bob", Items{"apple", "ball", "orange", "sugar", "lemon", "knife"}, 4.0),
         std::make_tuple("Steve", Items{"apple", "lemon", "ball"}, 4.6),
         std::make_tuple("Steve", Items{"lemon", "ball"}, 4.0)
      };
      for (auto t: tests)
         BOOST_CHECK_EQUAL(
            auction.call<pa::value_query>(std::get<0>(t), std::get<1>(t)), std::get<2>(t)
         );
   }

   auto map_to_func = [](const std::unordered_map<Item, Value> &m)
   {
      return [&](const Item& item) { return m.count(item) > 0 ? m.at(item) : 1; };
   };

   // demand query auction
   {
      auto auction = pa::make_xor_bids_to_demand_query_auction(
         bidders, items, get_bids, get_value, get_items, get_copies_num
      );
      check_auction_bidders_and_items(auction, bidders, want_items);
      BOOST_CHECK_EQUAL(auction.call<pa::get_copies_num>("sugar"), 3);
      BOOST_CHECK_EQUAL(auction.call<pa::get_copies_num>("knife"), 1);

      const auto empty = std::make_pair(Items{}, 0);
      // TODO refactor these checks once C++14 polymorphic lambdas arrive
      BOOST_CHECK(check_demand_query<Item>(auction, "John", pu::return_one_functor(), empty));
      BOOST_CHECK(check_demand_query<Item>(
         auction, "John", pu::make_dynamic_return_constant_functor(0.8),
         std::make_pair(Items{"apple", "knife"}, 0.1)
      ));
      BOOST_CHECK(check_demand_query<Item>(auction, "Bob", pu::return_one_functor(), empty));
      BOOST_CHECK(check_demand_query<Item>(
         auction, "Bob", map_to_func({{"apple", 0.1}, {"lemon", 0.1}, {"knife", 0.1}, {"orange", 0.1}}),
         std::make_pair(Items{"apple", "ball", "orange", "sugar", "lemon", "knife"}, 1.6)
      ));
      BOOST_CHECK(check_demand_query<Item>(
         auction, "Bob", map_to_func({{"apple", 0.3}, {"lemon", 0.3}, {"knife", 0.3}, {"orange", 0.3}}),
         std::make_pair(Items{"apple", "lemon", "knife"}, 0.9)
      ));
      BOOST_CHECK(check_demand_query<Item>(auction, "Steve", pu::return_one_functor(),
         std::make_pair(Items{"apple"}, 3.6)));
      BOOST_CHECK(check_demand_query<Item>(
         auction, "Steve", pu::make_dynamic_return_constant_functor(5.0), empty
      ));
      BOOST_CHECK(check_demand_query<Item>(
         auction, "Steve", map_to_func({{"lemon", 2}, {"apple", 3}}), std::make_pair(Items{"ball"}, 2.1)
      ));
   }

   // gamma oracle auction
   {
      using Frac = paal::data_structures::fraction<Value, Value>;

      auto auction = pa::make_xor_bids_to_gamma_oracle_auction(
         bidders, items, get_bids, get_value, get_items, get_copies_num
      );
      check_auction_bidders_and_items(auction, bidders, want_items);
      BOOST_CHECK_EQUAL(auction.call<pa::get_copies_num>("apple"), 3);
      BOOST_CHECK_EQUAL(auction.call<pa::get_copies_num>("knife"), 1);
      BOOST_CHECK_EQUAL(auction.get<pa::gamma>(), 1);

      // TODO refactor these checks once C++14 polymorphic lambdas arrive
      BOOST_CHECK(check_gamma_oracle<Items>(
         auction, "John", pu::return_one_functor(), 1.7, boost::none
      ));
      BOOST_CHECK(check_gamma_oracle<Items>(
         auction, "John", pu::return_one_functor(), 1.6,
         std::make_pair(Items{"apple", "knife"}, Frac(2, 0.1))
      ));
      BOOST_CHECK(check_gamma_oracle<Items>(
         auction, "Bob", pu::return_one_functor(), 4, boost::none
      ));
      BOOST_CHECK(check_gamma_oracle<Items>(
         auction, "Bob", map_to_func({{"sugar", 10}, {"orange", 5}}), 1,
         std::make_pair(Items{"apple", "lemon", "knife"}, Frac(3, 0.8))
      ));
      BOOST_CHECK(check_gamma_oracle<Items>(
         auction, "Bob", map_to_func({{"sugar", 10}, {"orange", 5}}), 2,
         std::make_pair(Items{"apple", "ball", "orange", "sugar", "lemon", "knife"}, Frac(19, 2))
      ));
      BOOST_CHECK(check_gamma_oracle<Items>(
         auction, "Bob", map_to_func({{"ball", 7}}), 2,
         std::make_pair(Items{"lemon", "orange", "sugar", "knife"}, Frac(4, 0.7))
      ));
      BOOST_CHECK(check_gamma_oracle<Items>(
         auction, "Steve", pu::return_one_functor(), 1,
         std::make_pair(Items{"apple"}, Frac(1, 3.6))
      ));
      BOOST_CHECK(check_gamma_oracle<Items>(
         auction, "Steve", map_to_func({{"apple", 2}, {"lemon", 1.5}}), 1,
         std::make_pair(Items{"ball"}, Frac(1, 2.1))
      ));
   }
}
