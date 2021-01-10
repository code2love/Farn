#ifndef __FARN_PRODUCER_HPP
#define __FARN_PRODUCER_HPP

#include <random>
#include <mutex>
#include "producer.hpp"
#include "farn_common.hpp"

class FarnProducer : public Producer<farn_point_t>
{
    private:
    std::mutex m_mutex;
    std::random_device m_random;
    std::uniform_real_distribution<float> m_dist;
    farn_params_list_t m_params;
    farn_point_t m_x;
    long long m_remaining_points;

    bool produce(std::vector<farn_point_t>& data) override
    {
        for(farn_point_t& p:data)
        {
            float r = m_dist(m_random);
            float s = 0;
            int params_i = 0;

            // Draw random parameter set
            for(const farn_params_t& params:m_params)
            {
                s = s + params.p;
                if(r <= s) break;
                params_i = params_i + 1;
            }

            const farn_params_t& params = m_params[params_i];

            // Calculate new data point
            p.first  = params.a * m_x.first + params.b * m_x.second + params.e;
            p.second = params.c * m_x.first + params.d * m_x.second + params.f;

            m_x = p;

            std::unique_lock<std::mutex> lock(m_mutex);
            if(--m_remaining_points == 0) return true;
        }

        return false;
    }

    public:
    FarnProducer(Buffer<farn_point_t>& buffer, long long n_points, unsigned int chunk_size = 1, farn_params_list_t params = farn_params_list_t {
        {    0,     0,     0, 0.16, 0,  0,   0.01},
        { 0.85,  0.04, -0.04, 0.85, 0,  1.6, 0.85},
        {  0.2, -0.26,  0.23, 0.22, 0,  1.6, 0.07},
        {-0.15,  0.28,  0.26, 0.24, 0, 0.44, 0.07}
    })
        : m_x(0,0), Producer(buffer, chunk_size), m_remaining_points(n_points), m_params(params), m_dist(0,1)
    {
    }

    long long getRemainingPoints()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_remaining_points;
    }
};

#endif