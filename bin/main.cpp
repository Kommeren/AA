/*
 *  * main.cpp
 *  *
 *  *  Created on: 03-01-2013
 *  *      Author: Piotr Wygocki
 *  */

#include <iostream>
#include <iterator>

#include "local_search/2_local_search/2_local_search.hpp"

std::ostream& operator<< (std::ostream &o, const std::pair<int, int> &p) {
    return o << p.first << ',' << p.second;
}

int main() {

    std::vector<int> v = {1,2,3,4,5, 6, 7, 8, 9, 10};
    auto vExl = TrivialNeigbourGetter().getNeighbourhood(v.begin(), v.end(), 4); 
    std::cout << (vExl.first == vExl.second) << std::endl << std::flush;
    auto f = std::function<bool(int)>(std::bind(std::equal_to<int>(), 4, std::placeholders::_1));
    f(2);
    std::cout << "t1" << std::endl << std::flush;
    std::copy(vExl.first, vExl.second, std::ostream_iterator<int>(std::cout, "\n"));

    SimpleCycleManager<int> cm(vExl.first, vExl.second);
    auto r = cm.getEdgeRange(2);
    for(;r.first != r.second; ++r.first) {
        std::cout << *r.first << std::endl;
    }
    std::cout << "xxx" << std::endl;

    typedef std::pair<int, int> PT;
    PT p1(1,2);
    PT p2(7,8);
    cm.swapEnds(p1, p2);
    
    r = cm.getEdgeRange(2);
    
    
    for(;r.first != r.second; ++r.first) {
        std::cout << *r.first << std::endl;
    }
    std::cout << "xxx" << std::endl;

}
