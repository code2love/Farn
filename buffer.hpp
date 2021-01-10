#ifndef __BUFFER_HPP
#define __BUFFER_HPP

#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>

template<class T>
class Buffer{
    private:
    std::mutex m_mutex;
    std::condition_variable m_hasCapacity, m_hasData;
    std::vector<T> m_buffer;
    int m_readPos, m_writePos;
    
    const int m_popTimeout = 1000;
    const int m_n;

    public:
    Buffer(int n)
        : m_n(n), m_buffer(n), m_readPos(0), m_writePos(0)
    {
    }

    float getLoad()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if(m_writePos < m_readPos) return 1 - (float)(m_readPos - m_writePos) / m_n;
        else return 1 - (float)(m_n - m_writePos + m_readPos) / m_n;
    }

    void push(const std::vector<T>& data)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        
        m_hasCapacity.wait(lock, [this, &data] {
            if(m_writePos < m_readPos) return (m_readPos - m_writePos) >= data.size();
            else return (m_n - m_writePos + m_readPos) >= data.size();
        });

        for(const T& el: data)
        {
            m_buffer[m_writePos] = std::move(el);
            m_writePos = (m_writePos + 1) % m_n;
        }

        m_hasData.notify_one();
    }

    void pop(std::vector<T>& data)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        using namespace std::chrono_literals;

        auto now = std::chrono::system_clock::now();
        bool ret = m_hasData.wait_until(lock, now + m_popTimeout*1ms, [this, &data] {
            if(m_readPos <= m_writePos) return (m_writePos - m_readPos) >= data.size();
            else return (m_n - m_readPos + m_writePos) >= data.size();
        });
        if(!ret)
        {
            throw std::runtime_error("Buffer::pop timed out");
        }

        for(T& el: data)
        {
            el = std::move(m_buffer[m_readPos]);
            m_readPos = (m_readPos + 1) % m_n;
        }

        m_hasCapacity.notify_one();
    }
};

#endif