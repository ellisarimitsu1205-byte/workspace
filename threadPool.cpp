#include <thread>
#include <vector>
#include <queue>
#include <functional> 
#include <mutex>
#include <condition_variable>



class ThreadPool
{
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool stop_ = false;
public:
    ThreadPool(size_t n)
    {
        for(size_t i = 0; i < n; ++i)
        {
            workers_.emplace_back([this]
            {
                while(true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mtx_);
                    cv_.wait(lock, [this] {
                        return !tasks_.empty() || stop_;
                    });
                    if(stop_ && tasks_.empty())
                    {
                        return;
                    }
                    task = std::move(tasks_.front());
                    tasks_.pop();

                }
                task();
                }
            });
        }
    }


    void submit(std::function<void()> task)
    {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            tasks_.push(std::move(task));
        }
        cv_.notify_one();
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            stop_ = true;
        }
        cv_.notify_all();
        for(auto& t : workers_)
        {
            if (t.joinable()) t.join();
        }
    }
};