/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Macro.h
 * Author: Fedde
 *
 * Created on 18 november 2023, 11:56
 */

#ifndef MACRO_H
#define MACRO_H


#include "Macro_Step.h"
#include "../Global.h"
#include "../Master.h"

class Macro_Step;

class Macro {
public:
    Macro(int msid, string name, Master *refmaster);
    Macro(const Macro& orig);
    virtual ~Macro();
    
    void LoadSteps();
    void do_work();
    void disable(int areaid = -1);
    bool isrunning();
    void start();
    int msid;
private:
    
    mutex m;
   
    string name;
    Master *refmaster;
    vector<Macro_Step*>steps;
    
   
    
    

};

#endif /* MACRO_H */

