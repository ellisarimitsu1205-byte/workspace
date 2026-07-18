#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <algorithm>
#include <limits>
#include <thread>
#include <cmath> 
#include <chrono>
#include <mutex>
#include <span>
#include <condition_variable>

constexpr size_t DATASET_SIZE = 50000000;
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

void ProcessDataset(std::span<int> arr, int& sum)
{
    for (int x : arr)
    {
        constexpr auto limit = (double)std::numeric_limits<int>::max();
        const auto y = (double)x / limit;
        sum += int(std::sin(std::cos(y)) * limit);
    }
}

std::vector<std::array<int, DATASET_SIZE>> GenerateDatasets()
{
    std::minstd_rand rne;
    std::vector<std::array<int, DATASET_SIZE>> datasets{ 4 };
    for (auto& arr : datasets)
    {
        std::ranges::generate(arr, rne);
    }
    return datasets;
}

struct Value
{
    int v{ 0 };
    char padding[60];
};


int DoBiggie()
{
	auto datasets = GenerateDatasets();
    std::vector<std::thread> workers;
    Timer t;

    Value sum[4];

    t.Pin();
    for(size_t i = 0; i < datasets.size(); ++i)
    {
        workers.push_back(std::thread{ ProcessDataset, std::span{ datasets[i] }, std::ref(sum[i].v) });
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
	std::cout << "The sum of the processed data is: " << sum[0].v + sum[1].v + sum[2].v + sum[3].v << "\n";
    return 0;
}

class MasterControl
{
public:
    MasterControl(int workerCount)
        : lk{ mtx }, workerCount{ workerCount }
    { }
    void SignalDone()
    {
        {
			std::lock_guard<std::mutex> lk{ mtx };
			++doneCount;
        }
		if (doneCount == workerCount)
		{
			cv.notify_one();
		}
    }
    void WaitForAllDone()
    {
        cv.wait(lk, [this] {return doneCount == workerCount; });
        doneCount = 0;
    }
private:
	std::condition_variable cv;
	std::mutex mtx;
	std::unique_lock<std::mutex> lk;
    int workerCount{ 0 };
	int doneCount{ 0 };

};

class Worker
{
public:
    Worker(MasterControl* pMaster)
        : pMaster{ pMaster },
		thread{ &Worker::Run_, this } //pass a pointer to the member function and the object instance
    { }
    void SetJob(std::span<int> data, int* pOut)
    {
        {
            std::lock_guard lk{ mtx };
            input = data;
            pOutput = pOut;
        }
		cv.notify_one();
    }
    void Kill()
    {
        {
			std::lock_guard lk{ mtx };
			dying = true;
        }
        cv.notify_one();
    }
private:
    void Run_()
    {
		std::unique_lock<std::mutex> lk{ mtx };
		while (true)  //polling loop, but we can use a condition variable to avoid busy waiting
        {
            cv.wait(lk, [this] {return pOutput != nullptr || dying; });
			if (dying)
			{
				break;
			}
            ProcessDataset(input, *pOutput);
            pOutput = nullptr;
			input = {};
            pMaster->SignalDone();
        }
    }
    MasterControl* pMaster;
    std::jthread thread;
	std::condition_variable cv;
    std::mutex mtx;
    //shared memory
    std::span<int> input;
    int* pOutput{ nullptr };
	bool dying{ false };

};

int DoSmallies()
{
	auto datasets = GenerateDatasets();
	Timer t;
    Value sum[4];
	
    constexpr size_t workerCount = 4;
	MasterControl mctrl{ workerCount };
    std::vector<std::unique_ptr<Worker>> workerPtrs; //need stable place in memory
    std::vector<std::jthread> workers;
    t.Pin();
	for (int i = 0; i < workerCount; ++i)
	{
		workerPtrs.push_back(std::make_unique<Worker>(&mctrl));
	}

	constexpr const auto subsetSize = DATASET_SIZE / 10'000;
	for (size_t i = 0; i < DATASET_SIZE; i += subsetSize)
	{
        std::vector<std::jthread> workers;
        for (size_t j = 0; j < 4; ++j)
        {
			workerPtrs[j]->SetJob(std::span{ &datasets[j][i], subsetSize }, &sum[j].v);
        }
        mctrl.WaitForAllDone();
	}
	auto ms = t.Peek();
	std::cout << "Processing the database for smallsies took: " << ms << " milliseconds!\n";
	std::cout << "The sum of the processed data for smallies is: " << sum[0].v + sum[1].v + sum[2].v + sum[3].v << "\n";
    
    for (auto& w : workerPtrs)
    {
        w->Kill();
    }
    workerPtrs.clear();

    return 0;
}

int main()
{   
    return DoSmallies();
}