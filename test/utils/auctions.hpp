//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file auctions.hpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-2-25
 */
#ifndef PAAL_AUCTIONS_HPP
#define PAAL_AUCTIONS_HPP

#include "paal/auctions/auction_components.hpp"
#include "paal/auctions/auction_traits.hpp"
#include "paal/data_structures/fraction.hpp"

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/test/unit_test.hpp>

#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <utility>

template <
   class Items,
   class GammaOracleAuction,
   class GetPrices,
   class Bidder,
   class Traits = paal::auctions::gamma_oracle_auction_traits<GammaOracleAuction>
>
bool check_gamma_oracle(
   GammaOracleAuction&& auction,
   Bidder bidder,
   GetPrices get_prices,
   typename Traits::value_t threshold,
   typename Traits::gamma_oracle_result_t want_frac,
   Items want_items = Items{}
) {
   std::vector<typename Traits::item_val_t> got_items;
   auto got_frac = auction.template call<paal::auctions::gamma_oracle>(
      bidder, get_prices, threshold, std::back_inserter(got_items)
   );
   if (!want_frac || !got_frac) return !want_frac == !got_frac;
   return paal::data_structures::are_fractions_equal(*want_frac, *got_frac, 1e-3) &&
      want_items == Items(std::begin(got_items), std::end(got_items));
}

template <
   class ValueQueryAuction,
   class Assignments,
   class Traits = paal::auctions::value_query_auction_traits<ValueQueryAuction>
>
typename Traits::value_t
assignment_value(ValueQueryAuction&& value_query, const Assignments& assignments)
{
   using Bidder = typename Traits::bidder_val_t;
   using Item = typename Traits::item_val_t;

   std::unordered_map<Bidder, std::unordered_set<Item>>  bidder_to_items;
   for (auto& assignment: assignments)
      bidder_to_items[assignment.first].insert(assignment.second);

   return paal::utils::accumulate_functor(
      bidder_to_items,
      typename Traits::value_t(0),
      [&](decltype(*bidder_to_items.cbegin()) it)
      {
         return value_query.template call<paal::auctions::value_query>(it.first, it.second);
      }
   );
}

template <class Auction, class Assignments>
bool is_feasible(Auction&& auction, const Assignments& assignments)
{
   using Traits = paal::auctions::auction_traits<Auction>;
   using Item = typename Traits::item_val_t;
   using CopiesNum = typename Traits::copies_num_t;

   std::unordered_map<Item, CopiesNum> count;
   for (auto& assignment: assignments)
      ++count[assignment.second];

   return boost::algorithm::all_of(count, [&](decltype(*count.cbegin()) item_count)
   {
      return item_count.second <=
         auction.template call<paal::auctions::get_copies_num>(item_count.first);
   });
}

#endif // PAAL_AUCTIONS_HPP
