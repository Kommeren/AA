/**
 * @file auction_traits.hpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-03-24
 */
#ifndef AUCTION_TRAITS_HPP
#define AUCTION_TRAITS_HPP

#include "paal/auctions/auction_components.hpp"
#include "paal/data_structures/fraction.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/type_functions.hpp"

#include <boost/function_output_iterator.hpp>
#include <boost/optional/optional.hpp>

#include <iterator>
#include <unordered_set>
#include <utility>

namespace paal {
namespace auctions {

/**
 * @brief Types associated with all auctions.
 *
 * @tparam AuctionComponents
 */
template <class AuctionComponents>
struct auction_traits {
   using bidders_t = decltype(std::declval<AuctionComponents>().template get<bidders>());
   using bidder_iterator_t = puretype(std::begin(std::declval<bidders_t>()));
   using bidder_t = range_to_ref_t<bidders_t>;
   using bidder_val_t = range_to_elem_t<bidders_t>;
   using items_t = decltype(std::declval<AuctionComponents>().template get<items>());
   using item_t = range_to_ref_t<items_t>;
   using item_val_t = range_to_elem_t<items_t>;
   using copies_num_t = puretype(
      std::declval<AuctionComponents>().template call<get_copies_num>(std::declval<item_t>())
   );
};

/**
 * @brief Types associated with value query auction.
 *
 * @tparam ValueQueryAuctionComponents
 */
template <class ValueQueryAuctionComponents>
class value_query_auction_traits: public auction_traits<ValueQueryAuctionComponents> {
   using super = auction_traits<ValueQueryAuctionComponents>;

   public:
      using value_t = puretype(std::declval<const ValueQueryAuctionComponents&>().template call<value_query>(
         std::declval<typename super::bidder_t>(),
         std::unordered_set<typename super::item_val_t>() // any container of items with count method
      ));
};

/**
 * @brief Types associated with gamma oracle auction.
 *
 * @tparam GammaOracleAuctionComponents
 */
template <class GammaOracleAuctionComponents>
struct gamma_oracle_auction_traits: auction_traits<GammaOracleAuctionComponents> {
   using value_t = puretype(
      std::declval<GammaOracleAuctionComponents>(). template call<gamma_oracle>(
         std::declval<typename auction_traits<GammaOracleAuctionComponents>::bidder_t>(),
         // this is a little tricky, in order to obtain the value type, we pass prices and threshold
         // as double types, because value type needs to be able to operate with doubles anyway
         utils::make_dynamic_return_something_functor(double(1.0)), // any functor with double operator()
         double(1.0), // any double
         boost::make_function_output_iterator(utils::skip_functor{}) // any item output iterator
      )->den
   );
   using frac_t = data_structures::fraction<value_t, value_t>;
   using gamma_oracle_result_t = boost::optional<frac_t>;
};

} //!auctions
} //!paal
#endif /* AUCTION_TRAITS_HPP */
