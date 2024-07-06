/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Light.h
 * Author: Fedde
 *
 * Created on 11 februari 2022, 20:30
 */

#ifndef LIGHT_H
#define LIGHT_H

#include "../Global.h"
#include "LightState.h"
#include "../Hubs/Hue_Device.h"
 
 

class Light {
public:
    Light(string id, Hue_Device *host_device);
    Light(const Light& orig);
    virtual ~Light();
    void do_work();     
    void set_name(string name);        
    void set_product_name(string name);     
    void set_archetype(string archetype);    
    void set_area_id(int area_id);     
    void set_act_on(bool on);
    void set_act_bri(float brightness);
    void set_act_ct(int ct, bool ct_valid);
    void set_act_color(float x, float y);
    void set_ct_min_max(int min, int max);
    void set_gammut_type(string type);
    void set_min_dim_level(float dimlevel);
    void set_con_state(string con_state, bool online);
    void set_batterylevel(float level);
    void set_batterystate(string state);
    void set_preview(Value &data);
    void set_room_brightness(float brightness);
    string get_id();
    Value get_json_value(Document::AllocatorType &allocator);
    string GetJson();
    string name;   
    int get_area_id();
    void set_background(int brightness, int mirek, int x, int y);
    void set_scene(int sceneid, uint32_t fade_in_ms, uint32_t releasetime_ms, int brightness, int mirek, int x, int y);
    void remove_scene(int sceneid);
    void extent_scene(int sceneid, uint64_t motiontime_ms);
    bool is_scene_active(int sceneid);
   // void stop_manual();
    void set_manual(int brightness, uint64_t releasetime_ms);
   // void set_manual_brightness(int brightness, time_t release);
   // void set_manual_ct(int ct, time_t release);
   // void set_manual_color(int x, int y, time_t release);
   // void set_manual_release_time(time_t time);
    void Identify();
   // void set_mesh_data(int intensity, int red, int green, int blue, string name, int nodeid, int index);
    void calc_rgb();
    
    LightState *cur_state; // current state from hubs
    LightState *des_state; // state to send to hubs
    //LightState *prev_state;
    LightState *background_state;
    LightState *manual_state;
    vector<LightState*>scene_states;
    
private:
    mutex m;
    string id;
    string archetype; 
    int mesh_nodeid;
    int mesh_index;
    string product_name;    
    bool have_color;
    bool have_ct;
    bool have_dim;
    string gammut_type;
    int area_id;     
    int ct_max;
    int ct_min;
    float min_dim_level;
    string con_state;
    bool online;
     float battery_level;
    string battery_state;
    float room_brightness;
    //LightState *cur_state;
   
    bool background_set;
    
    
    Hue_Device *host_device;
    
    void set_light();
    void set_des_state();
    int block;
    

};

#endif /* LIGHT_H */

