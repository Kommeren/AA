/**
 * @file lcp.hpp
 * @brief 
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-29
 */
#ifndef LCP_HPP
#define LCP_HPP


namespace paal{
namespace suffix_arrays{
/**
 * 
 * @brief
 * fill array lcp
 * lcp[0] is undefined
 * and
 * lcp[i] stores the largest common prefix of the lexicographically i-1'th smallest suffix 
 * and its predecessor in the suffix array
 * @tparam Letter
 * @param vector<int> suffixArray
 * @param vector<int> rank
 * @param vector<int> place for Lcp
 * @param vector<Letter> word
 */
template<typename Letter>
void lcp(std::vector<int> const& suffixArray,std::vector<int> const&rank,std::vector<int> &lcp,
         std::vector<Letter> const&sumWords){
    int comonPrefixLength=0;
    for(auto suffixRank: rank){//suffixRank number suffix in lexicographically order
        if(suffixRank!=0){
            while(sumWords[suffixArray[suffixRank]+comonPrefixLength]==sumWords[suffixArray[suffixRank-1]+comonPrefixLength]){
                ++comonPrefixLength;
            }
        }
        lcp[suffixRank]=comonPrefixLength;
        if(comonPrefixLength>0){
            --comonPrefixLength;
        }
    };
};
}//!suffix_arrays
}//!paal

#endif /*LCP*/