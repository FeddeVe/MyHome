/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Macro_Step.h
 * Author: Fedde
 *
 * Created on 17 november 2023, 21:11
 */

#ifndef MACRO_STEP_H
#define MACRO_STEP_H

#include "../Global.h"
#include "../Master.h"

enum functions{
     NONE = 0, 
     SYSTEM= 1, 
     SetAreaScene = 2,
     ReleaseAreaScene = 3,
     SetAreaIntensity = 4,
     ReleaseAreaIntensity = 5
};

class Macro_Step {
public:
    Macro_Step(uint32_t delay, int function, int intesnity, uint32_t fadetime, uint32_t releasetime, int areaid, int areascene, int systemfunction, Master *refmaster);
    Macro_Step(const Macro_Step& orig);
    virtual ~Macro_Step();
    
    void start();
    void do_work();
    void disable(int areaid = -1);
    bool isrunning();
    
private:
    
    Master *refmaster;
    mutex m;
    int msid;
    time_t starttime;
    time_t endtime;
    bool enabled;
    bool running;
    uint32_t delay;
    int function;
    int intensity;
    uint32_t fadetime;
    uint32_t releasetime;
    int areaid;
    int areascene;
    int systemfunction;
     
   
    

};

#endif /* MACRO_STEP_H */

