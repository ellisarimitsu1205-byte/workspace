#pragma once
#include <atomic>
#include <cstddef>
#include <new>
#include <vector>

#ifdef __cpp_lib_hardware_interference_size
    constexpr std::size_t CACHELINE = std::hardware_destructive_interference_size;
#else
    constexpr std::size_t CACHELINE = 64; //for x86-64 line sizes hehe
#endif


template <typename T>
class SpscQueue {
public:
    explicit SpscQueue(std::size_t capacity)
        : capacity_(capacity + 1), buffer_(capacity_)
    { }

    bool push(const T& item) {
        auto head = head_.load(std::memory_order_relaxed);
        auto next = increment(head);
        if(next == tail_.load(std::memory_order_acquire))
        {
            return false;
        }
        buffer_[head] = item;
        head_.store(next, std::memory_order_release);
        return true;
    }

    bool pop(T& out){
        auto tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire))
        {
            return false;
        }
        out = buffer_[tail];
        tail_.store(increment(tail), std::memory_order_release);
        return true;
    }

private:
    std::size_t increment(std::size_t i) const { return (i + 1) % capacity_; }
    std::size_t capacity_;
    std::vector<T> buffer_;
    alignas(CACHELINE) std::atomic<std::size_t> head_{0};
    alignas(CACHELINE) std::atomic<std::size_t> tail_{0};
};

