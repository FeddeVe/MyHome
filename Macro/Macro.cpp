/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Macro.cpp
 * Author: Fedde
 * 
 * Created on 18 november 2023, 11:56
 */

#include "Macro.h"

Macro::Macro(int msid, string name, Master *refmaster) {
    this->msid = msid;
    this->name = name;
    this->refmaster = refmaster;
    this->LoadSteps();
}

Macro::Macro(const Macro& orig) {
}

Macro::~Macro() {
    for(int i = 0; i < this->steps.size(); i++){
        delete this->steps[i];
    }
    this->steps.clear();
    
}

void Macro::LoadSteps(){
    m.lock();
    for(int i = 0; i < this->steps.size(); i++){
        delete this->steps[i];
    }
    this->steps.clear();
    this->steps.shrink_to_fit();
    
    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "SELECT delay, function, intensity, fadetime, releasetime, areaid, areascene, systemfunction FROM Macro_Step WHERE macroid ="+ inttostring(this->msid);
        if (!mysql_query(ms, query.c_str())) {
            MYSQL_RES *result = mysql_store_result(ms);
            if (result != NULL) {
                //Get the number of columns
                int num_rows = mysql_num_rows(result);
                int num_fields = mysql_num_fields(result);

                MYSQL_ROW row; //An array of strings
                while ((row = mysql_fetch_row(result))) {
                    if (num_fields >= 1) {
                        uint32_t delay = atoi(row[0]);
                        int function = atoi(row[1]);
                        int intensity = atoi(row[2]);
                        uint32_t fadetime = atoi(row[3]);
                        uint32_t releasetime = atoi(row[4]);
                        int areaid = atoi(row[5]);
                        int areascene = atoi(row[6]);
                        int systemfunction = atoi(row[7]);
                        Macro_Step *step = new Macro_Step(delay, function, intensity, fadetime, releasetime, areaid, areascene, systemfunction, this->refmaster);
                        this->steps.push_back(step);
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

void Macro::do_work(){
    m.lock();
    for(int i = 0; i < this->steps.size(); i++){
        this->steps[i]->do_work();
    }
        
        m.unlock();
}

void Macro::disable(int areaid){
     m.lock();
    for(int i = 0; i < this->steps.size(); i++){
        this->steps[i]->disable(areaid);
    }
        
        m.unlock();
}

bool Macro::isrunning(){
    bool ret = false;
    m.lock();
    for(int i = 0; i<this->steps.size(); i++){
        if(this->steps[i]->isrunning() == true){
            ret = true;
            break;
        }
    }
    m.unlock();
    return ret;
}

void Macro::start(){
    m.lock();
    for(int i = 0; i < this->steps.size(); i++){
        this->steps[i]->start();
    }
    
    m.unlock();
    this->do_work();
}

