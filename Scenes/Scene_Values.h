/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Scene_Values.h
 * Author: Fedde
 *
 * Created on 23 februari 2022, 8:36
 */

#ifndef SCENE_VALUES_H
#define SCENE_VALUES_H

#include "../Global.h"

class Scene_Values {
public:
    Scene_Values(int msid, string lightid, int sceneid);
    Scene_Values(string lightid, int sceneid);
    Scene_Values(const Scene_Values& orig);
    virtual ~Scene_Values();
    void set_brightness(int raw_brightness); //value 0>100
    void set_kelvin(int kelvin, bool use_ct);   
    void set_rgb(int red, int green, int blue);
    void set_in_use(bool inuse);
    void Save_Values();
    
    int msid;  
    int sceneid;
    string lightid;
    int raw_brightness;
    int brightness;
    int ct;
    int kelvin;
    bool usect;
    int x;
    int y;
    float raw_x;
    float raw_y; 
    int red;
    int green;
    int blue;
    bool inuse;
    float raw_ct_x;
    float raw_ct_y;
    int ct_x;
    int ct_y; 
private:
    
};

#endif /* SCENE_VALUES_H */

