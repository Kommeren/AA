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

template <class Auction, class Bidders, class Items>
void check_auction_bidders_and_items(Auction&& auction,
      Bidders&& want_bidders, Items&& want_items)
{
   BOOST_CHECK(want_bidders == auction.template get<paal::auctions::bidders>());
   BOOST_CHECK(want_items == auction.template get<paal::auctions::items>());
}

template <
   class Items,
   class DemandQueryAuction,
   class GetPrices,
   class Bidder,
   class Eps = double,
   class Traits = paal::auctions::demand_query_auction_traits<DemandQueryAuction>
>
bool check_demand_query(
   DemandQueryAuction&& auction,
   Bidder&& bidder,
   GetPrices get_prices,
   typename Traits::result_t want,
   Eps eps = 1e-8
) {
   auto got = auction.template call<paal::auctions::demand_query>(
      std::forward<Bidder>(bidder), get_prices
   );
   paal::utils::compare<typename Traits::value_t> cmp{eps};
   return cmp.e(want.second, got.second) && want.first == got.first;
}

template <
   class Items,
   class GammaOracleAuction,
   class GetPrices,
   class Bidder,
   class Traits = paal::auctions::gamma_oracle_auction_traits<GammaOracleAuction>
>
bool check_gamma_oracle(
   GammaOracleAuction&& auction,
   Bidder&& bidder,
   GetPrices get_prices,
   typename Traits::value_t threshold,
   typename Traits::result_t want
) {
   auto got = auction.template call<paal::auctions::gamma_oracle>(
      std::forward<Bidder>(bidder), get_prices, threshold
   );
   if (!want || !got) return !want == !got;
   return paal::data_structures::are_fractions_equal(want->second, got->second, 1e-3) &&
      want->first == got->first;
}

#endif // PAAL_AUCTIONS_HPP
