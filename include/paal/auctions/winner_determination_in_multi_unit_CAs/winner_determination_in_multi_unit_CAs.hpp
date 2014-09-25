//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/*
 * @file winner_determination_in_multi_unit_CAs.hpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-1-7
 */
#ifndef WINNER_DETERMINATION_IN_MUCAS
#define WINNER_DETERMINATION_IN_MUCAS

#include "paal/auctions/auction_traits.hpp"
#include "paal/auctions/auction_utils.hpp"
#include "paal/utils/type_functions.hpp"

#include <boost/optional/optional.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/iterator.hpp>

#include <iterator>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace paal {
namespace auctions {

namespace detail {
   template <class Value, class ItemVals>
   struct bidder_attributes {
      Value m_best_items_value;
      ItemVals m_best_items;
   };

   template <class GammaOracleAuctionComponents>
   struct determine_winners_in_gamma_oracle_auction_traits {
      using traits = gamma_oracle_auction_traits<GammaOracleAuctionComponents>;
      using value = promote_with_double_t<typename traits::value_t>;
      using item_val = typename traits::item_val_t;
      using item_set = std::vector<item_val>; // TODO allow for different container
      using bidder_attributes = detail::bidder_attributes<value, item_set>;
   };

   // TODO use lambda with auto parameter instead of this when C++14 arrives
   template <class PriceMap>
   struct price_functor {
      price_functor(PriceMap price) : m_price(price) {}

      PriceMap m_price;

