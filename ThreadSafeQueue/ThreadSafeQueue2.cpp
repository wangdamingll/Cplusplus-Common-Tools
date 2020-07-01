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
    struct node
    {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };

    std::mutex head_mutex;
    std::unique_ptr<node> head;
    std::mutex tail_mutex;
    node* tail;
    std::condition_variable data_cond;

private:
    node* get_tail()
    {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        return tail;
    }

    std::unique_ptr<node> pop_head()
    {
        std::unique_ptr<node> old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }

    std::unique_lock<std::mutex> wait_for_data()
    {
        std::unique_lock<std::mutex> head_lock(head_mutex);
        data_cond.wait(head_lock, [&] {return head.get() != get_tail(); });
        return head_lock;
    }

    std::unique_ptr<node> wait_pop_head()
    {
        std::unique_lock<std::mutex> head_lock(wait_for_data());
        return pop_head();
    }

    std::unique_ptr<node> wait_pop_head(T& value)
    {
        std::unique_lock<std::mutex> head_lock(wait_for_data());
        value = std::move(*head->data);
        return pop_head();
    }
    std::unique_ptr<node> try_pop_head()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if (head.get() == get_tail())
        {
            return std::unique_ptr<node>();
        }
        return pop_head();
    }

    std::unique_ptr<node> try_pop_head(T& value)
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if (head.get() == get_tail())
        {
            return std::unique_ptr<node>();
        }
        value = std::move(*head->data);
        return pop_head();
    }
public:
    ThreadsafeQueue() :head(new node), tail(head.get()){}
    ThreadsafeQueue(const ThreadsafeQueue& other) = delete;
    ThreadsafeQueue& operator=(const ThreadsafeQueue& other) = delete;

    std::shared_ptr<T> try_pop()
    {
        const std::unique_ptr<node> old_head = try_pop_head();
        return old_head ? old_head->data : std::shared_ptr<T>();
    }
    bool try_pop(T& value)
    {
        const std::unique_ptr<node> old_head = try_pop_head(value);
        return old_head;
    }
    std::shared_ptr<T> wait_and_pop() 
    {
        const std::unique_ptr<node> old_head = wait_pop_head();
        return old_head->data;
    }
    void wait_and_pop(T& value)
    {
        const std::unique_ptr<node> old_head = wait_pop_head(value);
    }
    void push(T new_value)
    {
        std::shared_ptr<T> new_data(
            std::make_shared<T>(std::move(new_value)));
        std::unique_ptr<node> p(new node);
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            tail->data = new_data;
            node* const new_tail = p.get();
            tail->next = std::move(p);
            tail = new_tail;
        }
        data_cond.notify_one();
    }
    void empty()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        return (head == get_tail());
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


