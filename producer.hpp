#ifndef __PRODUCER_HPP
#define __PRODUCER_HPP

#include <vector>
#include "worker.hpp"
#include "buffer.hpp"

template<class T>
class Producer : public Worker
{
    private:
    Buffer<T>& m_buffer;
    const unsigned int m_n;

    virtual bool produce(std::vector<T>& data) = 0;

    bool step() override
    {
        std::vector<T> data(m_n);

        bool ret = produce(data);

        m_buffer.push(data);

        return ret;
    }

    public:
    Producer(Buffer<T>& buffer, unsigned int n = 1)
        : m_buffer(buffer), m_n(n)
    {
    }
};

#endif