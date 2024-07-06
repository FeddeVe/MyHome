/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   hardware_nrf_msg.h
 * Author: Fedde
 *
 * Created on 6 februari 2021, 17:37
 */

#ifndef HARDWARE_NRF_MSG_H
#define HARDWARE_NRF_MSG_H

class hardware_nrf_msg {
public:
    hardware_nrf_msg();
    hardware_nrf_msg(const hardware_nrf_msg& orig);
    virtual ~hardware_nrf_msg();
    
    unsigned char addr[5];
    unsigned char msg[32];
private:

};

#endif /* HARDWARE_NRF_MSG_H */

