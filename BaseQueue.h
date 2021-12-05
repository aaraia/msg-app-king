#ifndef H_BASEQUEUE_H
#define H_BASEQUEUE_H



//  stl
#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>

//  app

namespace MessageService {

template<typename Task>
class BaseQueue
{
    //  limits
    static const uint32_t MAX_WORKERS = 5;
    static const uint32_t MAX_QUEUE_SIZE = 50;

    //  no move/copy for now
    BaseQueue& operator=(BaseQueue&&) = delete;
    BaseQueue& operator=(const BaseQueue&) = delete;
    BaseQueue(BaseQueue&&) = delete;
    BaseQueue(const BaseQueue&) = delete;

public:
    explicit BaseQueue(const uint32_t numWorkers = 1);
    virtual ~BaseQueue();

    void push(Task task);
    bool empty() const;

    virtual void shutdown();

private:
    virtual void work(Task task) = 0;

protected:
    std::queue<Task> m_queue;
    std::vector<std::thread> m_workers;
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    std::atomic_bool m_shutdown = false;
    std::atomic_uint32_t m_availableWorkers;
};


template <typename Task>
BaseQueue<Task>::BaseQueue(const uint32_t numWorkers)
{
    //  limit the number of worker to avoid unrealistic values
    //  keep a record of the number of workers we started with
    auto count = std::min(numWorkers, MAX_WORKERS);

    m_availableWorkers = count;

    //  create workers
    for (std::size_t i = 0; i < count; ++i) 
    {

        m_workers.emplace_back(std::thread{ [this]() {
            while (true) 
            {
                //  acquire the lock and loop
                std::unique_lock<std::mutex> lock{ m_mutex };

                while (m_queue.empty() && !m_shutdown) 
                {
                    //  check for spurious wakes using the below lambda
                    //  release the lock and sleep if there is no task to process
                    m_condition.wait(lock, [this] { return (!m_queue.empty() || m_shutdown); });
                }

                //  check that this wake was not triggered by a shutdown
                if (m_shutdown) return;

                try {
                    //  if we reached here then we can do some work
                    Task task = m_queue.front();
                    m_queue.pop();
                    //  no longer need the lock, so unlock
                    lock.unlock();

                    //  delgate to the derived function
                    work(task);
                }
                catch (...)
                {
                    //  handle any side effects here
                    //  do not let exceptions propgate out of a thread
                    //  TODO: pass exception outside the thread and handle there too
                    //  the mutex will be released when we return
                    --m_availableWorkers;
                    return;
                }
            }
        } });
    }
}

template <typename Task>
BaseQueue<Task>::~BaseQueue()
{
    //  let all threads know that we are shutting down
    //  wake all workers
    m_shutdown = true;
    m_condition.notify_all();

    //  join all threads
    for (auto& worker : m_workers) 
    {
        //  must be joinable, might have already stopped
        if (worker.joinable()) 
        {
            worker.join();
        }
    }
}

template <typename Task>
void BaseQueue<Task>::push(Task task)
{
    //  we have intitiated shutdown, no point in adding tasks
    if (m_shutdown) return;

    //  acquire lock, add task, unlock 
    std::unique_lock<std::mutex> lock{ m_mutex };

    //  are there any workers? no, reject work
    if (m_availableWorkers == 0) return;

    //  ideally we would start up more resources to deal with this
    //  but for now lets do some sort of rate limiting to prevent using too much resources
    //  drop some of the older tasks
    while (m_queue.size() >= MAX_QUEUE_SIZE) 
    {
        m_queue.pop();
    }

    //  move the tasks into the queue
    m_queue.emplace(task);

    //  no longer need lock as we are finsihed with the queue, so unlock
    lock.unlock();

    //  notify the workers
    m_condition.notify_one();
}

template <typename Task>
bool BaseQueue<Task>::empty() const
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    bool isEmpty = m_queue.empty();
    lock.unlock();
    return isEmpty;
}

template <typename Task>
void BaseQueue<Task>::shutdown()
{
    m_shutdown = true;
    m_condition.notify_all();
}

}

#endif
