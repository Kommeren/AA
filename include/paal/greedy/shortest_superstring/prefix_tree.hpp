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
class PrefixTree{
    
    
    struct Node{
        Letter letter;
        Node* son;
        std::vector<int> prefixes;//ends of all prefixes of single words in concatenate words corresponding to Node
        Node(int _letter): letter(_letter){
            son=CHILDLESS;
        };
        Node(){
            son=CHILDLESS;
            letter=DELIMITER;
        };//root
    };
public:
    PrefixTree(int length,
               std::vector<int> const& suffixArray,
               std::vector<Letter> const& sumWords,
               std::vector<int> const& lcp,
               std::vector<int> const& lengthSuffixWord
               ):m_length(length),
               m_suffixArray(suffixArray),
               m_sumWords(sumWords),
               m_lcp(lcp),
               m_lengthSuffixWord(lengthSuffixWord){}
    
    void buildPrefixTree(){
        //reserve to never reallocation m_prefixTree
        m_prefixToTree.resize(m_length);
        m_prefixTree.reserve(m_length);
        m_suffixToTree.resize(m_length);
        m_wichSonAmI.resize(m_length);
        
        m_prefixTree.push_back(Node());
        for(auto suffix : m_suffixArray){
            //memory protection and we add only whole words in lexographic order
            if((suffix!=0) && (m_sumWords[suffix-1]==DELIMITER)){
                addWordToPrefixTree(suffix);
            }
        }
    }
    
    
    void eraseWordFormPrefixTree(int wordBegin){
        for(int letterOfWord=0;m_sumWords[letterOfWord+wordBegin]!=DELIMITER;++letterOfWord){
            int lastPrefix=m_prefixToTree[wordBegin+letterOfWord]->prefixes.back();
            m_prefixToTree[wordBegin+letterOfWord]->prefixes[m_wichSonAmI[wordBegin+letterOfWord]]=lastPrefix;
            m_wichSonAmI[lastPrefix+letterOfWord]=m_wichSonAmI[wordBegin+letterOfWord];
            m_prefixToTree[wordBegin+letterOfWord]->prefixes.pop_back();
        }
    }
    
    //for all suffix of word: if suffix is equal to any prefix of word we remember position in prefix tree coresponding to suffix
    void fillSuffixToTree(){
        for(int suffix=m_length-1,lastWord=0,commonPrefix=0;suffix>0;suffix--){
            if(m_sumWords[m_suffixArray[suffix]-1]==DELIMITER){
                lastWord=m_suffixArray[suffix];
                commonPrefix=m_lcp[suffix];
            }
            else{
                if(commonPrefix==m_lengthSuffixWord[m_suffixArray[suffix]]){
                    m_suffixToTree[suffix]=m_prefixToTree[lastWord+commonPrefix-1];
                }
                if(m_lcp[suffix]<commonPrefix){
                    commonPrefix=m_lcp[suffix];
                }
            }
        }
    }
    
    int getPrefixEqualToSuffix(int suffix,int firstWordInBlock){
        if(m_suffixToTree[suffix]==NO_SUFFIX_IN_TREE){
            return NOT_PREFIX;
        }
        Node* overlapPrefix=m_suffixToTree[suffix];
        
        if(overlapPrefix->prefixes.size()){
            int whichPrefix=ANY_PREFIX;//which prefix of prefixes equal to suffix, will be joined
            //check if first prefix belong to same block as prefix (avoid loops)
            if(overlapPrefix->prefixes[whichPrefix]==firstWordInBlock)
            {
                if(overlapPrefix->prefixes.size()>=2){
                    whichPrefix=ANY_OTHER_PREFIX;
                }
                else{
                    return NOT_PREFIX;
                }
            }
            return overlapPrefix->prefixes[whichPrefix];
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
        while(m_sumWords[letter]){
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
        //    m_wichSonAmI
        //and add to Node.prefixes coresponding prefixes
        while(m_sumWords[letter]!=DELIMITER){
            node=node->son;
            
            m_prefixToTree[letter]=node;
            m_wichSonAmI[letter]=node->prefixes.size();
            node->prefixes.push_back(word);
            ++letter;
        }
    }
    
    std::vector<Node> m_prefixTree;
    std::vector<int> m_wichSonAmI;
    
    std::vector<Node*> m_prefixToTree;
    std::vector<Node*> m_suffixToTree;
    
    int m_length;
    const std::vector<int> & m_suffixArray;
    const std::vector<Letter> & m_sumWords;
    const std::vector<int> & m_lcp;
    const std::vector<int> & m_lengthSuffixWord;
    
    const static int ROOT=0;
    const static int NOT_PREFIX=-1;
    const static int ANY_PREFIX=0; 
    const static int ANY_OTHER_PREFIX=1; 
    const static int NO_SUFFIX_IN_TREE=0;
    const static std::nullptr_t CHILDLESS;
public:
    const static Letter DELIMITER=0;
    
};
}//!shortest_superstring
}//!greedy
}//!paal
#endif /*PREFIX_TREE_HPP*/