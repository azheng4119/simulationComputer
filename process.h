//
//  process.h
//  simulationComputer
//
//  Created by Andy Zheng on 3/27/19.
//  Copyright © 2019 Andy Zheng. All rights reserved.
//

#ifndef process_h
#define process_h

const int WAITING = 0;
const int RUNNING = 1;
const int ZOMBIE = 2;

class Process{
private:
    int pid,state,parent;
    bool isChild;
    std::vector<Process>children;
    
public:
    Process(int x, bool child) : pid(x), isChild(child), parent(1) {}
    Process(int x, bool child, int par) : pid(x), isChild(child),parent(par) {}
    void addChild(Process child){
        children.push_back(child);
    }
    int getPid(){
        return pid;
    }
    
    bool getChild(){
        return isChild;
    }
    
    void setState(int status){
        state = status;
    }
    
    std::vector<Process> getListOfChildren(){
        return children;
    }
    void removeChild(){
        if (!children.empty()) children.erase(children.begin());
    }
    
    int getParent(){
        return parent;
    }
};

#endif /* process_h */
