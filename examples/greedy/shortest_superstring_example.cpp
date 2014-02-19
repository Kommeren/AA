/**
 * @file shortest_superstring_example.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-29
 */

#include <iostream>
#include <string>
#include "paal/greedy/shortest_superstring/shortest_superstring.hpp"


/**
 * @brief show how to use shortest_superstring
 */
int main() {
//! [Shortest Superstring Example]
    std::vector<std::string> words({"ba","ab","aa","bb"});


    std::cout<<paal::greedy::shortestSuperstring(words)<<std::endl;
//! [Shortest Superstring Example]
}
