/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Tree_Structs.h
 * Author: Fedde
 *
 * Created on 29 juli 2023, 19:59
 */

#ifndef TREE_STRUCTS_H
#define TREE_STRUCTS_H

#include <string.h>
#include<iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sys/time.h>

using namespace std;

class Tree_Structs {
public:
    Tree_Structs();
    Tree_Structs(const Tree_Structs& orig);
    virtual ~Tree_Structs();
    
    enum NodeTypes{
        
    };

    struct Display_Config {
        uint32_t RoomID;
        char RoomName[32];
        char BackgroundPic[32];
        char IconDoorClose[32];
        char IconDoorOpen[32];
    };

    struct str_File_Request {
        char FileName[32];
        uint32_t Index;
    };

    struct str_File_Response {
        char FileName[32];
        bool exist;
        uint32_t Index;
        uint32_t FileSize;
        uint16_t Data_Length;
        char data[512];
    };
    
    struct str_Config_Announce{
        uint32_t config_version;
        char config_md5[32];
        uint32_t FileSize;
    };

    struct str_Config_Request {
        char FileName[32];
        uint32_t Index;
    };

    struct str_Config_Response {
        uint32_t Index;
        uint32_t FileSize;
        uint16_t Data_Length;
        char data[768];
    };

    struct _str_Dislay_Report {
        uint32_t ConfigVersion;
    };

    struct str_ui_screen_config {
        char res[256];
    };
    
    struct _str_error{
    char msg[128];
    int code;
};

struct str_BME680{
    uint8_t iaq_accuracy;
    uint16_t iaq;
    uint8_t static_iaq_accuracy;
    uint16_t static_iaq;
    float raw_temperature;
    float compensated_temperature;
    float raw_humidity;
    float compensated_humidity;
    float pressure;
    float co2;
    uint8_t BME_Status;
    uint8_t BSEC_Status;
};

    struct str_file { 
        uint32_t offset;
        uint32_t size;
    };

    struct str_display_config {
        char config_header[10];
        uint32_t version;
        uint32_t area_id;
        float temp_offset_fast;
        float temp_offset_slow;
        uint8_t off_touch_mode;
        uint8_t off_long_press_mode;
        uint8_t max_int;
        uint8_t min_int;
        uint8_t max_touch_int;
        uint8_t min_touch_int; 
        uint8_t text_r;
        uint8_t text_g;
        uint8_t text_b;
        uint8_t shadow_r;
        uint8_t shadow_g;
        uint8_t shadow_b;
        str_file btn_1;
        str_file btn_1_active;
        str_file btn_1_touch;
        str_file btn_1_icon;
        char btn1_header[32];
        char btn1_footer[32];
        uint8_t btn1_function;
        str_file btn_2;
        str_file btn_2_active;
        str_file btn_2_touch;
        str_file btn_2_icon;
        char btn2_header[32];
        char btn2_footer[32];
        uint8_t btn2_function;
        str_file btn_3;
        str_file btn_3_active;
        str_file btn_3_touch;
        str_file btn_3_icon;
        char btn3_header[32];
        char btn3_footer[32];
        uint8_t btn3_function;
        str_file btn_4;
        str_file btn_4_active;
        str_file btn_4_touch;
        str_file btn_4_icon;
        char btn4_header[32];
        char btn4_footer[32];
        uint8_t btn4_function;
        str_file btn_5;
        str_file btn_5_active;
        str_file btn_5_touch;
        str_file btn_5_icon;
        char btn5_header[32];
        char btn5_footer[32];
        uint8_t btn5_function;
        str_file btn_6;
        str_file btn_6_active;
        str_file btn_6_touch;
        str_file btn_6_icon;
        char btn6_header[32];
        char btn6_footer[32];
        uint8_t btn6_function;
        str_file btn_7;
        str_file btn_7_active;
        str_file btn_7_touch;
        char btn7_header[32];
        char btn7_footer[32];
        uint8_t btn7_function;
        str_file btn_8;
        str_file btn_8_active;
        str_file btn_8_touch;
        char btn8_header[32];
        char btn8_footer[32];
        uint8_t btn8_function;
        str_file btn_9;
        str_file btn_9_active;
        str_file btn_9_touch;
        char btn9_header[32];
        char btn9_footer[32];
        uint8_t btn9_function;
        str_file btn_10;
        str_file btn_10_active;
        str_file btn_10_touch;
        char btn10_header[32];
        char btn10_footer[32];
        uint8_t btn10_function;
        str_file btn_11;
        str_file btn_11_active;
        str_file btn_11_touch;
        char btn11_header[32];
        char btn11_footer[32];
        uint8_t btn11_function;
        str_file btn_12;
        str_file btn_12_active;
        str_file btn_12_touch;
        char btn12_header[32];
        char btn12_footer[32];
        uint8_t btn12_function;
        str_file wifi_ok;
        str_file wifi_nok_0;
        str_file wifi_nok_1;
        str_file wifi_nok_2;
        str_file wifi_nok_3;
        str_file wifi_nok_4;
        str_file flame_off;
        str_file flame_on;
        str_file hum_icon;
        str_file hum_dial;
        str_file iaq_dial;
        str_file temp_dial;
        str_file co2_icon;  
        str_file slider_icon;
        str_file settings_icon;
        str_file def_background;
        str_file close_icon;
         
    };
    
  

private:

};

#endif /* TREE_STRUCTS_H */

