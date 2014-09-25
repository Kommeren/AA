//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file auction_components.hpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-01-07
 */
#ifndef AUCTION_COMPONENTS_HPP
#define AUCTION_COMPONENTS_HPP

#include "paal/data_structures/components/components.hpp"
#include "paal/data_structures/components/components_join.hpp"
#include "paal/utils/functors.hpp"

#include <utility>

namespace paal {
/// Auctions namespace
namespace auctions {

   // Base

   /**
    * @brief name for the bidders component
    */
   struct bidders;
   /**
    * @brief name for the items component
    */
   struct items;
   /**
    * @brief name for the get_copies_num component
    */
   struct get_copies_num;
   /**
    * @brief Definition for the components class representing an auction.
    * This class is not meant to be directly used, it is just a base for the
    * more specialized components interfaces.
    */
   using base_auction_components = data_structures::components<
      bidders,
      items,
      data_structures::NameWithDefault<get_copies_num, utils::return_one_functor>
   >;

   namespace detail {
      /// extend base auction components with other components.
      template <typename... Names>
      using add_to_base_auction =
         typename data_structures::join<
            base_auction_components,
            data_structures::components<Names...>
         >::type;
   }; //!detail

   // Value Query Auction

   /**
    * @brief name for the value_query component
    */
   struct value_query;
   /**
    * @brief Definition for the components class for a value query auction.
    */
   using value_query_components = detail::add_to_base_auction<value_query>;

   /**
    * @brief value query auction components template alias
    *
    * @tparam Args
    */
   template <typename... Args>
   using value_query_auction_components = typename value_query_components::type<Args...>;

   /**
    * @brief make function for value query components
    *
    * @tparam Args
    * @param args
    *
    * @return value query components
    */
   template <typename... Args>
   auto make_value_query_auction_components(Args&&... args) ->
      decltype(value_query_components::make_components(std::forward<Args>(args)...))
   {
      return value_query_components::make_components(std::forward<Args>(args)...);
   }

   // Gamma Oracle Auction

   /**
    * @brief name for the gamma_oracle component
    */
   struct gamma_oracle;
   /**
    * @brief name for the gamma component
    */
   struct gamma;
   /**
    * @brief Definition for the components class for a gamma oracle auction.
    */
   using gamma_oracle_components = detail::add_to_base_auction<gamma_oracle, gamma>;

   /**
    * @brief gamma oracle auction components template alias
    *
    * @tparam Args
    */
   template <typename... Args>
   using gamma_oracle_auction_components = typename gamma_oracle_components::type<Args...>;

   /**
    * @brief make function for gamma oracle components
    *
    * @tparam Args
    * @param args
    *
    * @return gamma oracle components
    */
   template <typename... Args>
   auto make_gamma_oracle_auction_components(Args&&... args) ->
      decltype(gamma_oracle_components::make_components(std::forward<Args>(args)...))
   {
      return gamma_oracle_components::make_components(std::forward<Args>(args)...);
   }

} //!auctions
} //!paal
#endif // AUCTION_COMPONENTS_HPP
