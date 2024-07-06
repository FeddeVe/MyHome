/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/*
 * File:   Button.h
 * Author: Fedde
 *
 * Created on 15 februari 2022, 20:03
 */

#ifndef BUTTON_H
#define BUTTON_H

#include "../Global.h"
#include "Button_Member.h"
#include "../Master.h"
#include "../Hubs/Hue_Device.h"

class Button_Member;

class Button {
public:
    Button(Master *refmaster, Hue_Device *host,string ownerid, string name, string productname);
    Button(const Button& orig);
    virtual ~Button();
    string getID();
    void Set_Button(string id, int controlid, string last_event);
    Value GetJsonValue(Document::AllocatorType &allocator);
    void set_name(string name);
    void set_batterylevel(float level);
    void set_batterystate(string state);
    void set_constate(string state, bool online);
     void set_productname(string name);
     void start_detect();
     void stop_detect();
     void save_button(string name, Value &data);
    
private:
    mutex m;
    Master *refmaster;
    Hue_Device *host;
    string ownerid;
    vector<Button_Member*>buttons;
    string name;
    float battery_level;
    string battery_state;
    string Con_State;     
    string productname;
    bool online;
    bool detect;    
};

#endif /* BUTTON_H */

