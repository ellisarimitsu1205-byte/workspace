#include <cstdlib>
#include <algorithm>

class Buffer
{
private:
    int* data_;
    size_t size_;

public:
    Buffer(size_t n)
        : data_(new int[n]), size_(n) 
    { }


    ~Buffer()
    {
        delete[] data_;
    }
    Buffer(const Buffer& other)
        : data_(new int[other.size_]), size_(other.size_)
    {
        std::copy(other.data_, other.data_ + other.size_, data_);
    }

    Buffer& operator=(const Buffer& other)
    {
        if(this == &other) return *this;
        delete[] data_;
        data_ = new int[other.size_];
        size_ = other.size_;
        std::copy(other.data_, other.data_ + other.size_, data_);
        return *this;
    }
    Buffer(Buffer&& other)
        : data_(other.data_), size_(other.size_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
    }
    Buffer& operator=(Buffer&& other) noexcept
    {
        if(this == &other) return *this;
        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;
        other.data_ = nullptr;
        other.size_ = 0;
        return *this; 
    }
};