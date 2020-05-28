#include<string>
#include <iostream>
#include "ThreadPool.hpp"

using namespace std;

int add_fun1(int n) {
    std::cout << "commonfun:add_fun1:n=" << n << std::endl;
    return n + 1;
}

double add_fun2(int n) {
    std::cout << "commonfun:add_fun2:n=" << n << std::endl;
    return n + 1.0;
}
void TestThdPool(){
    {
        ThreadPool<> pool;
        pool.Start(2);
        std::thread thd1([&pool] {
            for (int i = 0; i < 1; i++) {
                auto thdId = this_thread::get_id();
                pool.AddTask([thdId] {
                    cout << "lambda表达式:同步层线程1的线程ID:" << thdId << endl; });
            }});

        std::thread thd2([&pool] {
            auto thdId = this_thread::get_id();
            pool.AddTask([thdId] {
                cout << "lambda表达式:同步层线程2的线程ID:" << thdId << endl; });
            
            std::function<int()> f = std::bind(add_fun1, 1);
            pool.AddTask(f);//其实参数应该是std::function<void()>
            
            pool.AddTask(add_fun2, 1);
            
            });
        this_thread::sleep_for(std::chrono::seconds(2));
        pool.Stop();
        thd1.join();
        thd2.join();
    }

    {
        ThreadPool<int> pool;
        pool.Start(1);
        std::thread thd1([&pool] {
            auto thdId = this_thread::get_id();
            auto f = std::bind(add_fun1, 1);
            pool.AddTask(f);
        });

        this_thread::sleep_for(std::chrono::seconds(2));
        pool.Stop();
        thd1.join();
    }

    {
        ThreadPool<double> pool;
        pool.Start(1);
        std::thread thd1([&pool] {
            auto thdId = this_thread::get_id();
            pool.AddTask(add_fun2,1);
        });

        this_thread::sleep_for(std::chrono::seconds(2));
        pool.Stop();
        thd1.join();
    }
}

int main() {
    TestThdPool();
    return 0;
}

