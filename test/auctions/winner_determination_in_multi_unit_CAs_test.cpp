/**
 * @file winner_determination_in_multi_unit_CAs_test.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-3-27
 */

#include "paal/auctions/winner_determination_in_multi_unit_CAs/winner_determination_in_multi_unit_CAs.hpp"
#include "utils/auctions.hpp"
#include "utils/winner_determination_in_multi_unit_CAs.hpp"

#include <boost/test/unit_test.hpp>

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
std::set<Bidder> bidders {"John", "Bob", "Steve"}; std::map<Bidder, Bids> bids {
      {"John", {
         { {}, 0.0},
         { {"apple", "ball"}, 1.7},
      }},
      {"Bob", {
         { {"lemon", "orange", "sugar", "knife"}, 2.7},
         { {"apple", "lemon", "knife"}, 1.8},
         { {"apple", "ball", "orange", "sugar", "lemon", "knife"}, 4.0},
      }},
      {"Steve", {
         { {"lemon"}, 4.0},
         { {"apple"}, 4.6},
         { {"ball"}, 3.1},
      }},
   };
   auto get_bids = [&](const Bidder& bidder) -> const Bids& { return bids.at(bidder); };
   auto get_value = [](const Bid& bid) { return bid.second; };
   auto get_items = [](const Bid& bid) -> const Items& { return bid.first; };
   auto get_quantity = [](const Item& item) { return item == "lemon" ? 2 : 1; };
   Items items;
   paal::auctions::extract_items_from_xor_bids(
      bidders, get_bids, get_items, std::inserter(items, items.begin())
   );
   auto gamma = 1.5;
   auto opt = 1.7 + 2.7 + 4.0;
   check_determine_winners_in_gamma_oracle(
      bidders, items, get_bids, get_value, get_items, get_quantity, gamma, opt
   );
}
