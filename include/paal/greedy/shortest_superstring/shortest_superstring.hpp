/**
 * @file shortest_superstring.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-29
 */
#ifndef SHORTEST_SUPERSTRING_HPP
#define SHORTEST_SUPERSTRING_HPP

#include<vector>
#include<algorithm>
#include <utility>
#include <type_traits>

#include <boost/range/adaptors.hpp>
#include "boost/range/irange.hpp"

#include "paal/suffix_array/lcp.hpp"
#include "paal/suffix_array/suffix_array.hpp"
#include "paal/utils/type_functions.hpp"
#include "paal/greedy/shortest_superstring/prefix_tree.hpp"
#include "paal/data_structures/bimap.hpp"


namespace paal{
namespace greedy{
namespace detail{


/**
 * @class ShortestSuperstring
 * @brief class to solve shortest superstring 3.5 aproximation,
 * using greedy algorithm:
 * contract pair of words with largest overlap until one word stays
    \snippet shortest_superstring_example.cpp Shortest Superstring Example
 *
 * complete example is shortest_superstring_example.cpp
 *
 * @tparam Words
 */
template<typename Words>
class ShortestSuperstring{
public:
    typedef typename utils::CollectionToElem<Words>::type Word;
    typedef typename utils::CollectionToElem<Word>::type Letter;


    ShortestSuperstring(const Words& words):
          m_length(countSumLenght(words)),
          m_prefixTree(m_length,m_suffixArray,m_sumWords,m_lcp,m_lengthSuffixWord) {

        initialize(words);

        suffix_arrays::suffixArray<Letter>(m_sumWords, m_suffixArray);

        data_structures::rank(m_suffixArray,m_rank);

        suffix_arrays::lcp(m_suffixArray,m_rank,m_lcp,m_sumWords);

        m_prefixTree.buildPrefixTree();

        m_prefixTree.fillSuffixToTree();

        joinAllWords();
    }


/**
 * @brief return word contains all words as subwords,
 * of lenght at most 3.5 larger than shortest superstring.
 *
    \snippet shortest_superstring_example.cpp Shortest Superstring Example
 *
 * complete example is shortest_superstring_example.cpp
 *
 */
    Word getSolution(){
        Word answer;
        for(auto posInSumWords: boost::irange(1,m_length)){
            if((!m_isJoinedSufiix[m_posToWord[posInSumWords]]) &&
                (m_sumWords[posInSumWords-1]==m_prefixTree.DELIMITER)){
                for(int nextLetter=posInSumWords;m_sumWords[nextLetter]!=m_prefixTree.DELIMITER;){
                    answer.push_back(m_sumWords[nextLetter]);
                    if(m_res[nextLetter]==NO_OVERLAP_STARTS_HERE){
                        ++nextLetter;
                    }
                    else{
                        nextLetter=m_res[nextLetter];
                    }
                }
            }
        }
        return answer;
    }

private:
    int countSumLenght(const Words& words){
        int length=1;
        for(auto const & word : words){
            length+=word.size()+1;
        }
        return length;
    }

    void initialize(const Words& words){
        m_nuWords=words.size();
        m_firstWordInBlockToLastWordInBlock.resize(m_length);
        m_lastWordInBlockToFirstWordInBlock.resize(m_length);
        m_posToWord.resize(m_length);
        m_lengthSuffixWord.resize(m_length);

        m_suffixArray.resize(m_length);
        m_lcp.resize(m_length);
        m_rank.resize(m_length);
        m_res.resize(m_length);
        m_sumWords.resize(m_length);
        m_isJoinedPrefix.resize(m_nuWords);
        m_isJoinedSufiix.resize(m_nuWords);
        m_lengthToPos.resize(m_length);

        m_length=1;
        int wordsId = 0;
        for(auto const & word : words){
            auto wordSize = boost::distance(word);
            m_lengthWords.push_back(wordSize);
            m_lengthToPos[wordSize].push_back(m_length);
            int noLetterInWord = 0;
            for(auto letter : word) {
                assert(letter != 0);
                auto globalLetterId = m_length + noLetterInWord;
                m_sumWords[globalLetterId] = letter;
                m_posToWord[globalLetterId] = wordsId;
                m_lengthSuffixWord[globalLetterId] = wordSize - noLetterInWord;
                ++noLetterInWord;
            }
            m_firstWordInBlockToLastWordInBlock[m_length]=m_length;
            m_lastWordInBlockToFirstWordInBlock[m_length]=m_length;
            m_length += wordSize + 1;
            ++wordsId;
        }
    }

