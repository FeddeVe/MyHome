/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Lights.h
 * Author: Fedde
 *
 * Created on 11 februari 2022, 20:29
 */

#ifndef LIGHTS_H
#define LIGHTS_H

#include "../Global.h"
#include "Light.h"

class Light;

class Lights {
public:
    Lights();
    Lights(const Lights& orig);
    virtual ~Lights();
    Light *getlight(string id);
    void add_light(Light *l);
    void do_work();
    string GetJson();
private:
    mutex m;    
    void sort_by_name();
    vector<Light*>lights;

};

#endif /* LIGHTS_H */

