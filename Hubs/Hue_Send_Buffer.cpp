/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Hue_Send_Buffer.cpp
 * Author: Fedde
 * 
 * Created on 13 februari 2022, 9:33
 */

#include "Hue_Send_Buffer.h"

Hue_Send_Buffer::Hue_Send_Buffer(string url, string body, int type) {
    this->url = url;
    this->body = body;
    this->type = type;
}

Hue_Send_Buffer::Hue_Send_Buffer(const Hue_Send_Buffer& orig) {
}

Hue_Send_Buffer::~Hue_Send_Buffer() {
}

