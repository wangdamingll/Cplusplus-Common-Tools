#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include<list>
#include<thread>
#include<functional>
#include <type_traits>
#include<memory>
#include <atomic>
#include "SyncQueue.hpp"

const int MaxTaskCount = 100;

template <typename R = void>
class ThreadPool {
public:
    using Task  = std::function<R()>;

public:
    ThreadPool(int numThreads = std::thread::hardware_concurrency()) : m_queue(MaxTaskCount) {
        Start(numThreads);
    }
    ~ThreadPool(void) {
        // stop thread pool
        Stop();
    }
    void Stop() {
        // only invoke once StopThreadGroup
        std::call_once(m_flag, [this] {StopThreadGroup(); });
    }

public:
    void AddTask(Task&& task) {
        m_queue.Put(std::forward<Task>(task));
    }
    void AddTask(const Task& task) {
        m_queue.Put(task);
    }

    template<typename Fun,typename Arg ,typename... Args>
    void AddTask(Fun&& f, Arg&& arg,Args&&... args) {
        auto f_task = std::bind(std::forward<Fun>(f), std::forward<Arg>(arg),std::forward<Args>(args)...);
        m_queue.Put(std::move(f_task));
    }
public:
    void Start(int numThreads) {
        m_running = true;
        // create thread group
        for (int i = 0; i < numThreads; ++i) {
            m_threadgroup.push_back(std::make_shared<std::thread>(&ThreadPool::RunInThread, this));
        }
    }

    void RunInThread() {
        while (m_running) {
            // detach all task
            std::list<Task> list;
            m_queue.Take(list);
            for (auto& task : list) {
                if (!m_running)
                    return;
                task();
            }
        }
    }
    void StopThreadGroup() {
        m_queue.Stop();                 //let thread stop of syncqueue
        m_running = false;              // set thread status
        for (auto thread : m_threadgroup) {                       
            if (thread)
                thread->join();
        }
        m_threadgroup.clear();
    }

private:
    std::list<std::shared_ptr<std::thread>> m_threadgroup;  // thread group
    SyncQueue<Task> m_queue;                                // sync queue
    atomic_bool m_running;                                  // thread status
    std::once_flag m_flag;
};


#endif // !__THREAD_POOL__

