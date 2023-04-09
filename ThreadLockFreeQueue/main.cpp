#include <iostream>
#include <thread>
#include <chrono>
#include <random>
using namespace std;

#include "LockFreeQueue.hpp"

LockFreeQueue<int> lockFreeQueue;

void PushThreadFun()
{
    static thread_local int i = 1;
    std::random_device r;
    std::default_random_engine e(r());
    std::uniform_int_distribution<int> uniform_dist(1, 3);
    while (true)
    {
        int* p = new int (i);
        std::cout << "PushThreadFun id:" << std::this_thread::get_id() << " value:" << i << std::endl;
        lockFreeQueue.Push(p);
        i++;
        std::this_thread::sleep_for(std::chrono::seconds(uniform_dist(e)));
    }
}

void GetThreadFun()
{
    while (true)
    {
        if(!lockFreeQueue.Empty())
        {
            auto* tmp = lockFreeQueue.Pop();
            while(!tmp->empty())
            {
                auto* pData = tmp->front();
                tmp->pop();
                std::cout << "GetThreadFun id:" << std::this_thread::get_id() << " value:" << *pData << std::endl;
            }
        }
    }
}

int main()
{
    std::thread pushT(PushThreadFun);
    std::thread getT(GetThreadFun);

    pushT.join();
    getT.join();
    return 0;
}