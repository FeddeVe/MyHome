/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Tree_OTA.cpp
 * Author: Fedde
 * 
 * Created on 2 april 2023, 17:45
 */

#include <mutex>

#include "Tree_OTA.h"

Tree_OTA::Tree_OTA() {
    this->main_folder = "/var/www/html/MeshFirmware/";
    this->ota_running = false;
    this->msgid = 0; 
    this->load_list();
    this->ota_running_set = time(0);
}

Tree_OTA::Tree_OTA(const Tree_OTA& orig) {
}

Tree_OTA::~Tree_OTA() {
}

void Tree_OTA::load_list() {
    m.lock();
    for (int i = 0; i < this->OTAlist.size(); i++) {
        delete this->OTAlist[i];
    }
    this->OTAlist.clear();
    this->OTAlist.shrink_to_fit();
    
     MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "SELECT NodeType, Version, FileName FROM Tree_Node_Firmware";
        if (!mysql_query(ms, query.c_str())) {
            MYSQL_RES *result = mysql_store_result(ms);
            if (result != NULL) {
                //Get the number of columns
                int num_rows = mysql_num_rows(result);
                int num_fields = mysql_num_fields(result);

                MYSQL_ROW row; //An array of strings
                while ((row = mysql_fetch_row(result))) {
                    if (num_fields >= 1) {
                         Tree_OTA_list_item *item = new Tree_OTA_list_item();
                         item->nodetype = atoi(row[0]);
                         item->firmware = atoi(row[1]);
                         item->filename = row[2];
                         this->OTAlist.push_back(item);
                    }
                }
                mysql_free_result(result);

            }
        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
        }
        Mysql_Disconnect(ms);
    } 
//    this->OTAlist.push_back(item);
    m.unlock();

};

Tree_Message *Tree_OTA::check_firmware(Tree_Node *node) {
    Tree_Message *ret = NULL;
    m.lock();
    //if (this->ota_running == false) {
        bool found = false;
        //check OTA BY NODEID
        for (int i = 0; i < this->OTAlist.size(); i++) {
            if ((this->OTAlist[i]->nodeid == node->UID)||(this->OTAlist[i]->nodetype == node->nodetype)){
                if(this->OTAlist[i]->firmware != node->firmware){
                found = true;
                ret = this->create_message(node, 0);
                } 
            }
        }        
    //}
    m.unlock();
    return ret;
}

Tree_Message *Tree_OTA::handle_OTA_Request(Tree_Node *node, Tree_Message *mes){
    m.lock();
    OTA_Header *hdr = (OTA_Header*) mes->header->SPECIAL;
    cout << "OTA REQUEST HDR INDEX " <<(uint32_t) hdr->index << endl;
    Tree_Message *ret = this->create_message(node, hdr->index);
    m.unlock();
    return ret;
};

Tree_Message *Tree_OTA::create_message(Tree_Node *node, uint32_t index) {
    string filename = this->main_folder;
    bool found = false;
    for (int i = 0; i < this->OTAlist.size(); i++) {
        if (this->OTAlist[i]->nodeid == node->UID) {
            filename = filename + this->OTAlist[i]->filename;
            found = true;
            break;
        }
    }
    if (found == false) {
        for (int i = 0; i < this->OTAlist.size(); i++) {
            if ((this->OTAlist[i]->nodetype == node->nodetype) && (this->OTAlist[i]->firmware != node->firmware)) {
                filename = filename + this->OTAlist[i]->filename;
                found = true;
                break;
            }
        }
    }
    if (found) {
        std::ifstream infile(filename);
        if(infile.good()){
        std::vector<char> buffer;

        //get length of file
        infile.seekg(0, infile.end);
        size_t length = infile.tellg();
        infile.seekg(0, infile.beg);

        //read file
        if (length > 0) {
            buffer.resize(length);
            infile.read(&buffer[0], length);
        }
        infile.close();
        if(index == 0){
           // Tree_Message *m = new Tree_Message(true, node->UID, msg_types::OTA_Announce, node->get_msg_id());
            OTA_Header *hdr = new OTA_Header();
            hdr->index = 0;
            hdr->total_size = length;
            char tmp[8];
            memcpy(tmp, hdr, 8);
            delete hdr;
           // memcpy(m->header.SPECIAL, &hdr, sizeof(OTA_Header));
            char send_buffer[MAX_MSG_SIZE-1];
            uint16_t msg_size = 0;
            for(int i = 0; i < length; i++){
                send_buffer[i] = buffer[i];
                msg_size++;
                if(msg_size == MAX_MSG_SIZE-1){
                    break;
                }
            }     
            
            Tree_Message *m = new Tree_Message(tmp, 8, node->UID, msg_types::OTA_Announce, node->get_msg_id(), msg_size, send_buffer);
            //m->header.data_size = msg_size;
            this->ota_running = true;
            this->ota_running_set = time(0);
            return m;
        }else{
            //Tree_Message *m = new Tree_Message(true, node->UID, msg_types::OTA_Response, node->get_msg_id());
            OTA_Header *hdr = new OTA_Header();
            hdr->index = index;
            hdr->total_size = length;
             char tmp[8];
            memcpy(tmp, hdr, 8);
            delete hdr;
            uint16_t msg_size = 0;
            char send_buffer[MAX_MSG_SIZE-1];
            for(int i = index; i < length; i++){
                send_buffer[msg_size] = buffer[i];
                msg_size++;
                if(msg_size == MAX_MSG_SIZE-1){
                    break;
                }
            }    
            cout << "OTA REQUEST index= " << (uint32_t) index << " MSG SIZE = " << (uint16_t) msg_size << endl;
             Tree_Message *m = new Tree_Message(tmp, 8, node->UID, msg_types::OTA_Response, node->get_msg_id(), msg_size, send_buffer);           
            this->ota_running = true;
            this->ota_running_set = time(0);
            return m;            
        }
        }
    };
    return NULL;
};

void Tree_OTA::loop_work(){
    m.lock();
    if(this->ota_running){
    uint64_t diff = time(0) - this->ota_running_set;
    if(diff > 120){
        this->ota_running = false;
    }
    }
    
    m.unlock();
};
