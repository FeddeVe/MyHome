/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/file.h to edit this template
 */

/* 
 * File:   LightState.h
 * Author: Fedde
 *
 * Created on 11 februari 2022, 20:40
 */

#ifndef LIGHTSTATE_H
#define LIGHTSTATE_H

#include "../Global.h"

class LightState{
public:
    int colortemp;
    bool ct_active;
    float raw_brightness;
    int brightness; //Raw brightnes * 100;
    bool on;
    float raw_x;
    float raw_y;
    int x; //raw_x * 1000;
    int y; // raw_y*1000;
    // time_t releaseat;
    int transistion; 
    int r;
    int g;
    int b;
    int sceneid;
    uint64_t starttime_ms;
    uint64_t fadetime_ms;
    uint64_t releasetime_ms;
    uint64_t raw_release_time;
    
};

#endif /* LIGHTSTATE_H */

