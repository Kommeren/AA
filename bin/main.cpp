/*
 *  * main.cpp
 *  *
 *  *  Created on: 03-01-2013
 *  *      Author: Piotr Wygocki
 *  */

#include <iostream>
#include <iterator>

#include "local_search.hpp"
#include "trivial_neighbour.hpp"
#include "cycle_manager.hpp"


int main() {

    std::vector<int> v = {1,2,3,4,5};
    auto vExl = TrivialNeigbourGetter<typename std::vector<int>::iterator, int>().getNeighbourhood(v.begin(), v.end(), 4); 
    std::cout << (vExl.first == vExl.second) << std::endl << std::flush;
    auto f = std::function<bool(int)>(std::bind(std::equal_to<int>(), 4, std::placeholders::_1));
    f(2);
    std::cout << "t1" << std::endl << std::flush;
    std::copy(vExl.first, vExl.second, std::ostream_iterator<int>(std::cout, "\n"));

    CycleManager<int> cm(vExl.first, vExl.second);

}
