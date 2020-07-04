// LockFreeStack.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>
#include <random>

template<typename T>
class LockFreeStack
{
private:
    struct node;
    struct counted_node_ptr
    {
        int external_count{0};
        node* ptr{nullptr};
    };
    struct node
    {
        std::shared_ptr<T> data;
        std::atomic<int> internal_count;
        counted_node_ptr next;
        node(T const& data_) :
            data(std::make_shared<T>(data_)),
            internal_count(0)
        {}
    };
    std::atomic<counted_node_ptr> head;
    void increase_head_count(counted_node_ptr& old_counter)
    {
        counted_node_ptr new_counter;
        do
        {
            new_counter = old_counter;
            ++new_counter.external_count;
        } while (!head.compare_exchange_strong(
            old_counter, new_counter,
            std::memory_order_acquire,
            std::memory_order_relaxed));
        old_counter.external_count = new_counter.external_count;
    }
public:
    ~LockFreeStack()
    {
        if (head.load(std::memory_order_relaxed).ptr != nullptr) {
            while (pop());
        }
    }
    void push(T const& data)
    {
        counted_node_ptr new_node;
        new_node.ptr = new node(data);
        new_node.external_count = 1;
        new_node.ptr->next = head.load(std::memory_order_relaxed);
            while (!head.compare_exchange_weak(
                new_node.ptr->next, new_node,
                std::memory_order_release,
                std::memory_order_relaxed));
    }
    std::shared_ptr<T> pop()
    {
        counted_node_ptr old_head =
            head.load(std::memory_order_relaxed);
        for (;;)
        {
            increase_head_count(old_head);
            node* const ptr = old_head.ptr;
            if (!ptr)
            {
                return std::shared_ptr<T>();
            }
            if (head.compare_exchange_strong(
                old_head, ptr->next, std::memory_order_relaxed))
            {
                std::shared_ptr<T> res;
                res.swap(ptr->data);
                int const count_increase = old_head.external_count - 2;
                if (ptr->internal_count.fetch_add(
                    count_increase, std::memory_order_release) == -count_increase)
                {
                    delete ptr;
                }
                return res;
            }
            else if (ptr->internal_count.fetch_add(
                -1, std::memory_order_relaxed) == 1)
            {
                ptr->internal_count.load(std::memory_order_acquire);
                delete ptr;
            }
        }
    }
};

void PushThreadFun(LockFreeStack<int>& stack) {
    static thread_local int i = 1;
    std::random_device r;
    std::default_random_engine e(r());
    std::uniform_int_distribution<int> uniform_dist(1, 3);
    while (1) {
        std::cout << "PushThreadFun1 id:" << std::this_thread::get_id()<<" value:"<<i << std::endl;
        stack.push(i++);
        std::this_thread::sleep_for(std::chrono::seconds(uniform_dist(e)));
    }
}

void GetThreadFun(LockFreeStack<int>& stack) {
    while (1) {
        auto res = stack.pop();
        if (res.get() != nullptr) {
            std::cout << "GetThreadFun id:" << std::this_thread::get_id() << " value:" << *res << std::endl;
        }
    }
}

int main()
{
    LockFreeStack<int> stack;
    std::thread pushT1(PushThreadFun,std::ref(stack));
    std::thread pushT2(PushThreadFun, std::ref(stack));
    std::thread getT1(GetThreadFun, std::ref(stack));
    std::thread getT2(GetThreadFun, std::ref(stack));

    pushT1.join();
    pushT2.join();
    getT1.join();
    getT2.join();
    std::cout << "Hello World!\n";
}


