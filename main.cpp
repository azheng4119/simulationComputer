//
//  main.cpp
//  simulationComputer
//
//  Created by Andy Zheng on 3/27/19.
//  Copyright © 2019 Andy Zheng. All rights reserved.
//

#include <iostream>
#include <vector>
#include <regex>
#include <list>
#include "process.h"
#include "memory.h"

static std::vector<std::string> responses;
static std::vector<Process> readyqueue,waitQueue,zombieQueue;
static std::vector<std::vector<std::pair<std::string, Process>> > ioqueue;
static std::vector<Memory> memoryTable;
static long ram;
static int pagesize,harddisks = 0;
static int timestamp = 1;
static int pidCounter = 2;
static bool running = false;

void init(){
    responses.push_back("A");
    responses.push_back("Q");
    responses.push_back("fork");
    responses.push_back("exit");
    responses.push_back("wait");
    responses.push_back("S r");
    responses.push_back("S i");
    responses.push_back("S m");
    responses.push_back("S z");
    //readyqueue.push_back(Process(1));
    for (int i = 0; i < harddisks; i++){
        ioqueue.push_back(std::vector<std::pair<std::string, Process>>{});
    }
    for (int i = 0; i < (ram/pagesize); i++){
        memoryTable.push_back(Memory(i, 0, 0, 0));
    }
}

void newProcess(){
    readyqueue.push_back(Process(pidCounter++,false));
}

void quitProcess(){
    readyqueue.push_back(readyqueue.at(0));
    if (readyqueue.size() != 0) readyqueue.erase(readyqueue.begin());
}

void forkProcess(){
    Process child{pidCounter++,true,readyqueue[0].getPid()};
    readyqueue.at(0).addChild(child);
    readyqueue.push_back(child);
}

void printQueue(){
    if (readyqueue.empty()){
        printf("Empty\n");
        return;
    }
    int counter = 0;
    for (auto i : readyqueue){
        if (counter == 0) std::cout << " Cpu : ";
        if (counter == 1) std::cout << " Ready Queue : ";
        std::cout << i.getPid() << ", ";
        counter++;
    }
            std::cout << std::endl;
}

void gatherVars(){
    std::cout << "How much Ram?" << std::endl;
    std::cin >> ram;
    std::cout << "How many Pages?" << std::endl;
    std::cin >> pagesize;
    std::cout << "How many Hard Disks?" << std::endl;
    std::cin >> harddisks;
    running = true;
}

void addToIOqueue(std::string name, int disknum){
    ioqueue[disknum].push_back(std::make_pair(name, readyqueue[0]));
    readyqueue.erase(readyqueue.begin());
    std::cout << "Adding " << name << " to hard disk #" << disknum << std::endl;
}

void printIOqueue(){
    for (int i = 0; i < harddisks; i++){
        std::cout << "Hard Disk #" << i << std::endl;
        if (ioqueue[i].size() != 0){
            for (auto x : ioqueue[i]){
                std::cout << x.first << " - Process #" << x.second.getPid() << std::endl;
            }
        }
        else{
            std::cout << "Empty" << std::endl;
        }
    }
}

void eraseMemory(int pi){
    for (auto & i: memoryTable){
        if (i.getPid() == pi) i.update(0, 0, 0,true);
    }
}

void advanceIO(int disknum){
    printf("Hard disk #%i has finished it's work\n",disknum);
    readyqueue.push_back(ioqueue[disknum].begin()->second);
    ioqueue[disknum].erase(ioqueue[disknum].begin());
}

void eraseFromIO(int del){
    if (ioqueue.empty()) return;
    for (auto & i :ioqueue){
        int xy = 0;
        for (auto x : i){
            if (x.second.getPid() == del){
                i.erase(i.begin()+xy);
            }
            xy++;
        }
    }
}

void wakeUpParent(int parent, int child){
    if (parent == 1) return;
    else{
        int index = 0;
        for (auto i : waitQueue){
            if (parent == i.getPid()){
                readyqueue.push_back(waitQueue[index]);
                waitQueue.erase(waitQueue.begin()+index);
                int index2 = 0;
                for (auto y : zombieQueue){
                    if (child == y.getPid()){
                        zombieQueue.erase(zombieQueue.begin()+index);
                        return;
                    }
                    index2++;
                }
            }
            index++;
        }
    }
}

static Process fake(-1,false);

Process & getChildren(int i){
    for (auto & x : readyqueue){
        if (x.getPid() == i)
            return x;
    }
    return fake;
}

