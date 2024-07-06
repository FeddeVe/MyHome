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

#ifndef TREE_SERVER_H
#define TREE_SERVER_H


#include <string.h>
#include<iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sys/time.h>

#include "../../Global.h"

#include "../../tcpacceptor.h" 
#include <mutex>
#include "../../Thread.h"
#include "../../Master.h"
 
#include <signal.h>
#include <chrono>
#include <cstdio>


#include "Tree_Structs.h"
#include "Tree_Message.h"
#include "Tree_Node.h"
#include "Tree_OTA.h"

/*
#include "../../rapidjson/document.h"
#include "../../rapidjson/stringbuffer.h"
#include "../../rapidjson/writer.h"
#include "../../rapidjson/error/en.h"
#include "../../rapidjson/filereadstream.h"
 */

#define MAX_RESEND 10
#define MAX_LAYERS 25

//class Master;
class Tree_Node;
class Tree_OTA;

using namespace std; 
using namespace std::chrono;
//using namespace rapidjson;

  struct _str_DateTime{
        uint64_t timestamp_s;
        uint8_t WeekDay;
        char WeekDayString[32];
        uint8_t Month;
        char MonthString[32];
        uint8_t hour;
        uint8_t minute;
        char ClockString[32];
        uint8_t day;
        uint16_t year;
    };

enum NODE_TYPES{ 
  MASTER = 0,
  MSH_DEBUG = 1,
  MSH_DISPLAY = 2,
  MSH_BUTTONBOX = 3,
  MSH_POWERMETER = 4
};

class Tree_Message;
class Master;

class Tree_Server:public Thread{
private:  
    mutex m;
    Master *refmaster;
   // mutex send_mutex;
     std::vector<Tree_Message*>new_msg_buffer; 
    std::vector<Tree_Message*>rec_buffer;
    std::vector<Tree_Message*>send_buffer;
     std::mutex calc_mutex;
    std::mutex buffer_mutex;
    std::mutex send_mutex;
    std::mutex ota_mutex;
    std::mutex rec_mutex;
    std::mutex network_send_mutex;
    uint32_t msg_id;
    int sleep;    
    bool handle_msg(Tree_Message *m);
    _str_DateTime datetime;
    
public:
    Tree_Server(Master *refmaster);
    ~Tree_Server();   
    
    bool exit;
    TCPStream* stream;
    TCPAcceptor* acceptor; 
    Tree_OTA *OTA;
    
    void init();   
    void *run();
    void quit();  
    void network_send(char *data, uint16_t len);    
    void send_message(Tree_Message *m); 
    void do_work();
    void buffer_tasks();
    void Ident(uint32_t nodeid);
    Tree_Node *get_Node(uint32_t nodeid);
    string GetJson();
    uint32_t get_msg_id();
    uint64_t last_message_send;
    uint64_t last_mesh_info_send;
    uint16_t mesh_info_interval;
    void send_ping();
    void send_mesh_info();
    uint64_t get_timestamp_ms();
    vector<Tree_Node*>nodes;
    //void send_socket_ready();
    void run_task();
    //for debug
    int index;
    bool host_ready; 
    vector<uint64_t>msg_min_buffer;
    uint32_t msg_min;
    bool connected;    
    //void create_config_file();
    
    
};


#endif /* TCP_H */

