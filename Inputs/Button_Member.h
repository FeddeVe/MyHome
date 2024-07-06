/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Button_Member.h
 * Author: Fedde
 *
 * Created on 15 februari 2022, 20:05
 */

#ifndef BUTTON_MEMBER_H
#define BUTTON_MEMBER_H

#include "../Global.h"
#include "Button.h"
#include "../Master.h"
#include "Button_Action.h"

class Button;

class Button_Member {
public:
    Button_Member(Button *host, Master *refmaster, string id);
    Button_Member(const Button_Member& orig);
    virtual ~Button_Member();
    void set_last_event(string id, string last_event);
    string get_id();
    string get_last_event();
    void set_ctrlid(int ctrl_id);
    void LoadAction();
    void SaveAction(int AreaID, int Function, int Intensity, uint32_t FadeTime, uint32_t ReleaseTime, int SceneID, int MacroID, int SystemFunction);
    Button_Action *action;
    void set_id(string id);
   
private:
    mutex m;
    string last_event;
     string id;   
    Button *host;
    Master *refmaster;
    int ctrl_id;
    bool loaded;
    
    
    
    //action
    
};

#endif /* BUTTON_MEMBER_H */

