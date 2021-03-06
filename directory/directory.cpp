#include <iostream>
#include <vector>
#include <string>
#include "directory.h"
#include<bits/stdc++.h> 
#include "../cpu/cpu.h"
#include "../cache/cache.h"
#include "../main_memory/main_memory.h"
#include <map>
#include <iterator> 

using namespace std;

bool Directory::singleFlag=false;
Directory* Directory::dir=NULL;

mutex dirMem;
Directory* Directory::getInstance()
{
    dirMem.lock();
    if(!singleFlag){
        // cout<<"New object created\n";
        dir = new Directory();
        singleFlag = true;
    }
    dirMem.unlock();
    return dir;
}
Directory::Directory()
{
    cpu_avail=cpu_avail->getInstance(NO_CPU);
    cpu=new CPU[NO_CPU];
    cache = new Cache[NO_CPU];
    //cout << "yayy"<<endl;
}
void Directory::choose_cpu(int pid)
{
    //cout <<"in dir:: choose cpu " << size <<endl;
    //if pid not in cache
    // dirMem.lock();
    if(cache_map.find(pid)==cache_map.end())
    {
        cout << "not found in cache :pid is "<<pid<<endl;
        cache_counters x;
        x.n_cache=1;
        x.d_cache=0;
        cache_map[pid]=x;
        //cout << "n_cache("<<pid<<") : "<< cache_map.at(pid).n_cache <<" d_cache : "<< cache_map.at(pid).d_cache <<endl;
        cout << "n cache "<<cache_map.at(pid).n_cache <<endl;
        cout << "d cache "<<cache_map.at(pid).d_cache <<endl;
    }
    else
    {
        cout << "found in cache :pid is "<<pid<<endl;
        cache_map.at(pid).n_cache+=1;
        cout << "n cache "<<cache_map.at(pid).n_cache <<endl;
        cout << "d cache "<<cache_map.at(pid).d_cache <<endl;
    }
    //choose cpu and call cpu
    int chosen_cpu=cpu_avail->choose_cpu();
    // dirMem.unlock();
    if(chosen_cpu == -1){
        exit(0);
    }
    cout << "Chosen CPU : "<<chosen_cpu <<endl;
    // Store in cache
    map<int, string> addrMap;
    map<int, string> c_addrMap;
    MainMemory *m1 = m1->getInstance();
    string op = m1->getOp(pid);
    // cout<< "Operation : "<<op<<endl;

    bool incache=false;
    for(int i=0;i<NO_CPU;i++)
    {
        c_addrMap=cache[i].getAllData(pid);
        if(c_addrMap.size()!=0)
        {
            incache=true;
            break;
        }
    }
    if(incache==false)
    {
        addrMap = m1->getData(pid);
        cache[chosen_cpu].store(pid,addrMap);
    }
    else
    {
        cache[chosen_cpu].store(pid,c_addrMap);
    }
    // cache[chosen_cpu].display();

    // cout << "type : "<<typeid(cache[chosen_cpu]).name()<<endl;
    cpu[chosen_cpu].execute(pid,op,cache[chosen_cpu],chosen_cpu);

    //DEBUG : Print of address map
    // map<int, string>::iterator itr;
    // cout << "ADDRESS MAP\n";
    // for (itr = addrMap.begin(); itr != addrMap.end(); ++itr) { 
    //     cout << '\t' << itr->first 
    //          << '\t' << itr->second << '\n'; 
    // } 
    
    //cpu[chosen_cpu].execute(base_addr,size,pid);
}
//called by cpu when new value written into cache
void Directory::update_map(int pid,int addr, int new_val)
{
    // dirMem.lock();
    if(cache_map.find(pid)!=cache_map.end())
    {
        
        
        cache_map.at(pid).d_cache=cache_map.at(pid).n_cache;
        //update cache ref
        cout << "update map : d_cache is ";
        cout << cache_map.at(pid).d_cache << endl;

        if(cache_map.at(pid).d_cache == 1){
            cout <<endl<< "Consistent ---------------- Exiting" << endl;
            return;    // Don't notify if nobody else is depending on it.
        }
        cache_map.at(pid).address = addr;
        cache_map.at(pid).dirty_val=new_val;
        //notify all cpus
        notify(pid,addr,new_val);
    }
    // dirMem.unlock();
}
void Directory::notify(int pid,int addr,int new_val)
{
    for(int i=0;i<NO_CPU;i++)
        {
            cache[i].modify(pid,addr,new_val);
        }
}
void Directory::finished_exec(int pid,int chosen_cpu)
{
    // cout<<"Entered"<<endl;
    // dirMem.lock();
    // cout<<"Entered"<<endl;
    //found in cache map
    if(cache_map.find(pid)!=cache_map.end())
    {
        cache_map.at(pid).n_cache-=1;
        //cout << "finished exec Chosen CPU : "<<chosen_cpu <<endl;
        cpu_avail->set_avail(chosen_cpu);
        if(cache_map.at(pid).n_cache==0)
        {
            cache_map.erase(pid);
            MainMemory *m1 = m1->getInstance();
            m1->removemem(pid);
        }
    }
    // dirMem.unlock();
}
//called by cache when it has updated dirty)
void Directory::finished_update(int pid)
{
    // dirMem.lock();
    if(cache_map.find(pid)!=cache_map.end())
    {
        cache_map.at(pid).d_cache-=1;
        cout << "updated d_cache is "<<cache_map.at(pid).d_cache<<endl;
    }
    else
    {
        cout << "not found" <<endl;
    }
    // dirMem.unlock();
}