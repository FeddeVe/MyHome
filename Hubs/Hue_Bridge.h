/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Hue_Bridge.h
 * Author: Fedde
 *
 * Created on 9 februari 2022, 21:09
 */

#ifndef HUE_BRIDGE_H
#define HUE_BRIDGE_H

#include "../Global.h"
#include "../Thread.h"
#include "../Master.h"

#include "Hue_Device.h"
#include "Hue_EventStream.h"
#include "Hue_Send_Buffer.h"



class Master;
class Hue_Device;
class Hue_Send_Buffer;
class Hue_EventStream;

class Hue_Bridge:public Thread {
public:
    Hue_Bridge(string id, string ip, string username, Master *refmaster);
    Hue_Bridge(const Hue_Bridge& orig);
    virtual ~Hue_Bridge();
    void setIP(string ip);
    void *run();
    string GetID();
    void quit();
    Value GetJsonValue(Document::AllocatorType &allocator);
    string GetJson();
    string link_bridge();
    void do_work(); 
    void add_send_buffer(Hue_Send_Buffer *buf);
    void add_send_buffer_NO_LOCK(Hue_Send_Buffer *buf);
    void add_EventStreamBuffer(string msg);
    int get_state();
    void handle_streamevents();
    void handle_incoming();
    string get_ip();
    string get_username();
private:
    mutex m;
     
    Master *refmaster;
    Hue_EventStream *stream;
    string id;
    string ip;
    string username;
    string name;
    bool exit;
    
    void handle_msg(Value &data);
    void handle_device_data(Value &data);
    void handle_device_power(Value &data);
    void handle_light(Value &data);
    void handle_light_level(Value &data);
    void handle_motion(Value &data);
    void handle_temperature(Value &data);
    void handle_zigbee_connectivity(Value &data);
    void handle_button(Value &data);
    void loop_worker();
    
    int state;  // 0 = not autorized, 1=not online, 2=running     
    Hue_Device *get_device(string id);
    vector<Hue_Device*>devices;
    vector<Hue_Send_Buffer*>send_buffer;
     vector<string>EventStreamBuffer;
     vector<string>incoming;
     int msg_timeout;
};

#endif /* HUE_BRIDGE_H */

