/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Tree_Node.h
 * Author: Fedde
 *
 * Created on 25 maart 2023, 15:49
 */

#ifndef TREE_NODE_H
#define TREE_NODE_H

#include <string.h>
#include<iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cstdio>

#include "Tree_Message.h"
#include "Tree_OTA.h"
#include "Tree_Server.h"
#include "Tree_Structs.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h" 

class Tree_Message;
class Tree_Server;


using namespace rapidjson;

class Tree_Node {
public:
    Tree_Node(Tree_Server *reftcp, Tree_Message *m);
    Tree_Node(const Tree_Node& orig);
    virtual ~Tree_Node();
    
    void set_online(bool online);
    void send_test_msg();
    bool handle_msg(Tree_Message *m);
    void upload_file(string filename, uint64_t index);
    void announce_config_update();
    void upload_config(uint64_t index);
    
     
    uint32_t UID;
    uint16_t nodetype;
    uint16_t firmware;
    uint16_t avg_turn_time;
    uint32_t config_version; 
    
    //void buffer_tasks();
    uint32_t get_msg_id();
    
    void loop_worker();
    
    
    
    
private:
    Tree_Server *reftcp;
    mutex bfr_mutex;
    mutex m;
    mutex m_msgid;
   // vector<Tree_Message*>msg_buffer; 
    uint32_t msg_id;
    bool ota_running;    
    time_t ota_running_time;
    void calc_avg_turn_time(Tree_Message *m);
     void send_ack(uint32_t to, uint32_t msg_id);
    uint16_t arr_avg_turn_time[100];
    int arr_avg_turn_time_index;    
    bool online;
    bool config_loaded;
    string config_file; 
    uint64_t last_message_send;
    void load_config();
    void load_screen_config(); 
    uint32_t add_button(string fileloc);
    
    

};

#endif /* TREE_NODE_H */

