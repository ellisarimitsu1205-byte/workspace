#!/usr/bin/env bash
set -e
FLAGS="-std=c++17 -O2 -pthread -Wall -Wno-interference-size"
g++ $FLAGS test_spsc.cpp  -o test
g++ $FLAGS bench_spsc.cpp -o bench
echo "built ./test_ and ./bench"