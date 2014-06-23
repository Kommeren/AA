/**
 * @file read_knapsack.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-08
 */
#ifndef READ_KNAPSACK_HPP
#define READ_KNAPSACK_HPP

#include "utils/parse_file.hpp"

#include <cassert>
#include <vector>

inline void read(const std::string &testDir, int testId, int &capacity,
                 std::vector<int> &weights, std::vector<int> &values,
                 std::vector<int> &optimal) {
    auto filePrefix = "p0" + std::to_string(testId);
    std::string fname = filePrefix + "_c.txt";

    // read capacity
    {
        std::ifstream ifs(testDir + fname);
        assert(ifs.good());
        ifs >> capacity;
        ifs.close();
    }

    // read weights
    fname = filePrefix + "_w.txt";
    paal::parse(testDir + fname, [&](const std::string & s, std::istream &) {
        int weight = std::stoi(s);
        assert(weight);
        weights.push_back(weight);
    });

    // read profits
    fname = filePrefix + "_p.txt";
    paal::parse(testDir + fname, [&](const std::string & s, std::istream &) {
        int val = std::stoi(s);
        assert(val);
        values.push_back(val);
    });
    assert(values.size() == weights.size());

    // read profits
    fname = filePrefix + "_s.txt";
    int idx(0);
    paal::parse(testDir + fname, [&](const std::string & s, std::istream &) {
        bool chosen = std::stoi(s);
        if (chosen) {
            optimal.push_back(idx);
        }
        ++idx;
    });
}

#endif /* READ_KNAPSACK_HPP */
