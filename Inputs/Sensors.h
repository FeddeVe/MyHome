/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Sensors.h
 * Author: Fedde
 *
 * Created on 12 februari 2022, 20:02
 */

#ifndef SENSORS_H
#define SENSORS_H

#include "Sensor.h"

class Sensor;

class Sensors {
public:
    Sensors();
    Sensors(const Sensors& orig);
    virtual ~Sensors();
    void Add_Sensor(Sensor *s);
    Sensor *get_sensor(string id);
    void do_work();
    string GetJson();
   
private:
    mutex m;
    vector<Sensor*>sensors;
    void sort_by_name();

    

};

#endif /* SENSORS_H */

