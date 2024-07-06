/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Tree_OTA.h
 * Author: Fedde
 *
 * Created on 2 april 2023, 17:45
 */

#ifndef TREE_OTA_H
#define TREE_OTA_H

#include <string.h>
#include<iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sys/time.h>

#include "Tree_OTA_list_item.h"
#include "Tree_Node.h"
#include "Tree_Message.h"
#include "Tree_Server.h"
#include "../../Global.h"

using namespace std;

class Tree_Message;
class Tree_OTA_list_item;
class Tree_Node;

struct OTA_Header{
  uint32_t total_size;
  uint32_t index;
};

class Tree_OTA {
public:
    Tree_OTA();
    Tree_OTA(const Tree_OTA& orig);
    virtual ~Tree_OTA();    
    void load_list();
    Tree_Message *check_firmware(Tree_Node *node);
    Tree_Message *handle_OTA_Request(Tree_Node *node, Tree_Message *m);
    void loop_work();
    
    
private:
    mutex m; 
    vector<Tree_OTA_list_item*>OTAlist;
    vector<Tree_Message*>OTA_msg_buffer;
    string main_folder;
    bool ota_running;
    uint64_t ota_running_set;
    uint32_t msgid;
    uint32_t get_msgid();
        Tree_Message *create_message(Tree_Node *node, uint32_t index);

   
    

};

#endif /* TREE_OTA_H */

