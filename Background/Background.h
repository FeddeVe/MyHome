
/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Background.h
 * Author: Fedde
 *
 * Created on 19 februari 2022, 20:20
 */

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "../Global.h"
#include "../Thread.h"
#include "Background_Slot.h"
#include "sunset.h"
#include <random>
#include "Background_Value.h"
#include "../Master.h"

#define S_DAY 86400


class Background: public Thread {
public:
    Background(Master *refmaster);
    Background(const Background& orig);
    virtual ~Background();
    
    void load_slots();
    void load_sunset();
    string GetJson();
    void add_slot_scene(int slotid, string name);
    string Get_Slot_Scene_Values(int slotid, int sceneid);
    void Save_Scene_Values(int slotid, int sceneid, Value &values);
    void *run();
    void quit();
    void do_work();
    
private:
    mutex m;    
    Master *refmaster;
    bool exit;
   // bool overnight;
    vector<Background_Slot*>slots;
    int worktime; // time of day in seconds;
    int sunset; //sunset time in seconds;
    int sunrise; // sunrise time in seconds;
    SunSet sun;
    time_t nu;
    time_t last_sunset_check;
    Background_Slot *act_slot;
    Background_Slot *prev_slot;
    Scene *act_scene;
    Scene *prev_scene;
    float act_pct;     
     bool select_slots();
     void calc_values();
     vector<Background_Value*>values;
     bool first_select;
      

};

#endif /* BACKGROUND_H */

