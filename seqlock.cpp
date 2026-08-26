#include <atomic>
#include <array>
#include <cstddef>

struct MarketDataTick
{
    uint16_t bid{};
    uint16_t ask{};
    uint32_t timestamp{};
};

template<typename T>
class SeqLock
{
private:
    alignas(64) std::atomic<std::size_t> seq_{ 0 };
    alignas(64) T tick_{};
public:

    void write(const T& tick)
    {
        seq_.fetch_add(1, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_release);
        tick_ = tick;
        seq_.fetch_add(1, std::memory_order_release);
    }

    T read()
    {
        T tick{};
        uint64_t seq0, seq1;
        do
        {
            seq0 = seq_.load(std::memory_order_acquire);
            if(seq0 & 1)
            {
                continue;
            }
            tick = tick_;
            seq1 = seq_.load(std::memory_order_acquire);
        } while(seq0 != seq1);
        return tick;
    }

};