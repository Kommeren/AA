//=======================================================================
// Copyright (c) 2013 Robert Rosolek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file winner_determination_in_MUCA.hpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-3-24
 */
#ifndef WINNER_DETERMINATION_IN_MUCA_UTILS
#define WINNER_DETERMINATION_IN_MUCA_UTILS

#include "paal/auctions/auction_components.hpp"
#include "paal/auctions/auction_traits.hpp"
#include "paal/auctions/auction_utils.hpp"
#include "paal/auctions/xor_bids.hpp"
#include "paal/auctions/winner_determination_in_MUCA/winner_determination_in_MUCA.hpp"
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
auto approximation_ratio(GammaOracleAuction&& auction) {
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
   class GetCopiesNum,
   class PriceMap,
   class Epsilon
>
void check_determine_winners_in_gamma_oracle(
   Bidders&& bidders,
   Items&& items,
   GetBids get_bids,
   GetValue get_value,
   GetItems get_items,
   GetCopiesNum get_copies_num,
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
      get_copies_num
   );

   using Traits = paal::auctions::gamma_oracle_auction_traits<decltype(auction)>;
   using Bidder = typename Traits::bidder_val_t;
   using Item = typename Traits::item_val_t;
   using ItemsBundle = typename Traits::items_t;
   using Value = typename Traits::value_t;

   auto valuation = paal::auctions::make_xor_bids_to_value_query_auction(
      bidders, items, get_bids, get_value, get_items, get_copies_num
   );
   Value social_welfare = 0;
   std::unordered_map<Item, Value> item_count;
   std::unordered_map<Bidder, Value> bidder_count;
   paal::auctions::determine_winners_in_gamma_oracle_auction(
      auction,
      boost::make_function_output_iterator([&](std::pair<Bidder, ItemsBundle> p)
      {
         auto bidder = p.first;
         auto& items = p.second;
         std::unordered_set<Item> item_set(std::begin(items), std::end(items));
         social_welfare += valuation.template call<paal::auctions::value_query>(bidder, item_set);
         for (auto item: items) {
            auto cnt = ++item_count[item];
            BOOST_CHECK_LE(cnt, get_copies_num(item));
         }
         auto cnt = ++bidder_count[bidder];
         BOOST_CHECK_LE(cnt, 1);
      }),
      prices,
      epsilon
   );
   auto ratio = approximation_ratio(auction);
   check_result_compare_to_bound(social_welfare, lower_bound, 1.0 / ratio,
         paal::utils::greater_equal(), epsilon);
}

template<
   class Bidders,
   class Items,
   class GetBids,
   class GetValue,
   class GetItems,
   class GetCopiesNum,
   class Epsilon = double
>
void check_determine_winners_in_gamma_oracle(
   Bidders&& bidders,
   Items&& items,
   GetBids get_bids,
   GetValue get_value,
   GetItems get_items,
   GetCopiesNum get_copies_num,
   double gamma,
   double lower_bound,
   Epsilon epsilon = 1e-8
) {
   using Value = paal::pure_result_of_t<
                    GetValue(paal::range_to_ref_t<
                            paal::pure_result_of_t<GetBids(paal::range_to_ref_t<paal::decay_t<Bidders>>)>
                            >)
                 >;
   using ItemVal = paal::range_to_elem_t<paal::decay_t<Items>>;

   std::unordered_map<ItemVal, Value> map;
   check_determine_winners_in_gamma_oracle(
      std::forward<Bidders>(bidders),
      std::forward<Items>(items),
      get_bids,
      get_value,
      get_items,
      get_copies_num,
      gamma,
      lower_bound,
      boost::make_assoc_property_map(map),
      epsilon
   );
}

#endif /* WINNER_DETERMINATION_IN_MUCA_UTILS */
