/**
 * @file gamma_oracle_example.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-6-6
 */

#include "paal/auctions/auction_components.hpp"
#include "paal/data_structures/fraction.hpp"

#include <boost/optional/optional.hpp>
#include <boost/range/algorithm/copy.hpp>

#include <cassert>
#include <vector>

//! [Gamma Oracle Auction Components Example]

namespace pa = paal::auctions;
namespace pds = paal::data_structures;
using Bidder = std::string;
using Item = std::string;
using Value = int;
using Frac = pds::fraction<Value, Value>;

const std::vector<Bidder> bidders {"Pooh Bear", "Rabbit"};

const std::vector<Item> items {"honey", "baby carrot", "carrot", "jam"};

const int gamma_val = 2;

struct gamma_oracle_func {
      template <class GetPrice, class Threshold, class OutputIterator>
      boost::optional<Frac>
      operator()(
         Bidder bidder,
         GetPrice get_price,
         Threshold z,
         OutputIterator result_items
      ) const
      {
         if (bidder == "Pooh Bear") {
            const int honey_val = 10;
            if (honey_val <= z) return boost::none;
            *result_items = "honey";
            return Frac(get_price("honey"), honey_val - z);
         }

         assert(bidder == "Rabbit");

         const int baby_val = 2, val = 3;
         auto baby_carrot_price = get_price("baby carrot"), carrot_price = get_price("carrot");
         auto baby_carrot_frac = Frac(baby_carrot_price, baby_val - z),
            carrot_frac = Frac(carrot_price, val - z),
            both_carrots_frac = Frac(baby_carrot_price + carrot_price, baby_val + val - z);

         auto check = [=](Frac candidate, Frac other1, Frac other2) {
            if (candidate.den <= 0) return false;
            auto check_single = [=](Frac candidate, Frac other) {
               return other.den <= 0 || candidate <= gamma_val * other;
            };
            return check_single(candidate, other1) && check_single(candidate, other2);
         };

         if (check(baby_carrot_frac, carrot_frac, both_carrots_frac)) {
            *result_items = "baby carrot";
            return baby_carrot_frac;
         }
         if (check(carrot_frac, baby_carrot_frac, both_carrots_frac)) {
            *result_items = "carrot";
            return carrot_frac;
         }
         if (check(both_carrots_frac, baby_carrot_frac, carrot_frac)) {
            boost::copy(std::vector<Item>{"baby carrot", "carrot"}, result_items);
            return both_carrots_frac;
         }
         return boost::none;
      }
};

//! [Gamma Oracle Auction Components Example]

int main()
{
   //! [Gamma Oracle Auction Create Example]
   const auto auction = pa::make_gamma_oracle_auction_components(
      bidders, items, gamma_oracle_func(), gamma_val
   );
   //! [Gamma Oracle Auction Create Example]

   //! [Gamma Oracle Auction Use Example]
   auto get_price_func = [](Item item) { return item == "honey" ? 5 : 2; };

   std::cout << "pooh bear buys: ";
   auto got_pooh_bear = auction.call<pa::gamma_oracle>(
      "Pooh Bear", get_price_func, 10, std::ostream_iterator<Item>(std::cout, ", ")
   );
   if (!got_pooh_bear) std::cout << "nothing";
   std::cout << std::endl;

   std::cout << "rabbit oracle buys: ";
   auto got_rabbit = auction.call<pa::gamma_oracle>(
      "Rabbit", get_price_func, 1, std::ostream_iterator<Item>(std::cout, ", ")
   );
   if (!got_rabbit) std::cout << "nothing";
   std::cout << std::endl;

   //! [Gamma Oracle Auction Use Example]
   return 0;
}