      template <class Item>
      auto operator()(Item&& item) const -> decltype(get(m_price, std::forward<Item>(item)))
      {
         return get(m_price, std::forward<Item>(item));
      }
   };
}//!detail

/**
 * @brief This is determine winners in gamma oracle auction and return assignment of bidders to items.
 *
 * Example:
 *  \snippet winner_determination_in_multi_unit_CAs_example.cpp Winner Determination In Multi Unit CAs Example
 *
 * Complete example is winner_determination_in_multi_unit_CAs_example.cpp.
 *
 * @tparam GammaOracleAuctionComponents
 * @tparam OutputIterator
 * @tparam PriceMap Should implement to Boost.PropertyMap interface. TODO add boost concept check instead
 * @tparam Epsilon
 * @param auction
 * @param result
 * @param price Stores the current assignment of prices to items. These are prices just for the working
 * purposes of the algorithm, not the prices to be paid by the bidders.
 * @param epsilon Used for floating point comparison to ensure feasibility.
 */
template<
   class GammaOracleAuctionComponents,
   class OutputIterator,
   class PriceMap,
   class Epsilon
>
void determine_winners_in_gamma_oracle_auction(
   GammaOracleAuctionComponents&& auction,
   OutputIterator result,
   PriceMap price,
   Epsilon epsilon
) {
   // TODO add concepts and static asserts to validate types given by the user
   using Price = typename boost::property_traits<PriceMap>::value_type;

   using DetermineWinnerTraits =
      detail::determine_winners_in_gamma_oracle_auction_traits<GammaOracleAuctionComponents>;
   using Value = typename DetermineWinnerTraits::value;
   using BidderAttributes = typename DetermineWinnerTraits::bidder_attributes;
   using ItemSet = typename DetermineWinnerTraits::item_set;

   using Traits = gamma_oracle_auction_traits<GammaOracleAuctionComponents>;
   using BidderIterator = typename Traits::bidder_iterator_t;
   using Frac = typename Traits::frac_t;

   for (auto item = std::begin(auction.template get<items>());
         item != std::end(auction.template get<items>());
         ++item
   ) {
      auto copies_num = auction.template call<get_copies_num>(*item);
      put(price, *item, 1.0 / copies_num);
   }
   const auto items_num = items_number(auction);
   Price price_sum = items_num;

   // TODO allow to change the allocator
   std::vector<BidderAttributes> bidders_attributes_vec(bidders_number(auction));

   auto last_assigned_bidder_attributes = bidders_attributes_vec.end();
   BidderIterator last_assigned_bidder;
   Value total_value = 0, last_value{};
   const auto b = get_minimum_copies_num(auction);
   const auto multiplier = std::exp(Value(b) + 1) * items_num;
   const auto gamma_ = auction.template get<gamma>();
   auto get_threshold = [=](const BidderAttributes& b) { return (1 + 2 * gamma_) * b.m_best_items_value; };
   // we use one buffer throughout the whole algorithm instead of
   // creating it from scratch each time - this is as an optimization
   // for the default case of ItemSet = std::vector.
   ItemSet cur_items{};
   do {
      boost::optional<std::pair<Frac, ItemSet>> best;
      auto bidder_attributes = bidders_attributes_vec.begin();
      auto bidder = std::begin(auction.template get<bidders>());
      for (; bidder_attributes != bidders_attributes_vec.end(); ++bidder_attributes, ++bidder) {
         cur_items.clear();
         auto threshold = get_threshold(*bidder_attributes);
         auto result = auction.template call<gamma_oracle>(
            *bidder,
            detail::price_functor<PriceMap>(price),
            threshold,
            // TODO this output iterator should be available for change as well,
            // maybe it's enough to get rid of std?
            std::back_inserter(cur_items)
         );
         if (!result) continue;
         if (!best || *result < best->first) {
            best = std::make_pair(*result, std::move(cur_items));
            last_assigned_bidder_attributes = bidder_attributes;
            last_assigned_bidder = bidder;
            last_value = result->den + threshold;
         }
      }
      if (!best) break;
      auto& best_items = best->second;
      for (auto item = std::begin(best_items); item != std::end(best_items); ++item) {
         auto copies_num = auction.template call<get_copies_num>(*item);
         auto old_price = get(price, *item);
         auto new_price = old_price * std::pow(multiplier, 1.0 / (copies_num + 1));
         put(price, *item, new_price);
         price_sum += copies_num * (new_price - old_price);
      }
      total_value += last_value - last_assigned_bidder_attributes->m_best_items_value;
      last_assigned_bidder_attributes->m_best_items = std::move(best_items);
      last_assigned_bidder_attributes->m_best_items_value = last_value;
   } while (price_sum + epsilon < multiplier);

   bool nothing_assigned = last_assigned_bidder_attributes == bidders_attributes_vec.end();
   if (nothing_assigned) return;

   auto output = [&](puretype(last_assigned_bidder_attributes) bidder_attributes, BidderIterator bidder)
   {
      const auto& items = bidder_attributes->m_best_items;
      boost::transform(
         items,
         result,
         [=](decltype(*std::begin(items)) item)
         {
            return std::make_pair(*bidder, std::forward<decltype(item)>(item));
         }
      );
   };
   if (last_value > total_value - last_value) {
      output(last_assigned_bidder_attributes, last_assigned_bidder);
      return;
   }
   auto bidder_attributes = bidders_attributes_vec.begin();
   auto bidder = std::begin(auction.template get<bidders>());
   for (; bidder_attributes != bidders_attributes_vec.end(); ++bidder_attributes, ++bidder)
      if (bidder != last_assigned_bidder)
         output(bidder_attributes, bidder);
}

/**
 * @brief This is determine winners in gamma oracle auction and return assignment of bidders to items.
 * This is version with default PriceMap using std::unordered_map and default epsilon = 1e-8.
 *
 * @tparam GammaOracleAuctionComponents
 * @tparam OutputIterator
 * @tparam Epsilon
 * @param auction
 * @param result
 * @param epsilon Used for floating point comparison to ensure feasibility.
 */
template<class GammaOracleAuctionComponents, class OutputIterator, class Epsilon = double>
void determine_winners_in_gamma_oracle_auction(
   GammaOracleAuctionComponents&& auction,
   OutputIterator result,
   Epsilon epsilon = 1e-8
) {
   using Traits = gamma_oracle_auction_traits<GammaOracleAuctionComponents>;
   using Value = promote_with_double_t<typename Traits::value_t>;
   using ItemVal = typename Traits::item_val_t;

   std::unordered_map<ItemVal, Value> map;
   return determine_winners_in_gamma_oracle_auction(
      std::forward<GammaOracleAuctionComponents>(auction),
      result,
      boost::make_assoc_property_map(map),
      epsilon
   );
}


}//!auctions
}//!paal

#endif /* WINNER_DETERMINATION_IN_MUCAS */
