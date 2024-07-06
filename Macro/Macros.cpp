/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Macros.cpp
 * Author: Fedde
 * 
 * Created on 18 november 2023, 12:25
 */

#include "Macros.h"

Macros::Macros(Master *refmaster) {
    this->refmaster = refmaster;
    this->load_macros();
}

Macros::Macros(const Macros& orig) {
}

Macros::~Macros() {
}

void Macros::load_macros(){
    m.lock();
     MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "SELECT id, name FROM Macro";
        if (!mysql_query(ms, query.c_str())) {
            MYSQL_RES *result = mysql_store_result(ms);
            if (result != NULL) {
                //Get the number of columns
                int num_rows = mysql_num_rows(result);
                int num_fields = mysql_num_fields(result);

                MYSQL_ROW row; //An array of strings
                while ((row = mysql_fetch_row(result))) {
                    if (num_fields >= 1) {
                        int msid = atoi(row[0]);
                        string name = row[1];
                        Macro *m = new Macro(msid, name, this->refmaster);
                        this->macros.push_back(m);
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
    
    m.unlock();
}

void Macros::reload_macro(int msid){
    m.lock();
    bool found = false;
    for(int i = 0; i < this->macros.size(); i++){
        if(this->macros[i]->msid == msid){
            this->macros[i]->LoadSteps();
            found = true;
        }
    }
    if(found == false){
         MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "SELECT id, name FROM Macro WHERE id = "+ inttostring(msid);
        if (!mysql_query(ms, query.c_str())) {
            MYSQL_RES *result = mysql_store_result(ms);
            if (result != NULL) {
                //Get the number of columns
                int num_rows = mysql_num_rows(result);
                int num_fields = mysql_num_fields(result);

                MYSQL_ROW row; //An array of strings
                while ((row = mysql_fetch_row(result))) {
                    if (num_fields >= 1) {
                        int msid = atoi(row[0]);
                        string name = row[1];
                        Macro *m = new Macro(msid, name, this->refmaster);
                        this->macros.push_back(m);
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
        
        
    }
    
    m.unlock();
}

bool Macros::isrunning(int macroid){
    bool ret = false;
    m.lock();
    for(int i = 0; i < this->macros.size(); i++){
        if(this->macros[i]->msid == macroid){
            ret = this->macros[i]->isrunning();
            break;
        }
    }
    m.unlock();
    return ret;
}

void Macros::delete_macro(int msid){
    m.lock();
    for(int i = 0; i < this->macros.size(); i++){
        if(this->macros[i]->msid == msid){
            delete this->macros[i];
            this->macros.erase(this->macros.begin() + i);
            break;
        }
    }
    m.unlock();
}

void Macros::do_work(){
    m.lock();
    for(int i = 0; i < this->macros.size(); i++){
        this->macros[i]->do_work();
    }
    m.unlock();
}

void Macros::start_macro(int macroid){
    m.lock();
     for(int i = 0; i < this->macros.size(); i++){
       if(this->macros[i]->msid == macroid){
           this->macros[i]->start();
       }
    }
    m.unlock();
}

void Macros::toggle_macro(int macroid){
    bool running = this->isrunning(macroid);
    m.lock();
    Macro *macro = NULL;
    for(int i = 0; i < this->macros.size(); i++){
        if(this->macros[i]->msid == macroid){
            macro = this->macros[i];
            break;
        }
    }
    if(macro == NULL){
        m.unlock();
        return;
    }
    if(running){
        macro->disable();
        m.unlock();
        return;
    }
    macro->start();
    m.unlock();
}

void Macros::disable_macro(int areaid){
    m.lock();
    for(int i = 0; i < this->macros.size(); i++){
        this->macros[i]->disable(areaid);
    }
    m.unlock();
}
