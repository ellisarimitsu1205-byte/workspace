#include <cstdlib>
#include <memory>
#include <iostream>
#include <atomic>
#include <vector>

template <typename T, size_t N>
class MemoryPool 
{
private:
    struct Slot {
        alignas(T) char data[sizeof(T)];
        Slot* next = nullptr;
    };

   Slot  slots_[N];
   std::atomic<Slot*> free_list_ = nullptr;
public:
    MemoryPool() 
    {
        for(size_t i = 0; i < N - 1; ++i)
        {
            slots_[i].next = &slots_[i + 1];
        }
        slots_[N-1].next = nullptr;
        free_list_ = &slots_[0];
    }

    T* allocate()
    {
        //
        Slot* candidate = free_list_.load(std::memory_order_acquire);
        do
        {
            if (candidate == nullptr) return nullptr; //full
        }    while (!free_list_.compare_exchange_weak(
                candidate,
                candidate->next,
                std::memory_order_release,
                std::memory_order_acquire
            ));
        return reinterpret_cast<T*>(candidate->data);
    }

    void deallocate(T* ptr)
    {
        ptr->~T();
        Slot* slot = reinterpret_cast<Slot*>(ptr);
        slot->next = free_list_.load(std::memory_order_acquire);
        while(!free_list_.compare_exchange_weak(
            slot->next,
            slot,
            std::memory_order_release,
            std::memory_order_acquire
        ));
    }
};

struct Order {
    uint64_t id;
    double price;
    uint32_t qty;
};

//Making it STL compatable

template<typename T, size_t N> 
struct PoolAllocator 
{
    using value_type = T;

    MemoryPool<T, N>* pool_;

    PoolAllocator(MemoryPool<T, N>& pool) : pool_(&pool)
    { }

    T* allocate(size_t N)
    {
        assert(n == 1);
        T* ptr = pool_->allocate();
        if(ptr == nullptr) throw std::bad_alloc{};
        return ptr;
    }

    void deallocate(T* p, size_t n)
    {
        pool_->deallocate(p);
    }
};


//usage

int main()
{
    MemoryPool<Order,10> memPool{};

    PoolAllocator<Order, 10> poolAlloc(memPool);

    std::vector<Order, PoolAllocator<Order,10>> v{poolAlloc};

    v.reserve(10); //never resize on hot path;

    v.push_back(Order{1,10.0,100});

    return 0;
}