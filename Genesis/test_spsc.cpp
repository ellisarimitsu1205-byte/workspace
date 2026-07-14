#include "spsc.hpp"
#include <thread>
#include <cstdio>
#include <cstdint>

int main()
{
    constexpr uint64_t N = 5'000'000;
    SpscQueue<uint64_t> q(1024);

    std::thread producer([&]{
        for (uint64_t i = 0; i < N; ++i)
        {
            while (!q.push(i)) { }
        }
    });

    uint64_t expected = 0, sum = 0;
    bool ordered = true;
    std::thread consumer([&]{
        uint64_t v, got = 0;
        while(got < N) 
        {
            if (q.pop(v))
            {
                if(v != expected)
                {
                    ordered = false;
                }
                expected++; 
                sum += v; 
                got++;
            }
        }
    });

    producer.join(); 
    consumer.join();
    uint64_t want = (N-1)*N/2;
    std::printf("ordered=%s  sum=%llu  want=%llu  %s\n", ordered ? "yes":"NO", (unsigned long long)sum, (unsigned long long)want, (ordered && sum==want) ? "PASS":"FAIL");
    return (ordered && sum==want) ? 0 : 1;
}