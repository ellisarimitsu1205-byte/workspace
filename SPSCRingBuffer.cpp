#include <atomic>
#include <cstdlib>
#include <array>

template<typename T, size_t Capacity>
class SPSCQueue
{
private:
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");
    static constexpr size_t mask = Capacity - 1;
    alignas(64) std::atomic<std::size_t> head_{ 0 };
    alignas(64) std::atomic<std::size_t> tail_{ 0 };
    alignas(64) std::array<T, Capacity> buffer_;

public:
    bool push(const T& item)
    {
        const auto tail = tail_.load(std::memory_order_relaxed);
        const size_t next = (tail + 1) & mask; 

        if(next == head_.load(std::memory_order_acquire))
        {
            return false;
            //tail + 1 = head we gone in circle
        }

        buffer_[tail] = item;
        tail_.store(next, std::memory_order_release);
        return true; 
    }

    bool pop(T& value)
    {
        auto head = head_.load(std::memory_order_relaxed);

        if(head == tail_.load(std::memory_order_acquire))
        {
            return false; //empty
        }

        value = buffer[head];
        head_.store((head + 1) & mask, std::memory_order_release);
        return true;
    }
};