/**
 * @file prefix_tree.hpp
 * @brief 
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-09-10
 */
#ifndef PREFIX_TREE_HPP
#define PREFIX_TREE_HPP

#include <vector>
namespace paal{
namespace greedy{
namespace shortest_superstring{
    
template<typename Letter>
class PrefixTree {
    
    struct Node{
        Letter letter = DELIMITER;
        Node* son = CHILDLESS;
        std::vector<int> prefixes;//ends of all prefixes of single words in concatenate words corresponding to Node
        Node(int _letter): letter(_letter){
        };
        Node(){
        };//root
    };
public:
    PrefixTree(int length,
               std::vector<int> const& suffixArray,
               std::vector<Letter> const& sumWords,
               std::vector<int> const& lcp,
               std::vector<int> const& lengthSuffixWord) : 
                 m_length(length),
                 m_prefixTree(m_length),
                 m_whichSonAmI(m_length),
                 m_prefixToTree(m_length),
                 m_suffixToTree(m_length),
                 m_suffixArray(suffixArray),
                 m_sumWords(sumWords),
                 m_lcp(lcp),
                 m_lengthSuffixWord(lengthSuffixWord)
    {}
    
    void buildPrefixTree(){
        m_prefixTree.push_back(Node()); //root
        for(auto suffix : m_suffixArray){
            //memory protection and we add only whole words in lexographic order
            if((suffix!=0) && (m_sumWords[suffix-1]==DELIMITER)){
                addWordToPrefixTree(suffix);
            }
        }
    }
    
    void eraseWordFormPrefixTree(int wordBegin){
        for(int letterOfWord=0;m_sumWords[letterOfWord+wordBegin]!=DELIMITER;++letterOfWord) {
            auto letterIdx = wordBegin + letterOfWord;
            auto whichSon = m_whichSonAmI[letterIdx];
            auto & nodePrefixes = m_prefixToTree[letterIdx]->prefixes;
            int lastPrefix = nodePrefixes.back();
            nodePrefixes[whichSon] = lastPrefix;
            m_whichSonAmI[lastPrefix + letterOfWord] = whichSon;
            nodePrefixes.pop_back();
        }
    }
    
    //for all suffix of word: if suffix is equal to any prefix of word we remember position in prefix tree coresponding to suffix
    void fillSuffixToTree(){
        for(int suffix=m_length-1,lastWord=0,commonPrefix=0;suffix>0;suffix--){
            auto beginOfSuffix = m_suffixArray[suffix];
            if(beginOfSuffix == 0 || m_sumWords[beginOfSuffix - 1] == DELIMITER) {
                lastWord = beginOfSuffix;
                commonPrefix = m_lcp[suffix];
            }
            else{
                if(commonPrefix == m_lengthSuffixWord[beginOfSuffix]){
                    m_suffixToTree[suffix]=m_prefixToTree[lastWord+commonPrefix-1];
                }
                if(m_lcp[suffix] < commonPrefix){
                    commonPrefix = m_lcp[suffix];
                }
            }
        }
    }
    
    int getPrefixEqualToSuffix(int suffix,int firstWordInBlock){
        Node* nodeCorrespondingToSuffix = m_suffixToTree[suffix];
        if(nodeCorrespondingToSuffix == NO_SUFFIX_IN_TREE){
            return NOT_PREFIX;
        }
        auto const & overlapPrefixes = nodeCorrespondingToSuffix->prefixes;
        
        if(overlapPrefixes.size()){
            int whichPrefix = ANY_PREFIX;//which prefix of prefixes equal to suffix, will be joined
            //check if first prefix belong to same block as prefix (avoid loops)
            if(overlapPrefixes[whichPrefix]==firstWordInBlock)
            {
                if(overlapPrefixes.size()>=2){
                    whichPrefix = ANY_OTHER_PREFIX;
                }
                else{
                    return NOT_PREFIX;
                }
            }
            return overlapPrefixes[whichPrefix];
        }
        else{
            return NOT_PREFIX;
        }
    }
private:
    void addWordToPrefixTree(int word){
        Node* node= &m_prefixTree[ROOT];
        int letter=word;
        //we go by patch until Letter on patch all equal to letter in words
        //we only check last son because we add words in lexographic order
        while(node->son!=CHILDLESS && 
                node->son->letter==m_sumWords[letter] && 
                m_sumWords[letter]!=DELIMITER){
            node=node->son;
            ++letter;
        }
        //we add new Node
        while(m_sumWords[letter]) {
            //if this asserts, you have very strange implementation of stl 
            assert(m_prefixTree.capacity()>m_prefixTree.size());
            m_prefixTree.push_back(Node(m_sumWords[letter]));
            node->son= &m_prefixTree.back();
            node= &m_prefixTree.back();
            ++letter;
        }
        node= &m_prefixTree[ROOT];
        letter=word;
        //we fill:
        //    m_prefixToTree 
        //    m_whichSonAmI
        //and add to Node.prefixes coresponding prefixes
        while(m_sumWords[letter]!=DELIMITER){
            node=node->son;
            
            m_prefixToTree[letter]=node;
            m_whichSonAmI[letter]=node->prefixes.size();
            node->prefixes.push_back(word);
            ++letter;
        }
    }
    int m_length;
    
    std::vector<Node> m_prefixTree;
    std::vector<int> m_whichSonAmI;
    
    std::vector<Node*> m_prefixToTree;
    std::vector<Node*> m_suffixToTree;
    
    const std::vector<int> & m_suffixArray;
    const std::vector<Letter> & m_sumWords;
    const std::vector<int> & m_lcp;
    const std::vector<int> & m_lengthSuffixWord;
    
    const static int ROOT = 0;
    const static int NOT_PREFIX = -1;
    const static int ANY_PREFIX = 0; 
    const static int ANY_OTHER_PREFIX = 1; 
    const static std::nullptr_t NO_SUFFIX_IN_TREE;
    const static std::nullptr_t CHILDLESS;
public:
    const static Letter DELIMITER=0;
    
};
}//!shortest_superstring
}//!greedy
}//!paal
#endif /*PREFIX_TREE_HPP*/
