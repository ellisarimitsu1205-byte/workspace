#include <atomic>
#include <mutex>
#include <cstdlib>
#include <thread>
#include <immintrin.h>

class Spinlock
{
private:
    std::atomic_flag flag_{};
public:
    void lock()
    {
        int backoff = 1;
        while(true)
        {
            while(flag_.test(std::memory_order_relaxed))
            {
                for(int i = 0; i < backoff; ++i)
                {
                    _mm_pause();
                } //
                backoff = std::min(backoff * 2, 1024);  //cap it
            }

            if (!flag_.test_and_set(std::memory_order_acquire))
                break;
        }
    }
    void unlock()
    {
        flag_.clear(std::memory_order_release);
    }
};