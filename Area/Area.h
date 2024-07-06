/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Area.h
 * Author: Fedde
 *
 * Created on 16 februari 2022, 10:38
 */

#ifndef AREA_H
#define AREA_H

#include "../Global.h"
#include "../Master.h"
#include "Area_Surrounding.h"
#include "Area_Surround_Brightness.h"
#include "Area_PID.h"
#include "../Scenes/Scene.h"

#define AREA_LOG_SAVE_INTERVAL 60 * 5
#define WINDOW_OPEN_DELAY_TIME 120 //30seconden * 4

struct str_area_info{
    uint32_t areaid;
    uint8_t backlight;
    float temperature;
    char temperature_str[32];
    float iaq;
    char iaq_str[32];
    float co2;
    char co2_str[32];
    float humidity;
    char humidity_str[32];
    float temp_set_point;
    char temp_set_point_str[32];
    uint8_t flame_state;
    uint8_t radiator_state;
    uint8_t door_state;
    uint8_t window_state;
            
};

struct str_setpoint{
    uint32_t worktime;
    int weekday;
    float setpoint;
    float min_temp;
};  

struct str_area_log{
    time_t last_saved;
    float temperature;
    float next_temp;
    float setpoint;
    uint8_t flame_on;
    uint8_t radiator_on;
    uint8_t manual_temp_set;    
    uint16_t iaq; 
    float co2;
    float humidity;
    uint8_t motion;
    uint8_t brightness;
    float lux;
};




class Area {
public:
    Area(Master *refmaster, int msid, string name);
    Area(const Area& orig);
    virtual ~Area();
    void do_work();
    Value GetJsonValue(Document::AllocatorType &allocator);
    string GetJsonLights();
    string GetJsonSensors();
    string toggle_light(string id);
    string toggle_sensor(string id);
    int get_msid();
    string save_settings(Value &data);
    string set_surrounding(int areaid, int level);
    void set_surround_brightness(int aread_id, float surround_brightness);
    void set_brightness(int des_bri, int fade_time, time_t release_time, string from, bool remove_scenes = true); //desbri 0>100, fadetime in seconds;
    void set_button_color(int ct, int bri, time_t release_time, string from);
    void LoadSetpoints();
    void LoadScenes();
    int add_scene(string name);
    string Get_Scene_Values(int sceneid);
    string Get_Scenes();
    bool Get_Scene_Active(int sceneid);
    int Save_Scene_Values(int sceneid, string name, Value &values);
    int Toggle_Scene(int sceneid, uint32_t fadetime_ms = 0, uint32_t releasetime_ms = 0);
    void Set_Scene(int sceneid, uint32_t fadetime_ms = 0, uint32_t releasetime_ms = 0);
    void Release_Scene(int sceneid);
    void Set_Temp_Manual(float temp);
    void Set_Temp_Auto();
     string name;
      vector<string>sensors;
    vector<string>lights;
    vector<string>door_sensors;
    vector<string>window_sensors;
private:
    mutex *m;
    mutex *tempmutex;
    mutex *scenemutex;
    Master *refmaster;
    int msid;
    str_area_log *Log;
   
   
    vector<Area_Surrounding*>surround;
    vector<Area_Surround_Brightness*>surround_brightness;
    vector<str_setpoint*>SetPoints;
    vector<Scene*>scenes;
    
    //settings
    bool motion_on;
    bool motion_off;
    int motion_off_time; //in seconds
    int manual_off_time; //in seconds
    int lux_thr;
    string icon;
    
   
    time_t last_motion;
    uint64_t last_motion_ms;
    float temperature;
    float lux;
    float humidity;
    float co2;
    float iaq;
    
    float room_brightness;
    int lux_fall;
    int motion_fall;
   
    int MOTION_FALL_TIME;
    int LUX_FALL_RISE_TIME;
    
    float manual_brightness;    
    time_t manual_release_at;
    float manual_fade_step;
    float manual_destination_brightness;
    
    bool heat_required;
    bool heat_manual; 
    float manual_setpoint;
    bool radiator_open;
    float setpoint; 
    bool door_open;
    bool window_open;
    bool enable_heating;
    float next_temp;
    int door_open_count;
    int window_open_count;
    
    str_area_info area_info;
    Area_PID *area_pid;
    
     void handle_sensors();
     void handle_light();
     void handle_heating();
     void handle_scenes();
     void handle_log();
     void load_settings();
     void Get_Display_Info();
     str_setpoint *Get_Setpoint();
     
     uint8_t Get_Backlight();
     uint64_t Last_Display_Info_Send;
     uint8_t display_resend_block;
     

};

#endif /* AREA_H */

