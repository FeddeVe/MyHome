/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Areas.cpp
 * Author: Fedde
 * 
 * Created on 16 februari 2022, 10:37
 */

#include "Areas.h"

Areas::Areas(Master *refmaster) {
    this->refmaster = refmaster;
    load_areas();
}

Areas::Areas(const Areas& orig) {
}

Areas::~Areas() {
    for (int i = 0; i < this->areas.size(); i++) {
        delete this->areas[i];
    }
}

void Areas::load_areas() {
    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "SELECT id, name FROM Area";
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
                        Area *a = new Area(this->refmaster, msid, name);
                        this->areas.push_back(a);
                    }
                }
            }
            mysql_free_result(result);
        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
        }
        Mysql_Disconnect(ms);
    }
    this->sort_by_name();
};

void Areas::sort_by_name(){
   sort(this->areas.begin(), this->areas.end(), [ ](const Area *lhs,const Area * rhs) {
        return lhs->name < rhs->name;
    });  
};

string Areas::GetJson() {
    Document d;
    d.SetObject();
    Document::AllocatorType &allocator = d.GetAllocator();
    m.lock();
    Value v;
    v.SetString("Areas", allocator);
    d.AddMember("item", v, allocator);
    d.AddMember("id", -1, allocator);
    d.AddMember("GlobalHeatingEnabled", this->refmaster->heating_enabled, allocator);
    Value MyArray(rapidjson::kArrayType);
    for (int i = 0; i < this->areas.size(); i++) {
        MyArray.PushBack(this->areas[i]->GetJsonValue(allocator), allocator);
    }
    d.AddMember("Areas", MyArray, allocator);
    m.unlock();
    string ret = GetJsonString(d);
    return ret;
}

void Areas::do_work() {
    for (int i = 0; i < this->areas.size(); i++) {
        this->areas[i]->do_work();
    }
};

bool Areas::add_area(string name) {
    int msid = -1;
    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {        
            string query = "INSERT INTO Area(Name) VALUES('"+ name +"')";
            if (!mysql_query(ms, query.c_str())) {
                query = "SELECT LAST_INSERT_ID()";
                if (!mysql_query(ms, query.c_str())) {
                MYSQL_RES *result = mysql_store_result(ms);
                if (result != NULL) {
                    //Get the number of columns
                    int num_rows = mysql_num_rows(result);
                    int num_fields = mysql_num_fields(result);

                    MYSQL_ROW row; //An array of strings
                    while ((row = mysql_fetch_row(result))) {
                        if (num_fields >= 1) {
                            msid = atoi(row[0]);
                                                     }
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
    
    if(msid != -1){
        Area *a = new Area(this->refmaster, msid, name);
        this->areas.push_back(a);
    }
    
    this->sort_by_name();
    return true;
    };
    
    Area *Areas::get_area(int msid){
     // m.lock();
      for(int i =0; i < this->areas.size(); i++){
          if(this->areas[i]->get_msid() == msid){
             // m.unlock();
              return this->areas[i];
          }
      }
    //  m.unlock();
      return NULL;
        
    };
