/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Tree_Message.cpp
 * Author: Fedde
 * 
 * Created on 25 maart 2023, 15:51
 */

#include "Tree_Message.h"



Tree_Message::Tree_Message() {
    this->header_new = false;
    this->clean_data();
}

Tree_Message::~Tree_Message() {
    
}

Tree_Message::Tree_Message(Tree_Node *from, uint32_t to, uint16_t msg_type, uint32_t msgid, uint32_t msg_size, char *data) {
    this->is_mijn = false;
    this->header = new HDR();
    this->resend_cnt = 0;
    this->header->to = to;     
    this->header->from = 0;     
    this->header->type = msg_type;
    this->header->msg_id = msgid;
    this->header->node_type = 0;
    if(to == 0xFFFFFFFF){
        this->is_broadcast = true;
    }else if(to != 0){
        this->is_broadcast = false;
        this->header->firmware = from->firmware;
        this->header->config = from->config_version;
    }
   // this->header = new HDR();
   // this->header_new = true;
    //this->clean_data();
   
    if (msg_size <= MAX_MSG_SIZE) {
        this->header->data_size = msg_size;
    } else {
        this->header->data_size = MAX_MSG_SIZE;
    }    
    memcpy(this->header->data, data, msg_size);    
    this->create_raw_data();
    if(msg_size == MAX_MSG_SIZE){
        int bp = 0;
        bp++;
    }
                
    delete this->header;
}

Tree_Message::Tree_Message(vector<char>data) {
    this->err = false; 
    size_t len = data.size();
    if (len < sizeof(HDR) - MAX_MSG_SIZE)
     {
        cout << "Something strange recieved, LEN TO SHORT - " <<len <<  endl;
        this->err = true;
     }else if(len > sizeof(HDR)){
           cout << "Something strange recieved, LEN TO LONG - " << len << endl;
           //len = sizeof(HDR);
            this->err = true;
     }
     else
     { 
         this->rawdata.resize(len);
         memcpy(&this->rawdata[0], &data[0], len);
         this->header = (HDR*) this->rawdata.data();
         int bp = 0;
         bp++;
     } 
}

Tree_Message::Tree_Message(bool ack, uint32_t to, uint16_t msg_type, uint32_t msgid){
    if(to == 0xFFFFFFFF){
        this->is_broadcast = true;
    }else{
        this->is_broadcast = false;
    }
     this->header = new HDR(); 
    this->header->to = to;     
    this->header->from = 0; 
    this->header->type = msg_type;
    this->header->msg_id = msgid; 
    this->header->node_type = 0;
    this->header->data_size = 0;
    this->create_raw_data();
    delete this->header;
}

Tree_Message::Tree_Message(char *special, size_t special_len, uint32_t to, uint16_t msg_type, uint32_t msgid, uint32_t msg_size, char *data){
    if(to == 0xFFFFFFFF){
        this->is_broadcast = true;
    }else{
        this->is_broadcast = false;
    }
    this->header = new HDR();
    this->resend_cnt = 0;
    this->header->to = to;     
    this->header->from = 0;     
    this->header->type = msg_type;
    this->header->msg_id = msgid;
    this->header->node_type = 0;
    if (msg_size <= MAX_MSG_SIZE) {
        this->header->data_size = msg_size;
    } else {
        this->header->data_size = MAX_MSG_SIZE;
    }    
    if(special_len < 16){
        special_len = 16;
    }
    memcpy(this->header->SPECIAL, special, special_len);
    memcpy(this->header->data, data, msg_size);    
    this->create_raw_data();
    delete this->header;
};

void Tree_Message::clean_data(){
   
};

void Tree_Message::create_raw_data() {   
    this->header->identifier = '#';
    this->header->identifier2 = '#';
    this->header->raw_size = sizeof(HDR) - (MAX_MSG_SIZE - this->header->data_size);
    this->raw_data_size = this->header->raw_size;
    this->UID = this->header->msg_id;
    this->msg_type = this->header->type;
    this->to = this->header->to;
   // this->header.raw_size = sizeof(HDR);
    //this->header.raw_size = this->rawdata_size;
    this->rawdata.resize(this->header->raw_size);
    memcpy(&this->rawdata[0], this->header, this->rawdata.size());   
    this->rawdata[0] = '#';
    uint16_t tmp = this->header->raw_size;
    
    this->rawdata[2] = (tmp &0xFF);
    tmp = tmp >> 8;
    this->rawdata[1] = (tmp &0xFF);
    this->rawdata[3] = '#';
    
}

void Tree_Message::send_msg(Tree_Server *client) { 
    this->time_stamp = client->get_timestamp_ms();
    //this->create_raw_data();
    //client->network_send(this->rawdata, this->raw_data_size);
    client->network_send(this->rawdata.data(), this->rawdata.size());
}

void Tree_Message::resend_msg(Tree_Server *client){
    this->time_stamp = client->get_timestamp_ms(); 
   // client->network_send(this->rawdata, this->raw_data_size);
     client->network_send(this->rawdata.data(), this->rawdata.size());
    this->resend_cnt++;
};


