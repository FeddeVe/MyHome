/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Buttons.h
 * Author: Fedde
 *
 * Created on 15 februari 2022, 20:03
 */

#ifndef BUTTONS_H
#define BUTTONS_H

#include "../Global.h"
#include "Button.h" 

class Button;

class Buttons {
public:
    Buttons();
    Buttons(const Buttons& orig);
    virtual ~Buttons();
    Button *get_button(string ownerid);
    void add_button(Button *b);
    string GetJson();
    string GetJsonDetect();
    void start_detect();
    void cancel_detect();
    void detect_result(string id);
    void SaveButton(string id, string name, Value &data);
private:
    mutex m;
    vector<Button*>buttons;
    bool detect;
    string detectedid;

};

#endif /* BUTTONS_H */

