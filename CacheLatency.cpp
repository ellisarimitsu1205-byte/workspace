#include <iostream>
#include <vector>
#include <chrono>


//**
// In HFT a cold L1 cache miss means data is not already in the L1 cache
// The teask is the write program that estimates the average latency in nanoseconds */


auto MeasureL1CacheLatency() -> double
{
    constexpr auto CacheLineSize = std::hardware_destructive_interference_size;
    constexpr auto EvictCacheLineSize = CacheLineSize * (2 << 12);
    constexpr auto Trials = 20'000;
    std::vector<uint8_t> CacheLineData(CacheLineSize), CacheLineEvictData(EvictCacheLineSize);
    volatile uint64_t sink = 0;
    
    auto floodTime =  0.0;
    using namespace std::chrono;

    const auto startTrialing = steady_clock::now();


    for(auto trials{0uz}; trials < Trials; ++trials)
    {
        auto floodStart = steady_clock::now();
        for (auto i{0uz}; i < CacheLineEvictData.size(); ++i)
        {
            sink += CacheLineEvictData[i];
        }
        auto floodEnd = steady_clock::now();
        const auto iterationFloodTime = duration_cast<nanoseconds>(floodEnd - floodStart).count();
        floodTime += static_cast<double>(iterationFloodTime);

        sink += CacheLineData[0];
    }

    const auto endTrialing = steady_clock::now();
    const auto totalTrialTime = static_cast<double>(duration_cast<nanoseconds>(endTrialing - startTrialing).count());

    const auto averageFloodTime = floodTime / Trials;

    return averageFloodTime; 
}




int main()
{
    const auto averageLatency = MeasureL1CacheLatency();
    std::cout << "Average L1 cache latency (ns): " << averageLatency << std::endl;
    return 0;
}