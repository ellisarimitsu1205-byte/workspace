#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <algorithm>
#include <limits>
#include <thread>
#include <cmath> 
#include <chrono>

constexpr size_t DATASET_SIZE = 5000000;
//I dont want to fucking make this over and over again lol
struct Timer
{
    std::chrono::steady_clock::time_point start{};
    std::chrono::steady_clock::time_point end{};

    void Pin()
    {
        start = std::chrono::steady_clock::now();
    }

    auto Peek()
    {
        end = std::chrono::steady_clock::now();
        auto duration = end - start;
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }
};

void ProcessDataset(std::array<int, DATASET_SIZE>& set)
{
    for (int x : set)
    {
        constexpr auto limit = (double)std::numeric_limits<int>::max();
        const auto y = (double)x / limit;
        set[0] += int(std::sin(std::cos(y)) * limit);
    }
}

int main()
{
    std::minstd_rand rne;
    std::vector<std::array<int, DATASET_SIZE>> datasets{ 4 };
    std::vector<std::thread> workers;
    Timer t;
    
    for(auto& arr : datasets)
    {
        std::ranges::generate(arr, rne);
    }
    t.Pin();
    for(auto& set : datasets)
    {
        auto w = [&set] {
            for (int x : set)
            {
                constexpr auto limit = (double)std::numeric_limits<int>::max();
                const auto y = (double)x / limit;
                set[0] += int(std::sin(std::cos(y)) * limit);
            }
        };
        workers.push_back(std::thread{ ProcessDataset, std::ref( set ) });
    }

    for (auto& w : workers)
    {
        if(w.joinable())
        {
            w.join();
        }
    }
    auto ms = t.Peek();

    std::cout << "Processing the database took: " << ms << " milliseconds!\n";
    return 0;
}