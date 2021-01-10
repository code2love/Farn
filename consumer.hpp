#ifndef __CONSUMER_HPP
#define __CONSUMER_HPP

#include <vector>
#include "worker.hpp"
#include "buffer.hpp"

template<class T>
class Consumer : public Worker
{
    private:
    Buffer<T>& m_buffer;
    const unsigned int m_n;

    virtual bool consume(const std::vector<T>& data) = 0;

    bool step() override
    {
        std::vector<T> data(m_n);

        try
        {
            m_buffer.pop(data);
        }
        catch(std::runtime_error e)
        {
            return true;
        }

        return consume(data);
    }

    public:
    Consumer(Buffer<T>& buffer, unsigned int n = 1)
        : m_buffer(buffer), m_n(n)
    {
    }
};

#endif