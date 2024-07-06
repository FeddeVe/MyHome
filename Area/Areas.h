/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Areas.h
 * Author: Fedde
 *
 * Created on 16 februari 2022, 10:37
 */

#ifndef AREAS_H
#define AREAS_H

#include "Area.h"
#include "../Global.h"
#include "../Master.h"

class Area;

class Areas {
public:
    Areas(Master *refmaster);
    Areas(const Areas& orig);
    virtual ~Areas();
    Area *get_area(int msid);
    string GetJson();
    bool add_area(string name);
    void do_work();
    
private:
    mutex m;
    Master *refmaster;
    void load_areas();
    void sort_by_name();
    
        vector<Area*>areas;

};

#endif /* AREAS_H */

