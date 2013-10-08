/**
 * @file read_knapsack.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-08
 */
#ifndef READ_KNAPSACK_HPP
#define READ_KNAPSACK_HPP 

namespace {
void read(const std::string & testDir, 
          int testId, 
          int & capacity, 
          std::vector<int> & weights, 
          std::vector<int> & values, 
          std::vector<int> & optimal) {
    auto filePrefix = "p0" + std::to_string(testId);

    //read capacity
    std::string fname = filePrefix + "_c.txt";
    std::ifstream ifs(testDir + fname);
    assert(ifs.good());
    ifs >> capacity;
    ifs.close();
    
    //read weights
    fname = filePrefix + "_w.txt";
    ifs.open(testDir + fname);
    assert(ifs.good());
    while(ifs.good()) {
        std::string s;
        ifs >> s;
        if(s == "")
            break;
        int weight = std::stoi(s);
        assert(weight);
        weights.push_back(weight);
    }
    ifs.close();
    
    //read profits
    fname = filePrefix + "_p.txt";
    ifs.open(testDir + fname);
    assert(ifs.good());
    while(ifs.good()) {
        std::string s;
        ifs >> s;
        if(s == "")
            break;
        int val = std::stoi(s);
        assert(val);
        values.push_back(val);
    }
    assert(values.size() == weights.size());
    ifs.close();
    
    //read profits
    fname = filePrefix + "_s.txt";
    ifs.open(testDir + fname);
    assert(ifs.good());
    int idx(0);
    while(ifs.good()) {
        std::string s;
        ifs >> s;
        if(s == "")
            break;
        bool chosen = std::stoi(s);
        if(chosen) {
            optimal.push_back(idx);
        }
        ++idx;
    }
    ifs.close();
}

}
#endif /* READ_KNAPSACK_HPP */
