#pragma once

#include <chrono>
#include <iostream>

/**
Measures the time execution of a callable simple implementation.
*/

void TimeExecution(auto&& func)
{
    const auto start = std::chrono::steady_clock::now();
    func();
    const auto end = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "Execution time: " << duration << " ms" << std::endl;
}