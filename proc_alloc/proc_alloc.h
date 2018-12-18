#ifndef PROCAlloc_H
#define PROCAlloc_H
#include <vector>
#include <iostream>

#define MAX_PROC 8

class PROCAlloc{
    // std::vector<int> *proc_list = new std::vector<int>(MAX_PROC);
    int pid;
public:
    PROCAlloc(std::vector<int> &);
    void deallocate(std::vector<int> &);
    int getpid();
};

#endif