void exitProcess(Process & del){
    Process & real = getChildren(del.getPid());
        for (auto & i : real.getListOfChildren()){
            exitProcess(i);
            eraseFromIO(i.getPid());
            i.removeChild();
        }
    for (auto it = readyqueue.begin(); it != readyqueue.end(); it++){
        if (it->getPid() == real.getPid()) {
            if (!waitQueue.empty()) {
                wakeUpParent(real.getParent(), real.getPid());
            }
            readyqueue.erase(it);
            eraseMemory(real.getPid());

            return;
        }

    }
}

void waitProcess(){
    if (readyqueue[0].getListOfChildren().empty()) {
        printf("No Children\n");
        return;
    }
    if (zombieQueue.empty()){
        waitQueue.push_back(readyqueue[0]);
        readyqueue.erase(readyqueue.begin());
    }else{
        for (auto x : readyqueue[0].getListOfChildren()){
            int index = 0;
            for (auto i : zombieQueue){
                if (x.getPid() == i.getPid()){
                    zombieQueue.erase(zombieQueue.begin()+index);
                    return;
                }
                index++;
            }
        }
    }
}


int findIndexOfNextMem(int amount , int pagenum){
    int counter = 0;
    bool found = false;
    int lowest = INT_MAX;
    int lowestIndex = -1;;
    for (int i = 0; i < memoryTable.size(); i++){
        if (memoryTable[i].getTime() < lowest){
            lowest = memoryTable[i].getTime();
            lowestIndex = i;
        }
        if (memoryTable[i].getPid() == readyqueue[0].getPid() && memoryTable[i].getPage() == pagenum && found != true){
            found = true;
            counter = i;
        }
        else{
            if (memoryTable[i].isEmpty() && found!= true) {
                found = true;
                counter = i;
            }
        }
        
        if (found != true){
            counter = lowestIndex;
        }
    }
    return counter;
}
    
void requestMem(int amount){
    int pagenum = amount / pagesize;
    memoryTable[findIndexOfNextMem(amount, pagenum)].update(pagenum, readyqueue[0].getPid(), timestamp,false);
    timestamp++;
}

void printMemory(){
    std::cout << "| Frame | Page | Pid | TS |" << std::endl;
    for (auto i : memoryTable){
        std::cout << "|     " << i.getFrame() << " |    " << i.getPage() << " |   " << i.getPid() << " | " << i.getTime() << std::endl;
    }
}

void printZombie(){
    for (auto i : zombieQueue){
        std::cout<<  i.getPid() << std::endl;
    }
}
bool handleInput(std::string input){
    bool invalid = readyqueue.empty();
    std::regex d ("(d )([0-9])(.*)");
    std::regex D ("(D )([0-9])(.*)");
    std::regex m ("(m )([0-9])(.*)");
    if (std::regex_match(input,d)) {
        if(invalid) {
            printf("No Process available\n");
            return false;
        }
            int x = 2;
            while (input.substr(x,1) != " ") x++;
            int num = std::stoi(input.substr(2,x-2));
            std::string filename = input.substr(3+(x-2));
            if (num > harddisks-1) {
                std::cout << num << " is not a valid hard disk number." << std::endl;
                return false;
            }
            addToIOqueue(filename,num);
            return true;
    };
    if (std::regex_match(input,D)){
        advanceIO(std::stoi(input.substr(2)));
        return true;
    }
    if (std::regex_match(input,m)){
        int amount = std::stoi(input.substr(2));
        requestMem(amount);
        return true;
    }
    auto iterator = std::find(responses.begin(),responses.end(),input);
    if (iterator != responses.end()) {
        if (*iterator == "S r") printQueue();
        if (*iterator == "S i") printIOqueue();
        if (*iterator == "S m") printMemory();
        if (*iterator == "S z") printZombie();
        if (*iterator == "Q") quitProcess();
        if (*iterator == "A") newProcess();
        if (*iterator == "fork") forkProcess();
        if (*iterator == "exit") {
            if (readyqueue[0].getChild()) zombieQueue.push_back(readyqueue[0]);
            exitProcess(readyqueue[0]);
        }
        if (*iterator == "wait") waitProcess();
        return true;
    }
    
    return false;
}
int main(int argc, const char * argv[]) {
    gatherVars();
    init();
    std::cin.ignore();
    while (running){
        std::cout << ">> ";
        std::string input = "";
        std::getline(std::cin,input);
        if (!handleInput(input)) std::cout << "Invalid command" << std::endl;
    }
    return 0;
}

