#include <cstdlib>
#include <atomic>
#include <functional>
#include <thread>


template <typename T>
struct DataWrapper
{
    T data{};
    bool is_last_chunk{ false };
};

template <typename T>
struct Node
{
    DataWrapper wrapper{};
    std::atomic<Node*> next{ nullptr };
};


template <typename T, typename Callback>
class SPSCQueue
{
private:
    Node<T>* dummy = new Node<T>();
    Node* head_ = dummy;
    Node* tail_ = dummy;
    std::function<void(DataWrapper<T>)> callback_;
    std::thread consumerThread_(&SPSCQueue::Consume, this);

public:
    SPSCQueue(std::function<void(DataWrapper<T>)> callback)
        : callback_(std::move(callback))
    { }
   
    ~SPSCQueue()
    {
        if(consumerThread_.joinable())
        {
            consumerThread_.join();
        }
        while(head_)
        {
            Node<T>* next = head->next.load(std::memory_order_relaxed);
            delete head_;
            head_ = next;
        }
    }

    SPSCQueue(const SPSCQueue&) = delete;
    SPSCQueue& operator=(const SPSCQueue&) = delete;
    SPSCQueue(SPSCQueue&&) = delete;
    SPSCQueue& operator=(SPSCQueue&&) = delete;


    void push(DataWrapper<T> data)
    {
        //there is norhing i nthe queue yet 
        Node<T>* n = new Node<T>;
        n->wrapper = data;
        tail_->next.store(n, std::memory_order_release);
        tail_ = n;
    }

    void Consume()
    {
        while(true)
        {
            auto next = head_->next.load(std::memory_order_acquire);
            if(next)
            {
                callback_(next->wrapper);
                delete head_;
                head_ = next;
                if(next->wrapper.is_last_chunk)
                {
                    break;
                }
            }
            else 
            {
                std::this_thread::yield();
            }
        }
    }





};
