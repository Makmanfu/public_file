#ifndef SYNCHRONIZEDQUEUE_H
#define SYNCHRONIZEDQUEUE_H
 
#include <queue>
#include <mutex>
#include <condition_variable>
 
#include <iostream>
 
template<typename T>
class SynchronizedQueue
{
public:
    SynchronizedQueue(unsigned int maxSize = 1):
        m_queue(), m_mutex(), m_cond(), request_to_end(false), enqueue_data(true), m_maxSize(maxSize)
    {
        m_currentSize = 0;
    }
 
    ~SynchronizedQueue()
    {
        StopQueue();
    }
 
    void Enqueue(const T& data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(enqueue_data)
        {
            if(m_currentSize == m_maxSize)
            {
                m_queue.pop();
                m_queue.push(data);
            }
            else
            {
                m_queue.push(data);
                m_currentSize++;
            }
 
            m_cond.notify_one();
//            std::cout << m_currentSize << std::endl;
        }
    }
 
    bool Dequeue(T& result)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
 
        while(m_queue.empty() && (!request_to_end))
            m_cond.wait(lock);
 
 
        if(request_to_end)
        {
            DoEndActions();
            return false;
        }
 
        result = m_queue.front();
        m_queue.pop();
 
        m_currentSize--;
 
        return true;
    }
 
    void StopQueue()
    {
        {
        std::lock_guard<std::mutex> lock(m_mutex);
        request_to_end = true;
        }
        m_cond.notify_one();
    }
 
    unsigned int Size()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return static_cast<unsigned int> (m_queue.size());
    }
 
    bool IsEmpty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return (m_queue.empty());
    }
 
private:
    void DoEndActions()
    {
        enqueue_data = false;
 
        while(!m_queue.empty())
            m_queue.pop();
    }
 
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
 
    bool request_to_end;
    bool enqueue_data;
 
    unsigned int m_maxSize;
    unsigned int m_currentSize;
};
#endif // SYNCHRONIZEDQUEUE_H
