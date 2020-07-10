#ifndef _THREAD_POOL_HPP__
#define _THREAD_POOL_HPP__

#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <future>
#include <type_traits>

#include "ThreadSafeQueue.hpp"

class function_wrapper
{
    struct impl_base {
        virtual void call() = 0;
        virtual ~impl_base() {}
    };
    std::unique_ptr<impl_base> impl;
    template<typename F>
    struct impl_type : impl_base
    {
        F f;
        impl_type(F&& f_) : f(std::move(f_)) {}
        void call() { f(); }
    };
public:
    void operator()() { impl->call(); }

    function_wrapper() = default;

    template<typename F>
    function_wrapper(F&& f) : impl(new impl_type<F>(std::move(f))){}

    function_wrapper(function_wrapper&& other) :impl(std::move(other.impl)){}

    function_wrapper& operator=(function_wrapper&& other)
    {
        impl = std::move(other.impl);
        return *this;
    }

    function_wrapper(const function_wrapper&) = delete;
    function_wrapper(function_wrapper&) = delete;
    function_wrapper& operator=(const function_wrapper&) = delete;
};

class work_stealing_queue
{
private:
    typedef function_wrapper data_type;
    std::deque<data_type> the_queue;
    mutable std::mutex the_mutex;

public:
    work_stealing_queue()
    {}

    work_stealing_queue(const work_stealing_queue& other) = delete;
    work_stealing_queue& operator=(
        const work_stealing_queue& other) = delete;

    void push(data_type data)
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        the_queue.push_front(std::move(data));
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        return the_queue.empty();
    }

    bool try_pop(data_type& res)
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        if (the_queue.empty())
        {
            return false;
        }

        res = std::move(the_queue.front());
        the_queue.pop_front();
        return true;
    }

    bool try_steal(data_type& res)
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        if (the_queue.empty())
        {
            return false;
        }

        res = std::move(the_queue.back());
        the_queue.pop_back();
        return true;
    }
};




class ThreadPool
{
    typedef function_wrapper task_type;

    std::atomic_bool done;
    ThreadsafeQueue<task_type> pool_work_queue;
    std::vector<std::unique_ptr<work_stealing_queue> > queues;
    std::vector<std::thread> threads;

    static thread_local work_stealing_queue* local_work_queue;
    static thread_local unsigned my_index;

    void worker_thread(unsigned my_index_)
    {
        my_index = my_index_;
        local_work_queue = queues[my_index].get();
        while (!done)
        {
            run_pending_task();
        }
    }

    bool pop_task_from_local_queue(task_type& task)
    {
        return local_work_queue && local_work_queue->try_pop(task);
    }

    bool pop_task_from_pool_queue(task_type& task)
    {
        return pool_work_queue.try_pop(task);
    }

    bool pop_task_from_other_thread_queue(task_type& task)
    {
        for (unsigned i = 0; i < queues.size(); ++i)
        {
            unsigned const index = (my_index + i + 1) % queues.size();
            if (queues[index]->try_steal(task))
            {
                return true;
            }
        }

        return false;
    }

public:
    ThreadPool() : done(false)
    {
        unsigned const thread_count = std::thread::hardware_concurrency();

        try
        {
            for (unsigned i = 0; i < thread_count; ++i)
            {
                queues.push_back(std::unique_ptr<work_stealing_queue>(new work_stealing_queue));
                threads.push_back(std::thread(&ThreadPool::worker_thread, this, i));
            }
        }
        catch (...)
        {
            done = true;
            throw;
        }
    }

    ~ThreadPool()
    {
        done = true;
        std::for_each(threads.begin(), threads.end(),[](std::thread& thread) { thread.join();});
    }

    template<typename ResultType>
    using TaskHandle = std::future<ResultType>;

    template<typename FunctionType>
    TaskHandle<typename std::result_of<FunctionType()>::type> submit(FunctionType f)
    {
        typedef typename std::result_of<FunctionType()>::type result_type;

        std::packaged_task<result_type()> task(f);
        TaskHandle<result_type> res(task.get_future());
        if (local_work_queue)
        {
            local_work_queue->push(std::move(task));
        }
        else
        {
            pool_work_queue.push(std::move(task));
        }
        return res;
    }

    void run_pending_task()
    {
        task_type task;
        if (pop_task_from_local_queue(task) ||
            pop_task_from_pool_queue(task) ||
            pop_task_from_other_thread_queue(task))
        {
            task();
        }
        else
        {
            std::this_thread::yield();
        }
    }
};


#endif // !_THREAD_POOL_HPP__




