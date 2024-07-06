/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Area_PID.h
 * Author: Fedde
 *
 * Created on 16 oktober 2023, 21:57
 */

#ifndef AREA_PID_H
#define AREA_PID_H

#include "../Global.h"

#define HISTORY_POINTS 20
#define FUTURE_POINTS 10

class Area_PID {
public:
    Area_PID();
    Area_PID(const Area_PID& orig);
    virtual ~Area_PID(); 
    bool get_next_temp(float cur_temp, float *next_temp);
    
private:
    vector<float>History;
    //vector<float>Future;
    time_t last_measure;

};

#endif /* AREA_PID_H */

