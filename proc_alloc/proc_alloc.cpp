#include <iostream>
#include "proc_alloc.h"
#include <vector>

using namespace std;

PROCAlloc::PROCAlloc(vector<int> &proc_list){
    int index = rand() % proc_list.size(); // pick a random index
    pid = proc_list[index]; // a random value taken from that list
    proc_list.erase(proc_list.begin() + index);
}

void PROCAlloc::deallocate(vector<int> &proc_list){
    proc_list.push_back(pid);
}

int PROCAlloc::getpid(){
    return pid;
}