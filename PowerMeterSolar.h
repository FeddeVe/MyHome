/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   PowerMeterSolar.h
 * Author: Fedde
 *
 * Created on 16 juni 2024, 22:01
 */

#ifndef POWERMETERSOLAR_H
#define POWERMETERSOLAR_H

#include "Global.h"
#include "powermeter.h"

class _powermeter;

class PowerMeterSolar : public Thread {
public:
    PowerMeterSolar(_powermeter *pwrmeter);
    PowerMeterSolar(const PowerMeterSolar& orig);
    virtual ~PowerMeterSolar();
    
    void *run();
    void quit();
    
private:
    _powermeter *m_powermeter;
     bool exit;
    void loop_worker();
    string token;

};

#endif /* POWERMETERSOLAR_H */

