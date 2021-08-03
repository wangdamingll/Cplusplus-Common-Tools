#ifndef __CURL_MGR_H__
#define __CURL_MGR_H__

#include <functional>
#include <unordered_map>

#include <curl/curl.h>

/**
 * @brief multi curl 
 *      curl version 7.58.0
 */
class CurlMgr
{
public:
    /**
     * @brief request call back
     * 
     */
    using CurlFun = std::function<void (int32_t, std::string&&, void*)>;

    /**
     * @brief max request
     * 
     */
    const static uint32_t MAX_REQ_SIZE = 1024;

    /**
     * @brief time out, ms
     * 
     */
    const static uint32_t MAX_CURL_TIME_OUT = 5000;

    /**
     * @brief curl error
     * 
     */
    enum CurlError
    {
        CurlError_MultiInitFail = -1,
        CurlError_CurlSizeLimit = -2,
        CurlError_EasyInitFail = -3,
        CurlError_SetOptFail = -4,
        CurlError_MultiAddFail = -5,
        CurlError_AllocUrlReqFail = -6,
        CurlError_Slist_Append = -7

    };

public:
    CurlMgr();
    ~CurlMgr();

public:
    /**
     * @brief initialize resource
     * 
     * @return int32_t 
     */
    int32_t Init();

    /**
     * @brief destroy resource
     * 
     */
    void Final();

    /**
     * @brief update, main loop call
     * 
     */
    void Update();

    /**
     * @brief add url request
     * 
     * @param sUrl  http url
     * @param sPost  post data
     * @param fResCall request call back
     * @param sUserData user data
     * @param headers   header
     * @return int32_t 
     */
    int32_t AddRequest(const std::string& sUrl, const std::string& sPost, CurlFun&& fResCall, void* sUserData, const char* headers);
private:
    /**
     * @brief request param
     * 
     */
    struct UrlReq
    {
        UrlReq(){}
        virtual ~UrlReq(){
            if(userPwd != nullptr)
            {
                free(userPwd);
            }
            if(headers != nullptr)
            {
                curl_slist_free_all(headers);
            }
            if(pCurl != nullptr)
            {
                curl_easy_cleanup(pCurl);
            }
        }

        CURL* pCurl = nullptr;
        std::string sResult;
        CurlFun fCall;
        void* sUserData = nullptr;
        char* userPwd = nullptr;
        struct curl_slist* headers = nullptr;
    };
    
    /**
     * @brief private initialize curl
     * 
     * @param req   req param
     * @param sUrl  http url
     * @param sPost post data
     * @param headers http header
     * @return int32_t 
     */
    int32_t _InitCurl(UrlReq& req, const std::string& sUrl, const std::string& sPost, const char* headers);

    /**
     * @brief This callback function gets called by libcurl as soon as there is data received that needs to be saved. 
     * For most transfers, this callback gets called many times and each invoke delivers another chunk of data. 
     * buf points to the delivered data, and the size of that data is nmemb; size is always 1.
     * @param buf buf data
     * @param size size of nmemb
     * @param nmemb mem block
     * @param userP Set the userP argument with the CURLOPT_WRITEDATA option.
     * @return size_t 
     */
    static size_t _DataWrite(void* buf, size_t size, size_t nmemb, void* userP);

private:
    using UrlReqMap = std::unordered_map<CURL*, UrlReq*>;
    using UrlReqMapIter = UrlReqMap::iterator;

    UrlReqMap m_mapReq;
    CURLM* m_pMultiHandle = nullptr;
};


#endif