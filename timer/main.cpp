#include <iostream>
#include <ctime>
#include <functional>
using namespace std;

#include "TimerMgr.h"

struct TestA
{
    int count = 0;
};

void Test1()
{
    auto now = time(nullptr);
    INIT_TIMERMGR(timerMgr, now * 1000, 1);

    std::string* user = new std::string("test");

    TestA test;
    TestA testFirst;

    auto f = [](void* data, void* user){
        TestA* dataTmp = (TestA*)data;
        std::cout<<"count=" << ++dataTmp->count << " time="<<time(nullptr)<<std::endl;
    };

    auto fFirst = [](void* data, void* user){
        TestA* dataTmp = (TestA*)data;
        std::cout<<"first count=" << ++dataTmp->count << " time="<<time(nullptr)<<std::endl;
    };

    uint64_t timerId = 0;
    REG_TIMER(timerMgr, f, &test, nullptr, 1 * 1000, 0, timerId);

    uint64_t timerIdFirst = 0;
    REG_TIMER_FIRST(timerMgr, fFirst, &testFirst, nullptr, 1 * 1000, 0, 2 * 1000, timerIdFirst);

    auto cur = time(nullptr);
    while(true)
    {
        auto now = time(nullptr);
        RUN_TIMERMGR(timerMgr, now * 1000);
        if(now - cur >= 5)
        {
            STOP_TIMER(timerMgr, timerId);
            UNREG_TIMER(timerMgr, timerIdFirst);
            break;
        }
    }

    START_TIMER(timerMgr, timerId,2 * 1000, 2);

    while(true)
    {
        auto now = time(nullptr);
        RUN_TIMERMGR(timerMgr, now * 1000);
        if(test.count == 7)
        {
            STOP_TIMER(timerMgr, timerId);
            break;
        }
    }

    UNREG_TIMER(timerMgr, timerId);
    UNINIT_TIMERMGR(timerMgr);

    delete user;
}

class A
{
public:
    void Init()
    {
        auto now = time(nullptr);
        INIT_TIMERMGR(timerMgr, now * 1000, 1);

        std::string* user = new std::string("test");

        uint64_t timerId =0;
        REG_CLASS_TIMER(timerMgr, A::TestFun, this, user, 1 * 1000, 0, timerId);

        auto cur = time(nullptr);
        while(true)
        {
            auto now = time(nullptr);
            RUN_TIMERMGR(timerMgr, now * 1000)
            if(now - cur >= 5)
            {
                UNREG_TIMER(timerMgr, timerId);
                break;
            }
        }

        UNINIT_TIMERMGR(timerMgr);

        delete user;
    }

    void TestFun(void* data, void* user)
    {
        A* dataTmp = (A*)data;
        std::string* userTmp = (std::string*)user;
        std::cout<<"count=" << ++dataTmp->count <<" user=" << *userTmp<<" time="<<time(nullptr)<<std::endl;
    }

private:
    int count = 0;
};

void Test2()
{
    A a;
    a.Init();
}

int main()
{
    Test1();
    Test2();
    return 0;
}