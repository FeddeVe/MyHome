/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Hue_EventStream.h
 * Author: Fedde
 *
 * Created on 13 februari 2022, 9:28
 */

#ifndef HUE_EVENTSTREAM_H
#define HUE_EVENTSTREAM_H

#include "../Global.h"
#include "Hue_Bridge.h"

class Hue_Bridge;

class Hue_EventStream:public Thread {
public:
    Hue_EventStream(Hue_Bridge *host);
    Hue_EventStream(const Hue_EventStream& orig);
    virtual ~Hue_EventStream();
    void *run();
    void quit();
    
private:
    bool exit;
    void loop_worker();
    Hue_Bridge *host_bridge;
};

#endif /* HUE_EVENTSTREAM_H */

