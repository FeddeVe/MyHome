/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Area_PID.cpp
 * Author: Fedde
 * 
 * Created on 16 oktober 2023, 21:57
 */

#include "Area_PID.h"

Area_PID::Area_PID() {
    this->last_measure = 0;
}

Area_PID::Area_PID(const Area_PID& orig) {
}

Area_PID::~Area_PID() {
}

bool Area_PID::get_next_temp(float cur_temp, float *next_temp){
    time_t nu = time(0);
    if(nu - this->last_measure > 60){
        this->last_measure = time(0);
        this->History.push_back(cur_temp);       
        if(this->History.size() < HISTORY_POINTS){
            return false;
        }
        if(this->History.size() > HISTORY_POINTS){
            this->History.erase(this->History.begin());
            this->History.shrink_to_fit(); 
        }
        float res = 0;
        uint32_t count = 0;
        for(int i = 1; i <this->History.size(); i++){
            float diff = this->History[i] - this->History[i-1];
            if((diff > 0.5F)||(diff<-0.5F)){
                //skip this
            }else{
            res = res + (diff * i);
            count = count + i;
            }
        }
        if(count > 0){
            res = res /count;
        }
        
        float ret = cur_temp + (res * FUTURE_POINTS);
        next_temp[0] = ret;
        return true;
    }
    
    return false;
}

