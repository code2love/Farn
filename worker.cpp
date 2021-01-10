#include "worker.hpp"

void Worker::work()
{
    while(!m_terminate)
    {
        if(step()) return;
    }
}

bool Worker::start()
{
    if(m_running) return false;

    m_thread = std::move(std::thread(&Worker::work, this));
    m_running = true;

    return true;
}

void Worker::stop(bool force)
{
    if(force) m_terminate = true;
    m_thread.join();
}

Worker::Worker()
    : m_terminate(false), m_running(false)
{
}

Worker::~Worker()
{
    if(m_thread.joinable())m_thread.join();
}