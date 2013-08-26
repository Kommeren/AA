/**
 * @file shortest_superstring_example.cpp
 * @brief 
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-29
 */

//! [Shortest Superstring Example]
#include <iostream>
#include <string>
#include "paal/greedy/shortest_superstring/shortest_superstring.hpp"


using namespace paal::greedy::shortest_superstring;
using namespace std;
/**
 * @brief show how to use shortest_superstring
 */
int main() {
    vector<string> words({"ba","ab","aa","bb"});
    
    
    cout<<shortestSuperstring(words)<<endl;
    
 }
//! [Shortest Superstring Example]
