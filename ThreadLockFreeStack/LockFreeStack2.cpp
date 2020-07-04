#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>
#include <random>

/*
使用无锁std::shared_ptr<>的实现:std::atomic_is_lock_free(&some_shared_ptr)返回true
*/

template<typename T>
class LockFreeStack
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        std::shared_ptr<node> next;
        node(T const& data_) :
            data(std::make_shared<T>(data_))
        {}
    };
    std::shared_ptr<node> head;
public:
    void push(T const& data)
    {
        std::shared_ptr<node> const new_node = std::make_shared<node>(data);
        new_node->next = head;
        while (!std::atomic_compare_exchange_weak(
            &head, &new_node->next, new_node));
    }
    std::shared_ptr<T> pop()
    {
        std::shared_ptr<node> old_head = std::atomic_load(&head);
        while (old_head && !std::atomic_compare_exchange_weak(
            &head, &old_head, old_head->next));
        return old_head ? old_head->data : std::shared_ptr<T>();
    }
};

void PushThreadFun(LockFreeStack<int>& stack) {
    static thread_local int i = 1;
    std::random_device r;
    std::default_random_engine e(r());
    std::uniform_int_distribution<int> uniform_dist(1, 3);
    while (1) {
        std::cout << "PushThreadFun id:" << std::this_thread::get_id()<<" value:"<<i << std::endl;
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


