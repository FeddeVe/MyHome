/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Button_Action.cpp
 * Author: Fedde
 * 
 * Created on 4 maart 2022, 20:46
 */

#include "Button_Action.h"

Button_Action::Button_Action() {
     this->areaID = -1;
     this->Function = 0;
    this->Intensity =0;
    this->FadeTime=0; 
    this->ReleaseTime = 0;
    this->msid = -1;
    this->SceneID = 0;
    this->MacroID = 0;
    this->SystemFunction = 0;
}

Button_Action::Button_Action(const Button_Action& orig) {
}

Button_Action::~Button_Action() {
}

