#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

#include "SensitiveWordsCheck.hpp"
int main()
{
    //std::vector<std::string> vec{"你","是","傻","子"};

//    std::vector<std::string> sensitiveStr {"a", "b", "c"};//先假设abc是敏感词
    std::vector<std::string> sensitiveStr1 {"傻", "蛋"};
    std::vector<std::string> sensitiveStr2 {"傻", "子"};

    SensitiveWordsCheck<std::string> wordsCheck;
    //加入敏感词
    wordsCheck.AddWords(sensitiveStr1);
    wordsCheck.AddWords(sensitiveStr2);

//    std::vector<std::string> testStr {"g", "a", "b"};//测试字符串
//    std::vector<std::string> testStr {"h", "g", "a", "b", "c", "i", "a", "b", "d"};//测试字符串
    std::vector<std::string> testStr {"你", "是", "傻", "蛋", "我", "是", "傻", "子"};

    uint32_t sIdx = 0;
    uint32_t eIdx = 0;
    while(sIdx < testStr.size())
    {
        sIdx = eIdx == 0 ? 0 : eIdx + 1;
        if(wordsCheck.CheckWords(testStr, sIdx, eIdx))
        {
            std::stringstream iss;
            for(uint32_t idx = sIdx; idx <= eIdx; idx++)
            {
                iss << testStr[idx];
            }
            std::cout<<"testStr::有敏感词:"<<iss.str()<<std::endl;
        } else
        {
            std::cout<<"testStr:没有敏感词"<<std::endl;
        }
    }
    return 0;
}