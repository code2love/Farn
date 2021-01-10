#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <atomic>
#include <chrono>
#include <thread>
#include "farn_consumer.hpp"
#include "farn_producer.hpp"


int main()
{
    // Parameters
    const int img_w = 10000;
    const int img_h = 20000;
    const long long n_points = 2e10;
    const unsigned int n_producers = 10;
    const unsigned int n_consumers = 2;
    const unsigned int cs_producer = 20;
    const unsigned int cs_consumer = 500;

    // Storage for producers and consumers as well as buffer memory
    std::vector<std::unique_ptr<Worker>> workers;
    Buffer<farn_point_t> buffer(2000);
    
    // Create producers
    for(int i=0; i < n_producers; i++) 
        workers.push_back(std::unique_ptr<Worker>(new FarnProducer(buffer, n_points / n_producers, cs_producer)));

    // Create consumers
    for(int i=0; i < n_consumers; i++) 
        workers.push_back(std::unique_ptr<Worker>(new FarnConsumer<img_w, img_h>(buffer, cs_consumer)));

    // Start consumers and producers
    for(auto& worker:workers)
    {
        worker->start();
    }

    // Monitoring of buffer usage and progress
    std::atomic<bool> stop_flag(false);
    std::thread resources_watcher([&stop_flag, &buffer, &workers, n_points] {
        const int period = 5;
        long long prev_remaining_points = n_points;
        auto t1 = std::chrono::high_resolution_clock::now();
        for(;;)
        {
            using namespace std::chrono_literals;

            // Slow down thread
            std::this_thread::sleep_for(period * 1s);

            // Get current execution time
            auto t2 = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

            // Calculate progress
            long long remaining_points = 0;
            for(std::unique_ptr<Worker>& el:workers)
            {
                FarnProducer* p_el = dynamic_cast<FarnProducer*>(el.get());
                if(p_el != NULL) remaining_points += p_el->getRemainingPoints();
            }
            float progress = 1 - (float)remaining_points / n_points;

            // Calculate points per second
            float pps = (float)(prev_remaining_points - remaining_points) / period;
            prev_remaining_points = remaining_points;

            // Output
            std::cout << std::fixed << std::setprecision(3) 
                      << "[" << duration << "]" << std::endl 
                      << "\tbuffer load: \t"     << buffer.getLoad() << std::endl
                      << "\tpoints:      \t"     << (n_points-remaining_points) << " / " << n_points << std::endl
                      << "\tpoints / sec:\t"     << pps << std::endl
                      << "\tprogress:    \t"     << progress << std::endl
                      << std::endl;

            // Save current farn image
            FarnConsumer<img_w, img_h>::saveImage("farn.png");

            if(stop_flag) return;
        }
    });

    // Wait for all workers to stop
    for(auto& worker:workers)
    {
        worker->stop(false);
    }

    // Stop monitoring thread
    stop_flag = true;
    resources_watcher.join();

    return 0;
}