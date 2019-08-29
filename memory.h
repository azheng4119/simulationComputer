//
//  memory.h
//  simulationComputer
//
//  Created by Andy Zheng on 5/12/19.
//  Copyright © 2019 Andy Zheng. All rights reserved.
//

#ifndef memory_h
#define memory_h

class Memory{
    int frame, page, pid, time;
    bool empty;
public:
    Memory(int f, int p, int pi, int t) : frame(f), page(p), pid(pi), time(t), empty(true) {}
    void update(int p, int pi, int t,bool emp){
        page = p;
        pid = pi;
        time = t;
        empty = emp;
    }
    
    bool isEmpty(){
        return empty;
    }
    
    int getFrame(){
        return frame;
    }
    int getPid(){
        return pid;
    }
    
    int getPage(){
        return page;
    }
    
    int getTime(){
        return time;
    }
};
#endif /* memory_h */
