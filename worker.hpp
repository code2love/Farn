#ifndef __WORKER_HPP
#define __WORKER_HPP

#include <thread>
#include <atomic>

class Worker
{
    private:
    std::thread m_thread;
    std::atomic<bool> m_terminate;
    bool m_running;

    void work();

    protected:
    virtual bool step() = 0;

    public:
    bool start();
    void stop(bool force);

    Worker();
    virtual ~Worker();
};


#endif