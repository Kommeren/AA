//=======================================================================
// Copyright (c) 2013 Robert Rosolek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file winner_determination_in_MUCA_long_test.hpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-06-18
 */

using Bidder = int;
using Item = int;
using Items = std::vector<Item>;
using Value = long double;
using Bid = std::pair<Items, Value>;
using Bids = std::vector<Bid>;

const std::string test_dir = "test/data/MUCA/";
const std::string cases_file = "cases.txt";

namespace {
   std::tuple<Value, std::vector<Bids>, std::vector<int>, int, int>
   read_auction(
      const std::string& test_name,
      const std::string& solution_file_extension = ".sol"
   ) {
      std::string path = test_dir + "cases/" + test_name;
      std::ifstream data_file(path + ".txt");
      assert(data_file.good());
      std::ifstream solution_file(path + solution_file_extension);
      assert(solution_file.good());

      Value sol;
      solution_file >> sol;

      int bidders_num, items_num, bids_num;
      data_file >> bidders_num >> items_num >> bids_num;
      std::vector<Bids> bids(bidders_num);
      while (bids_num--) {
         Bidder bidder;
         int items_num;
         Value value;
         data_file >> bidder >> items_num >> value;
         Items items(items_num);
         for (auto& item: items) data_file >> item;
         bids[bidder].emplace_back(std::move(items), value);
      }
      std::vector<int> item_count(items_num);
      for (auto& cnt: item_count) data_file >> cnt;

      return std::make_tuple(sol, bids, item_count, bidders_num, items_num);
   }
}
