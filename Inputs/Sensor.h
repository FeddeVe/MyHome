/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Sensor.h
 * Author: Fedde
 *
 * Created on 12 februari 2022, 20:03
 */

#ifndef SENSOR_H
#define SENSOR_H

 

#include "../Global.h"
#include "../Hubs/Hue_Device.h"

class Sensor {
public:
    Sensor(string id, Hue_Device *host_device, bool Tree_Mesh);
    Sensor(const Sensor& orig);
    virtual ~Sensor();    
    void set_motion(bool motion);
    void set_temperature(float temperature);
    void set_lightlevel(int lightlevel, bool raw_lux);
    void set_batterylevel(float level);
    void set_batterystate(string state);
    void set_constate(string state, bool online);
    void set_name(string name);
    void set_productname(string name);
    void set_io_state(int io_state);
    
    time_t get_last_motion();
    float get_temperature();
    float get_light_level();
    float get_humidity();
    float get_co2();
    float get_iaq();
    int get_io_state();
    bool get_online();    
    string get_id();
    Value get_json_value(Document::AllocatorType &allocator);
    bool has_motion();
    bool has_temperature();
    bool has_lightlevel();
    bool has_humidity();
    bool has_co2();
    bool has_iaq();
    bool has_io();
     
    void set_bme680(float st_iaq, int iaq_accuracy, float co2, float humidity,float temperature, float pressure);
   
    
    void do_work();
     void Identify();
     string name;
    
private:
    mutex m;
    Hue_Device *host_device;
    string id;
    bool Tree_Mesh;
    bool have_motion;
    bool have_temperature;
    bool have_lightlevel;
    bool have_humidity;
    bool have_co2;
    bool have_iaq;
    bool have_battery;
    bool have_pressure;
    bool have_io;
    time_t last_motion;     
    float temperature;
    float light_level;
    float battery_level;
    string battery_state;
    float humidity;
    float co2;
    float iaq;
    float pressure;
    int io_state;
    string Con_State;
    
    string productname;
    bool online;
    time_t last_update;
    
    
    
    
    

};

#endif /* SENSOR_H */

