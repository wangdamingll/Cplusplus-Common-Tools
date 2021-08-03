#include "CurlMgr.h"

CurlMgr::CurlMgr() : m_pMultiHandle(nullptr)
{}


CurlMgr::~CurlMgr()
{
    Final();
}

int32_t CurlMgr::Init()
{
    m_pMultiHandle = curl_multi_init();
    if(m_pMultiHandle == nullptr)
    {
        return CurlError_MultiInitFail;
    }
    return 0;
}

void CurlMgr::Final()
{
    if(m_pMultiHandle != nullptr)
    {
        curl_multi_cleanup(m_pMultiHandle);
    }
    
    for(auto& urlReq : m_mapReq)
    {
        curl_easy_cleanup(urlReq.first);
    }
    m_mapReq.clear();
}

int32_t CurlMgr::AddRequest(const std::string& sUrl, const std::string& sPost, CurlFun&& fResCall, void* sUserData, const char* headers)
{
    if(m_mapReq.size() > MAX_REQ_SIZE)
    {
        return CurlError_CurlSizeLimit;
    }

    UrlReq* pReq = new UrlReq();
    if(pReq == nullptr)
    {
        return CurlError_AllocUrlReqFail;
    }
    int32_t ret = _InitCurl(*pReq, sUrl, sPost, headers);
    if(ret != 0)
    {
        delete pReq;
        return ret;
    }

    pReq->fCall = fResCall;
    pReq->sUserData = sUserData;
    m_mapReq[pReq->pCurl] = pReq;
    return 0;
}

int32_t CurlMgr::_InitCurl(UrlReq& req, const std::string& sUrl, const std::string& sPost, const char* headers)
{
    CURL* pCurl = curl_easy_init();
    if(pCurl == nullptr)
    {
        return CurlError_EasyInitFail;
    }

    req.pCurl = pCurl;

    if(headers != nullptr)
    {
        req.headers = curl_slist_append(nullptr, headers);
        if(req.headers == nullptr)
        {
            return CurlError_Slist_Append;
        }
        CURLcode ret = curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, req.headers);
        if(ret != CURLE_OK)
        {
            return CurlError_SetOptFail;
        }
    }

    CURLcode ret = curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, &_DataWrite);
    if(ret != CURLE_OK)
    {
        return CurlError_SetOptFail;
    }

    ret = curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, 1L);
    if(ret != CURLE_OK)
    {
        return CurlError_SetOptFail;
    }

    ret = curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L);
    if(ret != CURLE_OK)
    {
        return CurlError_SetOptFail;
    }

    //_DataWrite userP
    ret = curl_easy_setopt(pCurl, CURLOPT_FILE, &req);
    if(ret != CURLE_OK)
    {
        return CurlError_SetOptFail;
    }

    ret = curl_easy_setopt(pCurl, CURLOPT_TIMEOUT_MS, MAX_CURL_TIME_OUT);
    if(ret != CURLE_OK)
    {
        return CurlError_SetOptFail;
    }

    ret = curl_easy_setopt(pCurl, CURLOPT_URL, sUrl.c_str());
    if(ret != CURLE_OK)
    {
        return CurlError_SetOptFail;
    }

    if(!sPost.empty())
    {
        ret = curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, sPost.c_str());
        if(ret != CURLE_OK)
        {
            return CurlError_SetOptFail;
        }
    }

    CURLMcode mRet = curl_multi_add_handle(m_pMultiHandle, pCurl);
    if(mRet != CURLM_OK)
    {
        return CurlError_MultiAddFail;
    }
    return 0;
}

size_t CurlMgr::_DataWrite(void* buf, size_t size, size_t nmemb, void* userP)
{
    if(userP != nullptr)
    {
        UrlReq* pReq = static_cast<UrlReq*>(userP);
        pReq->sResult.append((char*)buf, size * nmemb);
        return size * nmemb;
    }
    return 0;
}

void CurlMgr::Update()
{
    if(m_mapReq.empty())
    {
        return ;
    }

    int32_t iLeftMsg = 0;
    int32_t iRunning = 0;
    CURLMsg* msg = nullptr;

    curl_multi_perform(m_pMultiHandle, &iRunning);

    while((msg = curl_multi_info_read(m_pMultiHandle, &iLeftMsg)) != nullptr)
    {
        if(msg->msg == CURLMSG_DONE)
        {
            CURL* pHandle = msg->easy_handle;
            UrlReqMapIter iter = m_mapReq.find(pHandle);
            if(iter != m_mapReq.end())
            {
                iter->second->fCall(msg->data.result, std::move(iter->second->sResult), iter->second->sUserData);
                delete iter->second;
                m_mapReq.erase(iter);
            }
            
            curl_multi_remove_handle(m_pMultiHandle, pHandle);
        }
    }
}
