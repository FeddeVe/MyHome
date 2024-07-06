/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Hue_Device.h
 * Author: Fedde
 *
 * Created on 13 februari 2022, 9:18
 */

#ifndef HUE_DEVICE_H
#define HUE_DEVICE_H

#include "../Global.h"
#include "../Master.h"
#include "Hue_Service.h"
#include "Hue_Bridge.h"



class Hue_Bridge;
class Master;

class Hue_Device {
public:
    Hue_Device(string id, Hue_Bridge *host_bridge, Master *refmaster);
    Hue_Device(const Hue_Device& orig);
    virtual ~Hue_Device();    
    void do_work();
    string GetID();
    void set_metadata(Value &data);
    void set_product_data(Value &data);
    void set_services(Value &data);
    void set_device_power(Value &data);
    void set_light(Value &data);
    void set_light_level(Value &data);
    void set_motion(Value &data);
    void set_temperature(Value &data);
    void set_zigbee_connectivity(Value &data);
    void set_button(Value &data);
    void put_data(string url, string body);
    void SetName(string name);
    void Ident();
private:
    mutex m;
    Master *refmaster;
    vector<Hue_Service*>services;
    string id;
    Hue_Bridge *host_bridge;
    string archetype;
    string name;
    string productname;
    string con_state;
    time_t last_update;
    int batterylevel;
    string bat_state;
    bool online;
    bool request_devices;
    
    
    

};

#endif /* HUE_DEVICE_H */

