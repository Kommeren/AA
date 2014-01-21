/**
 * @file winner_determination_in_multi_unit_CAs_long_test.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-3-20
 */

#include "paal/utils/functors.hpp"
#include "utils/logger.hpp"
#include "utils/parse_file.hpp"
#include "utils/winner_determination_in_multi_unit_CAs.hpp"

#include <boost/range/irange.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/property_map/vector_property_map.hpp>

#include <fstream>
#include <iterator>
#include <random>
#include <string>
#include <utility>
#include <vector>

BOOST_AUTO_TEST_CASE(testDetermineWinnersLong)
{
   using Bidder = int;
   using Item = int;
   using Items = std::vector<Item>;
   using Value = long double;
   using Bid = std::pair<Items, Value>;
   using Bids = std::vector<Bid>;

   std::default_random_engine generator(7);
   std::uniform_real_distribution<Value> distribution(1.0, 2.0);

   std::string test_dir = "test/data/MUCA/";
   paal::parse(test_dir + "cases.txt", [&](std::string test_name, paal::utils::ignore_param) {
      std::string path = test_dir + "cases/" + test_name;
      std::ifstream data_file(path + ".txt");
      assert(data_file.good());
      std::ifstream solution_file(path + ".sol");
      assert(solution_file.good());

      Value lower_bound;
      solution_file >> lower_bound;

      int bidders_num, items_num, bids_num;
      data_file >> bidders_num >> items_num >> bids_num;
      std::vector<Bids> bids(bidders_num);
      while (bids_num--) {
         int bidder, items_num;
         Value value;
         data_file >> bidder >> items_num >> value;
         Items items(items_num);
         for (auto& item: items) data_file >> item;
         bids[bidder].emplace_back(std::move(items), value);
      }
      std::vector<int> item_count(items_num);
      for (auto& cnt: item_count) data_file >> cnt;

      auto bidders = boost::irange(0, bidders_num);
      auto items = boost::irange(0, items_num);
      auto get_bids = [&](Bidder bidder) -> const Bids& { return bids[bidder]; };
      auto get_value = [](const Bid& bid) { return bid.second; };
      auto get_items = [](const Bid& bid) -> const Items& { return bid.first; };
      auto get_quantity = paal::utils::make_array_to_functor(item_count);
      auto gamma = distribution(generator);
      // TODO move logs file to repo, it might be useful for someone someday
      check_determine_winners_in_gamma_oracle(
         bidders,
         items,
         get_bids,
         get_value,
         get_items,
         get_quantity,
         gamma,
         lower_bound,
         boost::vector_property_map<Value>(),
         1e-8
      );
   });
}
