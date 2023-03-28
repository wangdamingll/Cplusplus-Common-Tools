#include <iostream>
#include <string>
#include "RingQueue.hpp"

using namespace std;

int main()
{
    RingQueue<uint32_t, 10> ringQueue;

    std::cout<<"is empty:"<<ringQueue.IsEmpty()<<std::endl;
    std::cout<<"is full:"<<ringQueue.IsFull()<<std::endl;

    for(uint32_t i = 1; i <= 10; i++)
    {
        std::cout<<"push:"<<i<<std::endl;
        ringQueue.Push(i);
    }

    std::cout<<"is empty:"<<ringQueue.IsEmpty()<<std::endl;
    std::cout<<"is full:"<<ringQueue.IsFull()<<std::endl;

    for(uint32_t i = 1; i <= 10; i++)
    {
        uint32_t popI = 0;
        ringQueue.Pop(popI);
        std::cout<<"pop:"<<popI<<std::endl;
    }

    std::cout<<"is empty:"<<ringQueue.IsEmpty()<<std::endl;
    std::cout<<"is full:"<<ringQueue.IsFull()<<std::endl;

    for(uint32_t i = 1; i <= 10; i++)
    {
        uint32_t popI = 0;
        ringQueue.Push(i);
        ringQueue.Pop(popI);
    }
    std::cout<<"is empty:"<<ringQueue.IsEmpty()<<std::endl;
    std::cout<<"is full:"<<ringQueue.IsFull()<<std::endl;
}