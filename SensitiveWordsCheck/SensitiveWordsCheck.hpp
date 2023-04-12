#ifndef _SENSITIVE_WORDS_CHECK_H__
#define _SENSITIVE_WORDS_CHECK_H__

#include <unordered_map>
#include <vector>

using namespace std;

template <typename T>
class WordNode
{
    template <typename V>
    friend class SensitiveWordsCheck;

private:
    //添加敏感词,如"你是傻子" 添加"你"
    bool AddWord(const std::vector<T>& words, uint32_t idx);

    //检查敏感词,如"你是傻子"
    bool CheckWords(const std::vector<T>& words, uint32_t& sIdx, uint32_t& eIdx);

    //释放节点资源
    void Free();

private:
    std::unordered_map<T, WordNode*/*也可以用智能指针*/> wordsMap;//子节点
};

template <typename T>
class SensitiveWordsCheck
{
public:
    SensitiveWordsCheck() = default;
    ~SensitiveWordsCheck();//如果用智能指针就不用写析构函数具体逻辑了啊

public:
    //添加敏感词,如"你是傻子"
    bool AddWords(const std::vector<T>& words);
    //检查敏感词,如"你是傻子"
    bool CheckWords(const std::vector<T>& words, uint32_t& sIdx, uint32_t& eIdx);

private:
    std::unordered_map<T, WordNode<T>*/*也可以用智能指针*/> wordsMap;//子森林
};

template <typename T>
bool WordNode<T>::AddWord(const std::vector<T>& words, uint32_t idx)
{
    if(idx < words.size() - 1)
    {
        WordNode*& node = wordsMap[words[++idx]];
        if(node == nullptr)
        {
            node = new WordNode();
        }
        node->AddWord(words,idx);
    }
    return true;
}

template <typename T>
bool WordNode<T>::CheckWords(const std::vector<T>& words, uint32_t &sIdx, uint32_t &eIdx)
{
    uint32_t sIdxTmp = sIdx;
    if(++sIdxTmp >= words.size())
    {
        eIdx = sIdxTmp - 1;
        return true;
    }
    const auto iter = wordsMap.find(words[sIdxTmp]);
    if(iter == wordsMap.end())
    {
        eIdx = sIdxTmp - 1;
        return true;
    }
    iter->second->CheckWords(words, sIdxTmp, eIdx);
    return true;
}

template <typename T>
void WordNode<T>::Free()
{
    for(auto pair : wordsMap)
    {
        if(pair.second != nullptr)
        {
            pair.second->Free();
            delete pair.second;
        }
    }
}

template <typename T>
SensitiveWordsCheck<T>::~SensitiveWordsCheck()
{
    for(auto pair : wordsMap)
    {
        if(pair.second != nullptr)
        {
            pair.second->Free();
            delete pair.second;
        }
    }
}

template <typename T>
bool SensitiveWordsCheck<T>::AddWords(const std::vector<T>& words)
{
    if(!words.empty())
    {
        WordNode<T>*& node = wordsMap[words[0]];
        if(node == nullptr)
        {
            node = new WordNode<T>();
        }
        node->AddWord(words, 0);
    }
    return true;
}

template <typename T>
bool SensitiveWordsCheck<T>::CheckWords(const std::vector<T>& words, uint32_t& sIdx, uint32_t& eIdx)
{
    eIdx = 0;
    for(uint32_t index = sIdx ; index < words.size(); index++)
    {
        const auto iter = wordsMap.find(words[index]);
        if(iter == wordsMap.end())
        {
            sIdx++;
            continue;
        }
        return iter->second->CheckWords(words, sIdx, eIdx);
    }
    return (sIdx != words.size());
}

#endif //_SENSITIVE_WORDS_CHECK_H__