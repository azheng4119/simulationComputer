//
//  controlblock.h
//  simulationComputer
//
//  Created by Andy Zheng on 4/29/19.
//  Copyright © 2019 Andy Zheng. All rights reserved.
//


#ifndef controlblock_h
#define controlblock_h

class controlblock{
private:
    int pid;
    int state;
    std::vector<Process> children;

public:
    controlblock(int x, int y){
        pid = x;
        state = y;
    }
    void addChild(Process){
        
    }
};
#endif /* controlblock_h */
