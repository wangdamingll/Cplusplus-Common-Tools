#include <iostream>

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

/*
use c++ 11
*/

template<typename T>
class ThreadsafeQueue
{
private:
    std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable data_cond;
public:
    ThreadsafeQueue()
    {}
    ThreadsafeQueue(ThreadsafeQueue const& other)
    {
        std::lock_guard<std::mutex> lk(other.mut);
        data_queue = other.data_queue;
    }

    void push(T new_value)
    {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(new_value);
        data_cond.notify_one();
    }

    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] {return !data_queue.empty(); });
        value = data_queue.front();
        data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] {return !data_queue.empty(); });
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty)
            return false;
        value = data_queue.front();
        data_queue.pop();
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};

//test
void PrepareData(ThreadsafeQueue<int>& queue) {
    while (true) {
        static int i = 1;
        std::cout << "threadid=" << std::this_thread::get_id() << " push:i=" << i << std::endl;
        queue.push(i++);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void ProcessData(ThreadsafeQueue<int>& queue) {
    while (true) {
        int i = 0;
        queue.wait_and_pop(i);
        std::cout << "threadid=" << std::this_thread::get_id << " wait_and_pop:i=" << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    ThreadsafeQueue<int> queue;

    std::thread t1(PrepareData,std::ref(queue));
    std::thread t2(ProcessData, std::ref(queue));
    t1.join();
    t2.join();
}


