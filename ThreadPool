#ifndef THREADPOOL_H
#define THREADPOOL_H


#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>


class ThreadPool
{
public:
    ThreadPool(size_t size):
        Stop(false)
    {
        for(size_t i = 0; i < size; ++i)
        {
            Works.emplace_back([this]{
                for (;;)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(Mutex);
                        Condition.wait(lock, [this]{ return Stop || !Tasks.empty();});
                        if(Stop && Tasks.empty())
                            return ;
                        task = std::move(Tasks.front());
                        Tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(Mutex);
            Stop = true;
        }

        Condition.notify_all();
        for(std::thread & worker: Works)
            worker.join();
    }



    template<class T, class... Args>
    auto Enqueue(T&& t, Args&&... args)->std::future<typename std::result_of<T(Args...)>::type>
    {
        using return_type = typename std::result_of<T(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<T>(t), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();

        {
            std::unique_lock<std::mutex> lock(Mutex);

            if(Stop)
                throw std::runtime_error("Enqueue on stopped ThreadPool");

            Tasks.emplace([task](){(*task)();});
        }

        Condition.notify_one();

        return res;
    }

private:
    bool Stop;

    std::vector<std::thread> Works;
    std::queue<std::function<void()>> Tasks;

    std::mutex Mutex;
    std::condition_variable Condition;
};


#endif // THREADPOOL_H
