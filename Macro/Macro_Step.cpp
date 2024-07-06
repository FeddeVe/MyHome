/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Macro_Step.cpp
 * Author: Fedde
 * 
 * Created on 17 november 2023, 21:11
 */

#include "Macro_Step.h"

Macro_Step::Macro_Step(uint32_t delay, int function, int intenstiy, uint32_t fadetime, uint32_t releasetime, int areaid, int areascene, int systemfunction, Master *refmaster) {
    
    this->delay = delay;
    this->function = function;
    this->intensity = intenstiy;
    this->fadetime = fadetime;
    this->releasetime = releasetime;
    this->areaid = areaid;
    this->areascene = areascene;
    this->systemfunction = systemfunction;
    this->refmaster = refmaster;
    
    this->enabled = false;
    this->running = false;
             
    
}

Macro_Step::Macro_Step(const Macro_Step& orig) {
}

Macro_Step::~Macro_Step() {
}

void Macro_Step::start(){
    m.lock();
    this->enabled = true;
    this->running = false;
    this->starttime = time(0)+(this->delay * 60);
    this->endtime = this->starttime + (this->releasetime*60); 
    m.unlock();    
}

void Macro_Step::do_work(){
    m.lock();
    time_t nu = time(0);
    if(this->enabled == false){
        this->running = false;
        m.unlock();
        return;
    }
    
    if(this->running == false){
        if(nu >= this->starttime){
            // do action
            switch(this->function){
                case functions::SYSTEM:
                {
                    
                }
                break;
                case functions::SetAreaScene:
                {
                    Area *a = this->refmaster->areas->get_area(this->areaid);
                    if(a!= NULL){
                        a->Set_Scene(this->areascene, this->fadetime * 1000, this->releasetime *60*1000);                        
                    }
                    this->running = true;
                    this->endtime = nu + (this->releasetime * 60); 
                }
                break;
                case functions::ReleaseAreaScene:
                {
                    Area *a = this->refmaster->areas->get_area(this->areaid);
                    if(a!= NULL){
                        a->Release_Scene(this->areascene);                        
                    }
                    this->running = true; 
                }
                break;
                case functions::SetAreaIntensity:
                {
                    Area *a = this->refmaster->areas->get_area(this->areaid);
                    if(a!= NULL){
                        a->set_brightness(this->intensity, this->fadetime, this->releasetime * 60, "MACRO", false);                       
                    }
                    this->running = true; 
                    this->endtime = nu + (this->releasetime * 60);
                }
                break;                
            } 
        }
    } 
    m.unlock();
}

void Macro_Step::disable(int areaid){
    m.lock();
    if(areaid == -1){
        this->enabled = false;
    }else{
        if(this->areaid == areaid){
            this->enabled= false;
        }
    } 
    m.unlock();
}

bool Macro_Step::isrunning(){
    bool ret = false;
    m.lock();
    if(this->enabled){
        time_t nu = time(0);
        if(this->endtime > nu){
            ret = true;
        }
    }
    m.unlock();
    return ret;
}

 
