#include <iostream>

#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include <random>

/*
“单生产者，单消费者”(single-producer,single-consumerSPSC)队列
*/

template<typename T>
class LockFreeQueue
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        node* next;
        node() :
            next(nullptr)
        {}
    };
    std::atomic<node*> head;
    std::atomic<node*> tail;
    node* pop_head()
    {
        node* const old_head = head.load();
        if (old_head == tail.load()) // 1
        {
            return nullptr;
        }
        head.store(old_head->next);
        return old_head;
    }
public:
    LockFreeQueue() :
        head(new node), tail(head.load())
    {}
    LockFreeQueue(const LockFreeQueue& other) = delete;
    LockFreeQueue& operator=(const LockFreeQueue& other) = delete;
    ~LockFreeQueue()
    {
        while (node* const old_head = head.load())
        {
            head.store(old_head->next);
            delete old_head;
        }
    }
    std::shared_ptr<T> pop()
    {
        node* old_head = pop_head();
        if (!old_head)
        {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(old_head->data);  
        delete old_head;
        return res;
    }
    void push(T new_value)
    {
        std::shared_ptr<T> new_data(std::make_shared<T>(new_value));
        node* p = new node; 
        node* const old_tail = tail.load(); 
        old_tail->data.swap(new_data); 
        old_tail->next = p;  
        tail.store(p);
    }
};

void PushThreadFun(LockFreeQueue<int>& queue) {
    static thread_local int i = 1;
    std::random_device r;
    std::default_random_engine e(r());
    std::uniform_int_distribution<int> uniform_dist(1, 3);
    while (1) {
        std::cout << "PushThreadFun id:" << std::this_thread::get_id() << " value:" << i << std::endl;
        queue.push(i++);
        std::this_thread::sleep_for(std::chrono::seconds(uniform_dist(e)));
    }
}

void GetThreadFun(LockFreeQueue<int>& queue) {
    while (1) {
        auto res = queue.pop();
        if (res.get() != nullptr) {
            std::cout << "GetThreadFun id:" << std::this_thread::get_id() << " value:" << *res << std::endl;
        }
    }
}

int main()
{
    LockFreeQueue<int> queue;
    std::thread pushT(PushThreadFun, std::ref(queue));
    std::thread getT(GetThreadFun, std::ref(queue));

    pushT.join();
    getT.join();
}

