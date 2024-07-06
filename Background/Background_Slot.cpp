/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Backgrund_Slot.cpp
 * Author: Fedde
 * 
 * Created on 19 februari 2022, 20:21
 */

#include "Background_Slot.h"

Background_Slot::Background_Slot(int id) {
    this->id = id;
    this->load_scenes();
}

Background_Slot::Background_Slot(const Background_Slot& orig) {
}

Background_Slot::~Background_Slot() {
    for(int i = 0; i < this->scenes.size(); i++){
     delete this->scenes[i];   
    }
    this->scenes.clear();
}

void Background_Slot::load_scenes(){
    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "SELECT id, Name FROM Scenes WHERE SlotID="+ inttostring(this->id);
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
                        Scene *s = new Scene(msid, name);
                        s->Load_Scene();
                        this->scenes.push_back(s);
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
    
    
};

void Background_Slot::add_scene(string name){
    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "INSERT INTO Scenes(SlotID, Name) VALUES("+ inttostring(this->id) +", '"+ name +"')";                
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
                            int id = atoi(row[0]);
                            Scene *s = new Scene(id, name);
                            if(this->scenes.size() > 0){
                              s->Create_Scene_Values(this->scenes[0]->Values);
                            }
                            this->scenes.push_back(s);
                            
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
    
};

string Background_Slot::Get_Scene_Values(int sceneid){
    for(int i = 0; i < this->scenes.size(); i++){
        if(this->scenes[i]->Get_ID() == sceneid){
            return this->scenes[i]->Get_Values();
        }
    }
    return "NOK";
}

void Background_Slot::Save_Scene_Values(int sceneid, Value &values){
     for(int i = 0; i < this->scenes.size(); i++){
        if(this->scenes[i]->Get_ID() == sceneid){
            return this->scenes[i]->Save_Scene_Values(values);
        }
    }
};

 