#include <iostream>

#include <memory>
#include <mutex>

template<typename T>
class ThreadsafeList
{
    struct node
    {
        std::mutex m;
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;

        node() : next(){}

        node(T const& value) : data(std::make_shared<T>(value))
        {}
    };

    node head;

public:
    ThreadsafeList()
    {}

    ~ThreadsafeList()
    {
        remove_if([](T const&) {return true; });
    }

    ThreadsafeList(ThreadsafeList const& other) = delete;
    ThreadsafeList& operator=(ThreadsafeList const& other) = delete;

    void push_front(T const& value)
    {
        std::unique_ptr<node> new_node(new node(value));
        std::lock_guard<std::mutex> lk(head.m);
        new_node->next = std::move(head.next);
        head.next = std::move(new_node);
    }

    template<typename Function>
    void for_each(Function f)
    {
        node* current = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while (node* const next = current->next.get())
        {
            std::unique_lock<std::mutex> next_lk(next->m);
            lk.unlock();
            f(*next->data);
            current = next;
            lk = std::move(next_lk);
        }
    }

    template<typename Predicate>
    std::shared_ptr<T> find_first_if(Predicate p)
    {
        node* current = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while (node* const next = current->next.get())
        {
            std::unique_lock<std::mutex> next_lk(next->m);
            lk.unlock();
            if (p(*next->data))
            {
                return next->data;
            }
            current = next;
            lk = std::move(next_lk);
        }
        return std::shared_ptr<T>();
    }

    template<typename Predicate>
    void remove_if(Predicate p)
    {
        node* current = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while (node* const next = current->next.get())
        {
            std::unique_lock<std::mutex> next_lk(next->m);
            if (p(*next->data))
            {
                std::unique_ptr<node> old_next = std::move(current->next);
                current->next = std::move(next->next);
                next_lk.unlock();
            }
            else
            {
                lk.unlock();
                current = next;
                lk = std::move(next_lk);
            }
        }
    }
};


int main()
{
    //简单函数功能测试
    ThreadsafeList<int> list;
    list.push_front(1);
    list.for_each([](const int& item) {
        std::cout << item << " ";
        });
    std::cout << std::endl;
    std::shared_ptr<int> ptr=list.find_first_if([](const int& item) {return item == 1; });
    if (ptr.get()!=nullptr) {
        std::cout << *ptr << std::endl;
    }
}


