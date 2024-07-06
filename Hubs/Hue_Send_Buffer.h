/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Hue_Send_Buffer.h
 * Author: Fedde
 *
 * Created on 13 februari 2022, 9:33
 */

#ifndef HUE_SEND_BUFFER_H
#define HUE_SEND_BUFFER_H

#include "../Global.h"

enum BUF_TYPE{
  GET,
  PUT
};

class Hue_Send_Buffer {
public:    
    Hue_Send_Buffer(string url, string body, int type);
    Hue_Send_Buffer(const Hue_Send_Buffer& orig);
    virtual ~Hue_Send_Buffer();
    string url; 
    string body; 
    int type;
private:

};

#endif /* HUE_SEND_BUFFER_H */

