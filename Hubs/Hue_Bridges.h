/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Hue_Bridges.h
 * Author: Fedde
 *
 * Created on 9 februari 2022, 12:11
 */

#ifndef HUE_BRIDGES_H
#define HUE_BRIDGES_H

#include "../Global.h"
#include "Hue_Bridge.h"
#include "../Master.h" 

class Master;
class Hue_Bridge;

class Hue_Bridges {
public:
    Hue_Bridges(Master *refmaster);
    Hue_Bridges(const Hue_Bridges& orig);
    virtual ~Hue_Bridges();
    void Discover();
    Hue_Bridge *get_bridge(string id);
    void start();
    string get_json();
    void do_work();
    void request_discover();
    
private:
    mutex m;
    
    Master *refmaster;
    void load_known_bridges();
    vector<Hue_Bridge*>bridges;
    time_t last_discover;
    bool request_discover_bool;
   

};

#endif /* HUE_BRIDGES_H */

