/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Backgrund_Slot.h
 * Author: Fedde
 *
 * Created on 19 februari 2022, 20:21
 */

#ifndef BACKGROUND_SLOT_H
#define BACKGROUND_SLOT_H

#include "../Global.h"
#include "../Scenes/Scene.h"

class Background_Slot {
public:
    Background_Slot(int id);
    Background_Slot(const Background_Slot& orig);
    virtual ~Background_Slot();
    void add_scene(string name);
    
    int id;     
    time_t time_end; // seconds
    int time_end_overnight;
    int duration; // seconds;
    string name;
    void load_scenes();
    vector<Scene*>scenes;
    string Get_Scene_Values(int sceneid);
    void Save_Scene_Values(int sceneid, Value &values);
    
  
    
private:
    

};

#endif /* BACKGRUND_SLOT_H */

