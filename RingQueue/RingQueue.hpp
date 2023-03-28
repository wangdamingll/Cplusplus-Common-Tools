#ifndef __RING_QUEUE_H__
#define __RING_QUEUE_H__

#include <stdint.h>
#include <mutex>

template<typename T, uint32_t MAX>
class RingQueue
{
public:
    RingQueue();
    ~RingQueue();

    bool Push(const T& item);
    bool Pop(T& item);

    bool IsEmpty();
    bool IsFull();

private:
    uint32_t _IncIndex(uint32_t index) const;

private:
    volatile uint32_t m_nTail{0};
    volatile uint32_t m_nHead{0};
    T m_items[MAX + 1];
    std::mutex m_mutex;
};

template<typename T, uint32_t MAX>
RingQueue<T, MAX>::RingQueue() : m_nTail(0), m_nHead(0){}

template<typename T, uint32_t MAX>
RingQueue<T, MAX>::~RingQueue(){}

template<typename T, uint32_t MAX>
bool RingQueue<T, MAX>::Push(const T& item)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    uint32_t nextTail = this->_IncIndex(m_nTail);
    if(nextTail == m_nHead)
    {
        return false;
    }
    m_items[m_nTail] = item;
    m_nTail = nextTail;
    return true;
}

template<typename T, uint32_t MAX>
bool RingQueue<T, MAX>::Pop(T& item)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    if(m_nTail == m_nHead)
    {
        return false;
    }
    item = m_items[m_nHead];
    m_nHead = this->_IncIndex(m_nHead);
    return true;
}

template<typename T, uint32_t MAX>
bool RingQueue<T, MAX>::IsEmpty()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return (m_nHead == m_nTail);
}

template<typename T, uint32_t MAX>
bool RingQueue<T, MAX>::IsFull()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    uint32_t nextTail = this->_IncIndex(m_nTail);
    return (nextTail == m_nHead);
}

template<typename T, uint32_t MAX>
uint32_t RingQueue<T, MAX>::_IncIndex(uint32_t index) const
{
    return (index + 1) % (MAX + 1);
}

#endif //__RING_QUEUE_H__