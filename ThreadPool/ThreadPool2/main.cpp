#include "ThreadPool.h"
#include <iostream>

int main()
{
    ThreadPool threadPool;
    int i = 1;
    std::vector<ThreadPool::TaskHandle<int>> taskResV;
    
    taskResV.emplace_back(std::move(threadPool.submit([&]() {return i * i; })));
    taskResV.emplace_back(std::move(threadPool.submit([&]() {return (i + 1) * (i + 1); })));
    
    std::for_each(taskResV.begin(), taskResV.end(),
        [](ThreadPool::TaskHandle<int>& future) {
            std::cout << future.get()<<" " << std::endl;
        });

}