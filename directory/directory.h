#ifndef DIRECTORY_H
#define DIRECTORY_H
#include "../cpu_avail/cpu_avail.h"
//#include "cpu.h"
#include <unordered_map>
#include<bits/stdc++.h> 
#include "../cache/cache.h"
#include <mutex>

//no of cpu/cache
#define NO_CPU 3
class CPU;
//struct to store counters
using namespace std;
struct cache_counters
{
    int n_cache;
    int d_cache;
    int address;
    int dirty_val;
};
class Directory
{
    private:
        CPUAvail *cpu_avail;
        CPU *cpu;
        Cache *cache;
        static bool singleFlag;
        static Directory *dir;
        unordered_map<int,cache_counters> cache_map;
        Directory();
        friend class CPU;
    public:
        static Directory *getInstance();
        void notify(int pid,int addr,int new_val);
        void choose_cpu(int pid);
        void update_map(int pid,int address, int new_val);
        void finished_update(int pid);
        void finished_exec(int pid,int chosen_cpu);
};
#endif