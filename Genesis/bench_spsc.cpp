#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sched.h>
#include <pthread.h>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <thread>
#include <vector>
#include <chrono>
#include <algorithm>

#ifdef __cpp_lib_hardware_interference_size
  constexpr std::size_t CL = std::hardware_destructive_interference_size;
#else
  constexpr std::size_t CL = 64;
#endif

struct Padded { alignas(CL) std::atomic<std::size_t> head{0};
                alignas(CL) std::atomic<std::size_t> tail{0}; };
struct Packed { std::atomic<std::size_t> head{0};
                std::atomic<std::size_t> tail{0}; };
 
template <class Idx>
struct Queue {
    explicit Queue(std::size_t cap): capacity_(cap+1), buf_(capacity_) {}
    bool push(uint64_t v){
        auto h = idx_.head.load(std::memory_order_relaxed);
        auto n = (h+1)%capacity_;
        if(n==idx_.tail.load(std::memory_order_acquire)) return false;
        buf_[h]=v; idx_.head.store(n,std::memory_order_release); return true;
    }
    bool pop(uint64_t& o){
        auto t = idx_.tail.load(std::memory_order_relaxed);
        if(t==idx_.head.load(std::memory_order_acquire)) return false;
        o=buf_[t]; idx_.tail.store((t+1)%capacity_,std::memory_order_release); return true;
    }
    std::size_t capacity_; std::vector<uint64_t> buf_; Idx idx_;
};
 
static bool pin(std::thread& t, int core){
    cpu_set_t set; CPU_ZERO(&set); CPU_SET(core, &set);
    return pthread_setaffinity_np(t.native_handle(), sizeof(set), &set) == 0;
}
 
template <class Idx>
static double run_once(uint64_t N, int pcore, int ccore, bool& pin_ok){
    Queue<Idx> q(1024);
    std::atomic<bool> go{false};
    std::thread p([&]{ while(!go.load()){} for(uint64_t i=0;i<N;++i) while(!q.push(i)){} });
    std::thread c([&]{ while(!go.load()){} uint64_t v,got=0; while(got<N) if(q.pop(v)) got++; });
    bool a = pin(p, pcore), b = pin(c, ccore);
    pin_ok = a && b;
    auto t0 = std::chrono::steady_clock::now();
    go.store(true);                     
    p.join(); c.join();
    auto t1 = std::chrono::steady_clock::now();
    double sec = std::chrono::duration<double>(t1-t0).count();
    return N/1e6/sec;                    
}
 
template <class Idx>
static double bench(const char* label, uint64_t N, int runs, int pcore, int ccore){
    bool pin_ok=false;
    run_once<Idx>(N/4, pcore, ccore, pin_ok);         
    std::vector<double> v;
    for(int i=0;i<runs;++i){ bool ok; v.push_back(run_once<Idx>(N, pcore, ccore, ok)); }
    std::sort(v.begin(), v.end());
    double med = v[v.size()/2];
    std::printf("%-8s median %6.1f M ops/s   (min %.1f, max %.1f, n=%d)  pinned=%s\n",
                label, med, v.front(), v.back(), runs, pin_ok?"yes":"NO(!)");
    return med;
}
 
int main(int argc, char** argv){
    int pcore = argc>1 ? std::atoi(argv[1]) : 0;
    int ccore = argc>2 ? std::atoi(argv[2]) : 2;
    uint64_t N = (argc>3 ? std::strtoull(argv[3],nullptr,10) : 200) * 1'000'000ULL;
    int runs  = argc>4 ? std::atoi(argv[4]) : 7;
 
    int ncpu = (int)std::thread::hardware_concurrency();
    std::printf("logical CPUs visible: %d   |  producer->core %d, consumer->core %d\n",
                ncpu, pcore, ccore);
    if(pcore>=ncpu || ccore>=ncpu)
        std::printf("WARNING: requested core >= CPU count; affinity will fail. Pick valid cores.\n");
    if(pcore==ccore)
        std::printf("WARNING: both threads on the SAME core -> no cross-core sharing to measure.\n");
    std::printf("(check `lscpu -e` for which cores are hyperthread siblings; use two PHYSICAL cores)\n\n");
 
    double padded = bench<Padded>("padded", N, runs, pcore, ccore);
    double packed = bench<Packed>("packed", N, runs, pcore, ccore);
    std::printf("\npadded / packed = %.2fx  ", padded/packed);
    if(padded > packed*1.10)      std::printf("-> padding WINS (false sharing was real)\n");
    else if(packed > padded*1.10) std::printf("-> packing wins here (these two cores share a cache, or noise)\n");
    else                          std::printf("-> basically equal (no measurable false sharing on this pair)\n");
}