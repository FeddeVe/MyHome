/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Button_Action.h
 * Author: Fedde
 *
 * Created on 4 maart 2022, 20:46
 */

#ifndef BUTTON_ACTION_H
#define BUTTON_ACTION_H

#include "../Global.h"

class Button_Action {
public:
    Button_Action();
    Button_Action(const Button_Action& orig);
    virtual ~Button_Action();
    int msid;
    int areaID;
    int Function;
    int Intensity;
    uint32_t FadeTime; 
    uint32_t ReleaseTime;
    int SceneID;
    int MacroID;
    int SystemFunction;
   
private:

    
};

#endif /* BUTTON_ACTION_H */

