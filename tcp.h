/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tcp.h
 * Author: Fedde
 *
 * Created on 17 februari 2016, 23:20
 */

#ifndef TCP_H
#define TCP_H


#include <string.h>
#include<iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "tcpacceptor.h" 
#include <mutex>

#include "Thread.h"
#include "Master.h"
#include <signal.h>

class Master;

using namespace std; 

class _tcp : public Thread {
private:
    Master *refmaster; 
    mutex m;
    string handle_incoming(string incoming);
    string handle_bridge(rapidjson::Value& object);
    string handle_lights(Value &object);
    string handle_sensors(Value &object);
    string handle_buttons(Value &object); 
    string handle_areas(Value &object);
    string handle_background(Value &object); 
    string handle_mesh(Value &object);
    string handle_macro(Value &object);
public:
    _tcp(Master *refmaster);
    ~_tcp();
    
    
    bool exit;
    TCPStream* stream;
    TCPAcceptor* acceptor; 
    
    void init();    
    void *run();    
    void quit();    
     
};


#endif /* TCP_H */

