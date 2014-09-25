//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file winner_determination_in_multi_unit_CAs.hpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-3-24
 */
#ifndef WINNER_DETERMINATION_IN_MUCAS_UTILS
#define WINNER_DETERMINATION_IN_MUCAS_UTILS

#include "paal/auctions/auction_components.hpp"
#include "paal/auctions/auction_traits.hpp"
#include "paal/auctions/auction_utils.hpp"
#include "paal/auctions/xor_bids.hpp"
#include "paal/auctions/winner_determination_in_multi_unit_CAs/winner_determination_in_multi_unit_CAs.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/type_functions.hpp"
#include "utils/auctions.hpp"
#include "utils/test_gamma_oracle_xor_bids.hpp"
#include "utils/test_result_check.hpp"

#include <cmath>
#include <iterator>
#include <utility>
#include <vector>

template <class GammaOracleAuction>
auto approximation_ratio(GammaOracleAuction&& auction)
-> puretype(
      std::exp(1.0) *
      (2 * auction.template get<paal::auctions::gamma>() + 1) *
      std::pow(
         paal::auctions::items_number(auction),
         1.0 / paal::auctions::get_minimum_copies_num(auction)
      )
   )
{
   auto e = std::exp(1.0);
   auto m = paal::auctions::items_number(auction);
   auto b = paal::auctions::get_minimum_copies_num(auction);
   auto gamma = auction.template get<paal::auctions::gamma>();
   return e * (2 * gamma + 1) * std::pow(m, 1.0 / (b + 1));
}

template<
   class Bidders,
   class Items,
   class GetBids,
   class GetValue,
   class GetItems,
   class GetQuantity,
   class PriceMap,
   class Epsilon
>
void check_determine_winners_in_gamma_oracle(
   Bidders&& bidders,
   Items&& items,
   GetBids get_bids,
   GetValue get_value,
   GetItems get_items,
   GetQuantity get_quantity,
   double gamma,
   double lower_bound,
   PriceMap prices,
   Epsilon epsilon
) {
   auto auction = make_test_xor_bids_to_gamma_oracle_auction(
      std::forward<Bidders>(bidders),
      std::forward<Items>(items),
      get_bids,
      get_value,
      get_items,
      gamma,
      get_quantity
   );

   using Auction = decltype(auction);
   using Bidder = typename paal::auctions::auction_traits<Auction>::bidder_val_t;
   using Item = typename paal::auctions::auction_traits<Auction>::item_val_t;

   std::vector<std::pair<Bidder, Item>> result;
   paal::auctions::determine_winners_in_gamma_oracle_auction(
      auction, std::back_inserter(result), prices, epsilon
   );
   BOOST_CHECK(is_feasible(auction, result));
   auto value_query = paal::auctions::make_xor_bids_to_value_query_auction(
      bidders, items, get_bids, get_value, get_items, get_quantity
   );
   auto ratio = approximation_ratio(auction);
   auto value = assignment_value(value_query, result);
   check_result_compare_to_bound(value, lower_bound, 1.0 / ratio, paal::utils::greater_equal(), 1e-3);
}

template<
   class Bidders,
   class Items,
   class GetBids,
   class GetValue,
   class GetItems,
   class GetQuantity,
   class Epsilon = double
>
void check_determine_winners_in_gamma_oracle(
   Bidders&& bidders,
   Items&& items,
   GetBids get_bids,
   GetValue get_value,
   GetItems get_items,
   GetQuantity get_quantity,
   double gamma,
   double lower_bound,
   Epsilon epsilon = 1e-8
) {
   using Value = puretype(get_value(*std::begin(get_bids(*std::begin(bidders)))));
   using ItemVal = puretype(*std::begin(items));
   using PriceMap = std::unordered_map<ItemVal, Value>;

   PriceMap map;
   check_determine_winners_in_gamma_oracle(
      std::forward<Bidders>(bidders),
      std::forward<Items>(items),
      get_bids,
      get_value,
      get_items,
      get_quantity,
      gamma,
      lower_bound,
      boost::make_assoc_property_map(map),
      epsilon
   );
}

#endif /* WINNER_DETERMINATION_IN_MUCAS_UTILS */
