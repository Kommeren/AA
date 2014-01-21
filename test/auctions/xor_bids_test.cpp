/**
 * @file xor_bids_test.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-2-25
 */

#include "paal/auctions/auction_components.hpp"
#include "paal/auctions/xor_bids.hpp"
#include "paal/data_structures/fraction.hpp"
#include "paal/utils/functors.hpp"
#include "utils/auctions.hpp"

#include <boost/optional/optional.hpp>
#include <boost/test/unit_test.hpp>

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
   auto get_value = [](const Bid& bid) { return bid.second; };
   auto get_items = [](const Bid& bid) -> const Items& { return bid.first; };
   auto get_copies_num = [](const Item& item) { return item == "knife" ? 1 : 3; };
   Items items;
   paal::auctions::extract_items_from_xor_bids(
      bidders, get_bids, get_items, std::inserter(items, items.begin())
   );

   Items want_items {"apple", "ball", "knife", "lemon", "orange", "sugar"};

   // value query auction
   {
      auto auction = paal::auctions::make_xor_bids_to_value_query_auction(
         bidders, items, get_bids, get_value, get_items, get_copies_num
      );
      BOOST_CHECK(bidders == Bidders(
         std::begin(auction.get<paal::auctions::bidders>()), std::end(auction.get<paal::auctions::bidders>())));
      BOOST_CHECK(want_items == Items(
         std::begin(auction.get<paal::auctions::items>()), std::end(auction.get<paal::auctions::items>())));
      BOOST_CHECK(auction.call<paal::auctions::get_copies_num>("apple") == 3);
      BOOST_CHECK(auction.call<paal::auctions::get_copies_num>("knife") == 1);
      BOOST_CHECK(auction.call<paal::auctions::value_query>(
         "John", Items{"apple", "ball"}) == 0);
      BOOST_CHECK(auction.call<paal::auctions::value_query>(
         "John", Items{"apple", "knife"}) == 1.7);
      BOOST_CHECK(auction.call<paal::auctions::value_query>(
         "John", Items{"knife", "orange", "lemon"}) == 0);
      BOOST_CHECK(auction.call<paal::auctions::value_query>("Bob", Items{}) == 0);
      BOOST_CHECK(auction.call<paal::auctions::value_query>(
         "Bob", Items{"apple", "lemon", "orange", "sugar", "knife"}) == 2.7);
      BOOST_CHECK(auction.call<paal::auctions::value_query>(
         "Bob", Items{"apple", "lemon", "sugar", "knife", "ball"}) == 1.8);
      BOOST_CHECK(auction.call<paal::auctions::value_query>(
         "Bob", Items{"apple", "ball", "orange", "sugar", "lemon", "knife"}) == 4.0);
      BOOST_CHECK(auction.call<paal::auctions::value_query>(
         "Steve", Items{"apple", "lemon", "ball"}) == 4.6);
      BOOST_CHECK(auction.call<paal::auctions::value_query>(
         "Steve", Items{"lemon", "ball"}) == 4.0);
   }

   // gamma oracle auction
   {
      using Frac = paal::data_structures::fraction<Value, Value>;

      auto auction = paal::auctions::make_xor_bids_to_gamma_oracle_auction(
         bidders, items, get_bids, get_value, get_items, get_copies_num
      );

      BOOST_CHECK(bidders == Bidders(
         std::begin(auction.get<paal::auctions::bidders>()), std::end(auction.get<paal::auctions::bidders>())));
      BOOST_CHECK(want_items == Items(
         std::begin(auction.get<paal::auctions::items>()), std::end(auction.get<paal::auctions::items>())));
      BOOST_CHECK(auction.call<paal::auctions::get_copies_num>("apple") == 3);
      BOOST_CHECK(auction.call<paal::auctions::get_copies_num>("knife") == 1);
      BOOST_CHECK(auction.get<paal::auctions::gamma>() == 1);

      auto unordered_map_to_func = [](const std::unordered_map<Item, Value> &m)
      {
         return [&](const Item& item) { return m.count(item) > 0 ? m.at(item) : 1; };
      };
      const boost::optional<Frac> none;
      BOOST_CHECK(check_gamma_oracle<Items>(
         auction, "John", paal::utils::return_one_functor(), 1.7, none
      ));
      BOOST_CHECK(check_gamma_oracle(
         auction, "John", paal::utils::return_one_functor(), 1.6,
         Frac(2.0, 0.1), Items{"apple", "knife"}
      ));
      BOOST_CHECK(check_gamma_oracle<Items>(
         auction, "Bob", paal::utils::return_one_functor(), 4.0, none
      ));
      BOOST_CHECK(check_gamma_oracle(
         auction, "Bob", unordered_map_to_func({{"sugar", 10}, {"orange", 5}}), 1.,
         Frac(3.0, 0.8), Items{"apple", "lemon", "knife"}
      ));
      BOOST_CHECK(check_gamma_oracle(
         auction, "Bob", unordered_map_to_func({{"sugar", 10}, {"orange", 5}}), 2.,
         Frac(19.0, 2.0), Items{"apple", "ball", "orange", "sugar", "lemon", "knife"}
      ));
      BOOST_CHECK(check_gamma_oracle(
         auction, "Bob", unordered_map_to_func({{"ball", 7}}), 2.,
         Frac(4.0, 0.7), Items{"lemon", "orange", "sugar", "knife"}
      ));
      BOOST_CHECK(check_gamma_oracle(
         auction, "Steve", paal::utils::return_one_functor(), 1.0,
         Frac(1.0, 3.6), Items{"apple"}
      ));
      BOOST_CHECK(check_gamma_oracle(
         auction, "Steve", unordered_map_to_func({{"apple", 2}, {"lemon", 1.5}}), 1.0,
         Frac(1.0, 2.1), Items{"ball"}
      ));
   }
}
