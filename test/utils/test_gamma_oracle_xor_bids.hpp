//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file test_gamma_oracle_xor_bids.hpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-3-20
 */
#ifndef PAAL_TEST_GAMMA_ORACLE_XOR_BIDS_HPP
#define PAAL_TEST_GAMMA_ORACLE_XOR_BIDS_HPP

#include "paal/auctions/auction_components.hpp"
#include "paal/auctions/xor_bids.hpp"
#include "paal/utils/functors.hpp"

#include <boost/none.hpp>

#include <cassert>
#include <utility>

namespace detail {

   template<class GetBids, class GetValue, class GetItems, class Gamma>
   class test_xor_bids_gamma_oracle {

      using xor_bids_gamma_oracle =
         paal::auctions::detail::xor_bids_gamma_oracle<GetBids, GetValue, GetItems>;

      Gamma m_gamma;
      xor_bids_gamma_oracle m_gamma_oracle;

      public:
         test_xor_bids_gamma_oracle(
            GetBids get_bids,
            GetValue get_value,
            GetItems get_items,
            Gamma gamma
         )
         : m_gamma(gamma), m_gamma_oracle(get_bids, get_value, get_items) {}

         template <class Bidder, class GetPrice, class Threshold, class OutputIterator>
         auto operator()(
            Bidder&& bidder,
            GetPrice get_price,
            Threshold threshold,
            OutputIterator result_items
         )
         -> puretype(m_gamma_oracle(
            std::forward<Bidder>(bidder), get_price, threshold, result_items
         )) const
         {
            using Traits =
               typename xor_bids_gamma_oracle:: template price_traits<decltype(bidder), GetPrice>;

            auto best =
               m_gamma_oracle.minimum_frac(std::forward<Bidder>(bidder), get_price, threshold);
            if (!best) return boost::none;
            auto result = m_gamma_oracle.calculate_best(
               std::forward<Bidder>(bidder),
               get_price,
               threshold,
               [&](typename Traits::frac frac, const typename Traits::best_bid& result)
               {
                  return frac <= m_gamma * best->second && (!result || result->second < frac);
               }
            );
            assert(result);
            return m_gamma_oracle.output(*result, result_items);
         }

   };
}; //!detail

/**
 * @brief Create test gamma oracle auction from xor bids valuations.
 * This gamma oracle implementation on purpose returns as bad bundles as
 * it is possible for a given gamma.
 *
 * @param bidders
 * @param items
 * @param get_bids
 * @param get_value
 * @param get_items
 * @param gamma
 * @param get_copies_num
 * @tparam Bidders
 * @tparam Items
 * @tparam GetBids
 * @tparam GetValue
 * @tparam GetItems
 * @tparam Gamma
 * @tparam GetCopiesNum
 */
template<
   class Bidders,
   class Items,
   class GetBids,
   class GetValue,
   class GetItems,
   class Gamma,
   class GetCopiesNum = paal::utils::return_one_functor
>
auto make_test_xor_bids_to_gamma_oracle_auction(
   Bidders&& bidders,
   Items&& items,
   GetBids get_bids,
   GetValue get_value,
   GetItems get_items,
   Gamma gamma,
   GetCopiesNum get_copies_num = GetCopiesNum{}
) ->
decltype(paal::auctions::make_gamma_oracle_auction_components(
   std::forward<Bidders>(bidders),
   std::forward<Items>(items),
   detail::test_xor_bids_gamma_oracle<GetBids, GetValue, GetItems, Gamma>(
      get_bids, get_value, get_items, gamma
   ),
   gamma,
   get_copies_num
))
{
   return paal::auctions::make_gamma_oracle_auction_components(
      std::forward<Bidders>(bidders),
      std::forward<Items>(items),
      detail::test_xor_bids_gamma_oracle<GetBids, GetValue, GetItems, Gamma>(
         get_bids, get_value, get_items, gamma
      ),
      gamma,
      get_copies_num
   );
}

#endif // PAAL_TEST_GAMMA_ORACLE_XOR_BIDS_HPP
