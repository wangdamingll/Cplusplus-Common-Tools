#include <iostream>
#include <thread>

#include "CurlMgr.h"

using namespace std;

int main()
{
    std::thread HttpThread([](){
        int32_t ret = 0;

        CurlMgr curlMgr;
        ret = curlMgr.Init();
        if(ret != 0)
        {
            std::cout<<"init ret="<<ret<<std::endl;
            return 0;
        }

        std::string* userData = new std::string("test user data");
        std::string  header("Content-Language: en,zh\r\nContent-Type: text/html; charset=utf-8");

        ret = curlMgr.AddRequest("http://www.baidu.com", "", [](int32_t ret, std::string&& httpRes, void* userData)->void{
            std::cout<<"ret="<<ret<<" httpRes="<<httpRes<<" userData="<<(char*)userData<<std::endl;
        } , (void*)userData->c_str(), header.c_str());

        if(ret != 0)
        {
            std::cout<<"ret="<<ret <<std::endl;
        }

        while(true)
        {
            curlMgr.Update();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        delete userData; //test case
    });

    HttpThread.join();
    return 0;
}