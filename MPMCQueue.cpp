#include <cstdlib>
#include <atomic>

template<typename T, size_t N>
class MPMCQueue
{
private:
    struct Slot
    {
        std::atomic<size_t> seq_;
        T data_;
    };

    alignas(64) Slot slots_[N];
    alignas(64) std::atomic<std::size_t> head_{};
    alignas(64) std::atomic<std::size_t> tail_{};

public:
    MPMCQueue() {
        static_assert((N & (N - 1)) == 0, "N must be a power of two");
        for (size_t i = 0; i < N; ++i)
        {
            slots_[i].seq_.store(i, std::memory_order_relaxed);
        }
    }
        bool push(const T& val) {
    auto tail = tail_.load(std::memory_order_acquire);
    while (true) {
        size_t seq = slots_[tail % N].seq_.load(std::memory_order_acquire);
        int64_t diff = (int64_t)seq - (int64_t)tail;

        if (diff == 0) {
            if (tail_.compare_exchange_weak(tail, tail + 1, std::memory_order_acq_rel))
                break;
        } else if (diff < 0) {
            return false;  // full
        } else {
            tail = tail_.load(std::memory_order_relaxed);
        }
    }
    slots_[tail % N].data_ = val;
    slots_[tail % N].seq_.store(tail + 1, std::memory_order_release);
    return true;
}

        bool pop(T& val)
        {
            //we load head
            auto head = head_.load(std::memory_order_acquire);
            //CAS LOOP
            while(true)
            {
                size_t seq = slots_[head % N].seq_.load(std::memory_order_acquire);
                int64_t diff = (int64_t)seq - (int64_t)(head + 1);
                if(diff == 0)
                {
                    if(head_.compare_exchange_weak(head, head + 1, std::memory_order_acq_rel)){
                        break;
                    } 
                } else if (diff < 0) {
                        return false;
                } else {
                        head = head_.load(std::memory_order_relaxed);
                }
            }
            val = slots_[head % N].data_;
            slots_[head % N].seq_.store(head + N, std::memory_order_release);
            return true;


        }
};