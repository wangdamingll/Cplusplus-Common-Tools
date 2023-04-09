#ifndef _LOCK_FREE_QUEUE__
#define _LOCK_FREE_QUEUE__

#include <iostream>
#include <atomic>
#include <chrono>
#include <queue>
#include <cstdint>

using namespace std;

template <typename T>
class LockFreeQueue
{
public:
    LockFreeQueue()
    {
        mIndex.store(0);
    };
    ~LockFreeQueue()
    {
        for(uint32_t index = 0; index < 2; index++)
        {
            std::queue<T*>& queue = queueArr[index];
            while(!queue.empty())
            {
                auto* p = queue.front();
                if(p != nullptr)
                {
                    delete p;
                }
                queue.pop();
            }
        }
    }

public:
    void Push(T* data)
    {
        queueArr[mIndex].push(data);
    }

    std::queue<T*>* Pop()
    {
        uint32_t cur = mIndex.load();
        mIndex = (cur + 1) % 2;
        return &queueArr[cur];
    }

    bool Empty()
    {
        uint32_t cur = mIndex.load();
        return (queueArr[cur].empty() && queueArr[(cur + 1) % 2].empty());
    }

private:
    std::atomic<uint32_t> mIndex;
    std::queue<T*> queueArr[2];
};









#endif //_LOCK_FREE_QUEUE__