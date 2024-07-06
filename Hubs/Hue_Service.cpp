/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Hue_Service.cpp
 * Author: Fedde
 * 
 * Created on 13 februari 2022, 9:20
 */

#include "Hue_Service.h"

Hue_Service::Hue_Service(int service_type, string service_id) {
    this->service_id = service_id;
    this->service_type = service_type;
    this->last_update = 0;
}

Hue_Service::Hue_Service(const Hue_Service& orig) {
}

Hue_Service::~Hue_Service() {
}

