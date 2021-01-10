#ifndef __FARN_CONSUMER_HPP
#define __FARN_CONSUMER_HPP

#define cimg_display 0

#include <string>
#include <vector>
#include <mutex>
#include <algorithm>
#include "CImg.h"
#include "consumer.hpp"
#include "farn_common.hpp"

template<int W=10000, int H=20000>
class FarnConsumer : public Consumer<farn_point_t>
{
    private:
    static const int m_pixels_per_mutex;
    static std::vector<std::mutex> m_mutexes;
    static cimg_library::CImg<unsigned char> m_img;

    bool consume(const std::vector<farn_point_t>& data) override
    {
        for(const farn_point_t& p:data)
        {
            float x = (p.first + 2.2) / 4.9 * W;
            float y = (1 - p.second / 10) * H;
            unsigned int pixel_i = x * H + y;
            unsigned int mutex_i = pixel_i / m_pixels_per_mutex;

            std::mutex& mutex = m_mutexes[mutex_i];
            std::unique_lock<std::mutex> lock(mutex);
            
            unsigned char* pixel = m_img.data(x,y);
            if(*pixel != 255) *pixel = *pixel + 1;
        }
        return false;
    }

    public:
    FarnConsumer(Buffer<farn_point_t>& buffer, unsigned int chunk_size = 1)
        : Consumer(buffer, chunk_size)
    {
    }

    static void saveImage(std::string filename, bool reset=false)
    {
        // Lock all mutexes such that we can save the image without interference
        std::vector<std::unique_lock<std::mutex>> locks;
        for(auto& mutex:m_mutexes) locks.push_back(std::move(std::unique_lock<std::mutex>(mutex)));

        m_img.save_png(filename.c_str());
        if(reset) m_img.fill(255);
    }
};

template<int W, int H>
const int FarnConsumer<W,H>::m_pixels_per_mutex = 1000;

template<int W, int H>
cimg_library::CImg<unsigned char> FarnConsumer<W,H>::m_img = cimg_library::CImg<unsigned char>(W,H,1,1,0);

// Use one mutex per 1000 pixel block
template<int W, int H>
std::vector<std::mutex> FarnConsumer<W,H>::m_mutexes = std::vector<std::mutex>((W * H) / FarnConsumer<W,H>::m_pixels_per_mutex);

#endif