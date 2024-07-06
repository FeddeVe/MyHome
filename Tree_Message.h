/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Tree_Message.h
 * Author: Fedde
 *
 * Created on 25 maart 2023, 15:51
 */
 
#ifndef TREE_MESSAGE_H
#define TREE_MESSAGE_H

#include "Tree_Server.h"
#include "Tree_Node.h"

#define MAX_MSG_SIZE 1024
#define MAX_RESEND 10

class Tree_Server;

enum msg_types {
   ping = 0,
    socket_ready = 1,
    ack = 2, //ack or next index 
    mesh_info = 3,
    OTA_Announce = 4,
    OTA_Request = 5,
    OTA_Response = 6,
    OTA_Error = 7,
    Req_Display_Config = 8,
    Display_Config = 9,
    File_Request = 10,
    File_Response = 11,
    Config_Request_Announce = 12,
    Config_Request = 13,
    Config_Response = 14,
    Config_Anounce = 15,
    Display_Report = 16,
    Door_Window_Sensor = 17,
    Pir_Sensor = 18,
    Button_Click = 19,
    BSEC_Data = 20,
    Error_msg = 21,
    Hello = 22,
    Lux_report = 23,
    Time_Report = 24,
    Area_Report = 25,
    Temperature = 26,
    test = 5432
};

struct HDR {
    uint8_t identifier : 8;
        uint16_t raw_size : 16;
        uint8_t identifier2 : 8;
        uint32_t from : 32;
        uint32_t to : 32;
        uint16_t type : 16;
        uint32_t msg_id : 32;
        uint16_t data_size : 16;
        uint16_t node_type : 16;
        uint16_t firmware : 16;
        uint32_t config : 32;
        uint64_t uptime : 64;
        char SPECIAL[16]; // size of 2x64
        char data[MAX_MSG_SIZE];
};

class Tree_Message {
public:
    Tree_Message();
    ~Tree_Message();
    Tree_Message(Tree_Node *from, uint32_t to, uint16_t msg_type, uint32_t msgid, uint32_t msg_size, char *data);
    Tree_Message(vector<char>data);
    Tree_Message(bool ack, uint32_t to, uint16_t msg_type, uint32_t msgid); // ACK MSG
    Tree_Message(char *special, size_t special_len, uint32_t to, uint16_t msg_type, uint32_t msgid, uint32_t msg_size, char *data);
    void send_msg(Tree_Server *client);
    void resend_msg(Tree_Server *client);
    HDR *header;
    uint64_t time_stamp; // USED FOR RECENDING ACK        
    //char data[MAX_MSG_SIZE]; //Data 
    vector<char>rawdata;
    //char rawdata[1500];
    uint16_t raw_data_size;
    uint32_t to;
    bool is_mijn;
    bool is_broadcast;
    uint8_t resend_cnt;
    bool err;
    void create_raw_data();
    uint32_t UID;
     uint16_t msg_type;
    

    // uint32_t bytes_recieved;


private:
    void clean_data();
    bool header_new;

};

#endif /* TREE_MESSAGE_H */

