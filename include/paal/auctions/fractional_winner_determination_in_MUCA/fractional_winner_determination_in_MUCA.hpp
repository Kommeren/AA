//=======================================================================
// Copyright (c) 2013 Robert Rosolek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file fractional_winner_determination_in_MUCA.hpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-06-09
 */

#ifndef  fractional_winner_determination_in_MUCA_INC
#define  fractional_winner_determination_in_MUCA_INC
#include "paal/auctions/auction_components.hpp"
#include "paal/auctions/auction_traits.hpp"
#include "paal/auctions/auction_utils.hpp"
#include "paal/lp/glp.hpp"
#include "paal/utils/concepts.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/property_map.hpp"
#include "paal/utils/rotate.hpp"

#include <boost/concept/requires.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/range/combine.hpp>

#include <iterator>
#include <random>
#include <tuple>
#include <type_traits>

namespace paal {
namespace auctions {

namespace detail {

   template <class Bidder, class BidId, class Bundle>
   struct bid {
      Bidder m_bidder;
      BidId m_bid_id;
      Bundle m_bundle;
      bid(Bidder bidder, BidId bid_id, Bundle bundle) :
         m_bidder(bidder), m_bid_id(bid_id), m_bundle(bundle) {}
   };

   template <class TryAddViolated, class SolveLp>
   void row_generation(TryAddViolated try_add_violated, SolveLp solve_lp)
   {
      do solve_lp(); while (try_add_violated());
   }

   template<
      class GetCandidates,
      class HowViolated,
      class AddViolated,
      class CompareHow
   >
   class add_max_violated {
      GetCandidates m_get_candidates;
      HowViolated m_how_violated;
      AddViolated m_add_violated;
      CompareHow m_cmp;

      public:
         add_max_violated(GetCandidates get_candidates,
               HowViolated how_violated, AddViolated add_violated)
            : m_get_candidates(get_candidates), m_how_violated(how_violated),
               m_add_violated(add_violated) {}

         bool operator()() {
            auto&& cands = m_get_candidates();
            using how_violated_t = puretype(m_is_violated(*std::begin(cands)));
            using cand_it_t = puretype(std::begin(cands));
            boost::optional<std::pair<how_violated_t, cand_it_t>> most;
            for (auto cand = std::begin(cands); cand != std::end(cands); ++cand)
            {
               const auto how = m_how_violated(*cand);
               if (!how) continue;
               if (!most || cmp(most->first, how))
                  most = std::make_pair(std::move(how), cand);
            }
            if (!most) return false;
            m_add_violated(*most->second);
            return true;
         }
   };

   struct max_violated_separation_oracle {
      template <
         class GetCandidates,
         class IsViolated,
         class AddViolated,
         class CompareHow = utils::less
      >
      auto operator()(
         GetCandidates get_candidates,
         IsViolated is_violated,
         AddViolated add_violated,
         CompareHow compare_how = CompareHow{}
      ) const {
         return add_max_violated<GetCandidates, IsViolated, AddViolated,
            CompareHow>(get_candidates, is_violated, add_violated, compare_how);
      }
   };

   template <class GetCandidates, class TryAddViolated, class ReorderCandidates>
   class add_first_violated {
      GetCandidates m_get_candidates;
      TryAddViolated m_try_add_violated;
      ReorderCandidates m_reorder_candidates;

      public:
         add_first_violated(
            GetCandidates get_candidates,
            TryAddViolated try_add_violated,
            ReorderCandidates reorder_candidates
         ) : m_get_candidates(get_candidates),
            m_try_add_violated(try_add_violated),
            m_reorder_candidates(std::move(reorder_candidates)) {}

         bool operator()() {
            auto&& cands = m_get_candidates();
            auto reordered =
               m_reorder_candidates(std::forward<decltype(cands)>(cands));
            for (auto c = std::begin(reordered); c != std::end(reordered); ++c)
               if (m_try_add_violated(*c)) return true;
            return false;
         }
   };

   struct first_violated_separation_oracle {
      template <
         class GetCandidates,
         class TryAddViolated,
         class ReorderCandidates = utils::identity_functor
      >
      auto operator() (
         GetCandidates get_candidates,
         TryAddViolated try_add_violated,
         ReorderCandidates reorder_candidates = ReorderCandidates{}
      ) const {
         return add_first_violated<GetCandidates, TryAddViolated,
            ReorderCandidates>(get_candidates, try_add_violated,
               reorder_candidates);
      }
   };

   template <class URNG>
   class random_rotate {
      URNG m_g;
      public:
         random_rotate(URNG&& g)
            : m_g(std::forward<URNG>(g)) {}
         template <class ForwardRange>
         auto operator()(const ForwardRange& rng)
         {
            const auto len = boost::distance(rng);
            std::uniform_int_distribution<decltype(len)> d(0, len);
            return utils::rotate(rng, d(m_g));
         }
   };

   template <class URNG = std::default_random_engine>
   auto make_random_rotate(URNG&& g = URNG{})
   {
      return random_rotate<URNG>(std::forward<URNG>(g));
   }

