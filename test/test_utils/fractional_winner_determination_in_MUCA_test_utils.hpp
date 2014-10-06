//=======================================================================
// Copyright (c) 2013 Robert Rosolek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file fractional_winner_determination_in_MUCA_test_utils.hpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-06-12
 */
#ifndef PAAL_FRACTIONAL_WINNER_DETERMINATION_IN_MUCA_TEST_UTILS_HPP
#define PAAL_FRACTIONAL_WINNER_DETERMINATION_IN_MUCA_TEST_UTILS_HPP

#include "paal/auctions/auction_components.hpp"
#include "paal/auctions/auction_traits.hpp"
#include "paal/auctions/fractional_winner_determination_in_MUCA/fractional_winner_determination_in_MUCA.hpp"
#include "paal/auctions/xor_bids.hpp"
#include "paal/lp/glp.hpp"
#include "paal/utils/make.hpp"
#include "paal/utils/type_functions.hpp"

#include <boost/function_output_iterator.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/test/unit_test.hpp>

#include <iterator>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>

template <
   class Bidders,
   class Items,
   class GetBids,
   class GetValue,
   class GetItems,
   class GetCopiesNum,
   class ItemToLpIdMap
>
void check_fractional_determine_winners_in_demand_query_auction(
   Bidders&& bidders,
   Items&& items,
   GetBids get_bids,
   GetValue get_value,
   GetItems get_items,
   GetCopiesNum get_copies_num,
   double opt,
   ItemToLpIdMap item_to_id,
   double eps
) {

    namespace pa = paal::auctions;

    auto auction = pa::make_xor_bids_to_demand_query_auction(
            std::forward<Bidders>(bidders),
            std::forward<Items>(items),
            get_bids,
            get_value,
            get_items,
            get_copies_num
            );


    using Traits = pa::demand_query_auction_traits<decltype(auction)>;
    using Bidder = typename Traits::bidder_val_t;
    using Item = typename Traits::item_val_t;
    using ItemsBundle = typename Traits::items_t;
    using Assignment = std::tuple<Bidder, ItemsBundle, double>;

    auto valuation = pa::make_xor_bids_to_value_query_auction(
            // TODO these arguments are copy paste, maybe we need xor_bids_auction_components?
            std::forward<Bidders>(bidders),
            std::forward<Items>(items),
            get_bids,
            get_value,
            get_items,
            get_copies_num
            );

    double social_welfare = 0;
    std::unordered_map<Bidder, double> bidder_count;
    std::unordered_map<Item, double> item_count;
    pa::fractional_determine_winners_in_demand_query_auction(
            auction,
            boost::iterators::make_function_output_iterator([&](Assignment a)
            {
                auto bidder = std::get<0>(a);
                auto& items = std::get<1>(a);
                auto frac = std::get<2>(a);
                auto item_set = paal::make_unordered_set(items);
                social_welfare += frac * valuation.template call<pa::value_query>(bidder, item_set);
                for (auto const & item: items) {
                    auto cnt = item_count[item] += frac;
                    BOOST_CHECK_LE(cnt, get_copies_num(item) + eps);
                }
                auto cnt = bidder_count[bidder] += frac;
                BOOST_CHECK_LE(cnt, 1 + eps);
            }),
            item_to_id,
            eps
            );

    BOOST_CHECK_CLOSE(opt, social_welfare, eps);
}

template<
class Bidders,
      class Items,
      class GetBids,
      class GetValue,
      class GetItems,
      class GetCopiesNum
      >
void check_fractional_determine_winners_in_demand_query_auction(
        Bidders&& bidders,
        Items&& items,
        GetBids get_bids,
        GetValue get_value,
        GetItems get_items,
        GetCopiesNum get_copies_num,
        double opt,
        double epsilon = 1e-8
        ) {
    using ItemVal = paal::range_to_elem_t<Items>;
    using PriceMap = std::unordered_map<ItemVal, paal::lp::col_id>;

    PriceMap map;
    check_fractional_determine_winners_in_demand_query_auction(
            std::forward<Bidders>(bidders),
            std::forward<Items>(items),
            get_bids,
            get_value,
            get_items,
            get_copies_num,
            opt,
            boost::make_assoc_property_map(map),
            epsilon
            );
}

#endif /* PAAL_FRACTIONAL_WINNER_DETERMINATION_IN_MUCA_TEST_UTILS_HPP */