    void eraseWordFormPrefixTree(int word){
        m_isJoinedSufiix[m_posToWord[word]]=JOINED;
        m_prefixTree.eraseWordFormPrefixTree(word);
    }


    void joinAllWords(){
        auto ovelapSizeRange = boost::irange(0, m_length) | boost::adaptors::reversed;
        for(auto overlapSize:ovelapSizeRange){
            for(auto word : m_lengthToPos[overlapSize]){
                if(m_lcp[m_rank[word]]>=overlapSize){//check if word is substring
                    eraseWordFormPrefixTree(word);
                }
            }

        }

        //in each iteration we join all pair of words who have overlap size equal overlapSize
        for(auto overlapSize:ovelapSizeRange){
            for(auto word : m_longWords){
                joinWord(word,overlapSize);
            }
            for(auto word : m_lengthToPos[overlapSize]){
                if(m_lcp[m_rank[word]]<overlapSize){//check if word is not substring
                    m_longWords.push_back(word);
                }
            }
        }
    }

    void joinWord(int ps,int overlap){
        if(m_isJoinedPrefix[m_posToWord[ps]]==JOINED){
            return;
        };
        int suffix=m_rank[ps+m_lengthWords[m_posToWord[ps]]-overlap];

        int prefix=m_prefixTree.getPrefixEqualToSuffix(suffix,m_lastWordInBlockToFirstWordInBlock[ps]);

        if(prefix==NOT_PREFIX){
            return;
        }
        m_res[ps+m_lengthWords[m_posToWord[ps]]-overlap-1]=prefix;
        m_isJoinedPrefix[m_posToWord[ps]]=JOINED;

        m_lastWordInBlockToFirstWordInBlock[m_firstWordInBlockToLastWordInBlock[prefix]]=
                m_lastWordInBlockToFirstWordInBlock[ps];
        m_firstWordInBlockToLastWordInBlock[m_lastWordInBlockToFirstWordInBlock[prefix]]=
                prefix;
        eraseWordFormPrefixTree(prefix);


    }

    int m_length,m_nuWords;
    std::vector<Letter> m_sumWords;
    std::vector<int> m_firstWordInBlockToLastWordInBlock,m_lastWordInBlockToFirstWordInBlock,
    m_posToWord,m_lengthWords,m_lengthSuffixWord,
    m_suffixArray,m_lcp,m_rank,m_res,m_longWords;
    std::vector<bool> m_isJoinedPrefix,m_isJoinedSufiix;
    std::vector <std::vector<int> >m_lengthToPos;

    PrefixTree<Letter> m_prefixTree;

    const static bool JOINED=true;

    const static int NO_OVERLAP_STARTS_HERE=0;
    const static int NOT_PREFIX=-1;

};
}//!detail

/**
 * @param Words words
 * @brief return word contains all words as subwords,
 * of lenght at most 3.5 larger than shortest superstring.
 * words canot contains letter 0
    \snippet shortest_superstring_example.cpp Shortest Superstring Example
 *
 * complete example is shortest_superstring_example.cpp
 * @tparam Words
 */
template<typename Words>
auto  shortestSuperstring(const Words & words) ->
decltype(std::declval<detail::ShortestSuperstring<Words>>().getSolution()){
    detail::ShortestSuperstring<Words> solver(words);
    return solver.getSolution();
};

}//!greedy
}//!paal
#endif /*SHORTEST_SUPERSTRING_HPP*/