   struct random_violated_separation_oracle {
      template <
         class GetCandidates,
         class TryAddViolated,
         class URNG = std::default_random_engine
      >
      auto operator() (
         GetCandidates get_candidates,
         TryAddViolated try_add_violated,
         URNG&& g = URNG{}
      ) const {
         return first_violated_separation_oracle{}(get_candidates,
               try_add_violated, make_random_rotate(std::forward<URNG>(g)));
      }
   };

}//!detail

/**
 * @brief This is fractional determine winners in demand query auction and return
 * assignment of fractional bundles to bidders.
 *
 * Example:
 *  \snippet fractional_winner_determination_in_MUCA_example.cpp
 *
 * Complete example is fractional_winner_determination_in_MUCA_example.cpp
 *
 * @tparam DemandQueryAuction
 * @tparam OutputIterator
 * @tparam ItemToLpIdMap
 * @tparam SeparationOracle
 * @param auction
 * @param result
 * @param item_to_id Stores the current mapping of items to LP column ids.
 * @param epsilon Used for floating point comparison.
 * @param separation_oracle Separation Oracle Strategy for searching the
 *  bidder with violated inequality.
 */
template <
   class DemandQueryAuction,
   class OutputIterator,
   class ItemToLpIdMap,
   class SeparationOracle = detail::random_violated_separation_oracle
>
BOOST_CONCEPT_REQUIRES(

   ((concepts::demand_query_auction<DemandQueryAuction>))

   ((boost::ForwardRangeConcept<
     typename demand_query_auction_traits<DemandQueryAuction>::bidders_universe_t
   >))

   ((boost::ForwardRangeConcept<
     typename demand_query_auction_traits<DemandQueryAuction>::items_t
   >))

   ((utils::concepts::move_constructible<
     typename demand_query_auction_traits<DemandQueryAuction>::items_t
   >))

   ((utils::concepts::output_iterator<
     OutputIterator,
     std::tuple<
         typename demand_query_auction_traits<DemandQueryAuction>::bidder_t,
         typename demand_query_auction_traits<DemandQueryAuction>::items_t,
         double
      >
   >))

   ((boost::ReadWritePropertyMapConcept<
      ItemToLpIdMap,
      typename demand_query_auction_traits<DemandQueryAuction>::item_t
   >)),

   // TODO concept check for SeparationOracle

(void))
fractional_determine_winners_in_demand_query_auction(
   DemandQueryAuction&& auction,
   OutputIterator result,
   ItemToLpIdMap item_to_id,
   double epsilon,
   SeparationOracle separation_oracle = SeparationOracle{}
) {
   using traits_t = demand_query_auction_traits<DemandQueryAuction>;
   using bundle_t = typename traits_t::items_t;
   using bidder_t = typename traits_t::bidder_t;
   using bid_t = detail::bid<bidder_t, lp::row_id, bundle_t>;

   lp::glp dual;
   dual.set_optimization_type(lp::MINIMIZE);

   // add items variables to the dual
   auto&& items_ = auction.template get<items>();
   for (auto item = std::begin(items_); item != std::end(items_); ++item) {
      const auto copies = auction.template call<get_copies_num>(*item);
      const auto id = dual.add_column(copies, 0, lp::lp_traits::PLUS_INF, "");
      put(item_to_id, *item, id);
   }

   // add bidders variables to the dual
   // TODO allow to change the allocator
   std::vector<lp::col_id> bidder_to_id(bidders_number(auction));
   for (auto& id: bidder_to_id)
      id = dual.add_column(1, 0, lp::lp_traits::PLUS_INF, "");

   // TODO allow to change the allocator
   std::vector<bid_t> generated_bids;

   auto item_to_id_func = utils::make_property_map_get(item_to_id);
   auto get_price = utils::compose(
      [&](lp::col_id id) { return dual.get_col_value(id); },
      item_to_id_func
   );

   auto try_add_violated =
      utils::make_tuple_uncurry([&](bidder_t bidder, lp::col_id bidder_id)
   {
      //check if there is a violated constraint for bidder
      auto res = auction.template call<demand_query>(bidder, get_price);
      auto& items = res.first;
      const auto util = res.second;
      if (util <= dual.get_col_value(bidder_id) + epsilon)
         return false;

      // add violated constraint
      const auto price = utils::sum_functor(items, get_price);
      const auto value = util + price;
      const auto expr = utils::accumulate_functor(items,
            lp::linear_expression(bidder_id), item_to_id_func);
      const auto bid_id = dual.add_row(expr >= value);
      generated_bids.emplace_back(bidder, bid_id, std::move(items));
      return true;
   });

   auto get_candidates = utils::make_dynamic_return_something_functor(
      boost::combine(auction.template get<bidders>(), bidder_to_id));

   // TODO check if max_violated strategy doesn't give better performance
   auto find_violated = separation_oracle(get_candidates, try_add_violated);

   auto solve_lp = [&]()
   {
      const auto res = dual.resolve_simplex(lp::DUAL);
      assert(res == lp::OPTIMAL);
   };

   detail::row_generation(find_violated, solve_lp);

   // emit results
   for (auto& bid: generated_bids) {
      const auto fraction = dual.get_row_dual_value(bid.m_bid_id);
      if (fraction <= epsilon) continue;
      *result = std::make_tuple(std::move(bid.m_bidder),
            std::move(bid.m_bundle), fraction);
      ++result;
   }
}

/**
 * @brief This is fractional determine winners in demand query auction and return
 * assignment of fractional bundles to bidders.
 * This is version with default ItemToLpIdMap using std::unordered_map and
 * default epsilon.
 *
 * @tparam DemandQueryAuction
 * @tparam OutputIterator
 * @param auction
 * @param result
 * @param epsilon Used for floating point comparison.
 */
template <class DemandQueryAuction, class OutputIterator>
void fractional_determine_winners_in_demand_query_auction(
   DemandQueryAuction&& auction,
   OutputIterator result,
   double epsilon = 1e-7
) {
   using traits_t = demand_query_auction_traits<DemandQueryAuction>;
   using ItemVal = typename traits_t::item_val_t;

   std::unordered_map<ItemVal, lp::col_id> map;
   return fractional_determine_winners_in_demand_query_auction(
      std::forward<DemandQueryAuction>(auction),
      result,
      boost::make_assoc_property_map(map),
      epsilon
   );
}

}//!auctions
}//!paal

#endif   /* fractional_winner_determination_in_MUCA_INC */
