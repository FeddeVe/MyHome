/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Button_Member.cpp
 * Author: Fedde
 * 
 * Created on 15 februari 2022, 20:05
 */

#include "Button_Member.h"

Button_Member::Button_Member(Button *host,Master *refmaster, string id) {
    this->host = host;   
    this->refmaster = refmaster;
    this->id = id;
    this->action = new Button_Action();
    //this->LoadAction();
    this->loaded = false;
}

Button_Member::Button_Member(const Button_Member& orig) {
}

Button_Member::~Button_Member() {
    delete this->action;
}

void Button_Member::LoadAction(){
     MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        cout << "Loading button id" << endl;
        cout << this->id << endl;
        string query = "SELECT id, AreaID, Function, Intensity, FadeTime, ReleaseTime, SceneID, MacroID, SystemFunction FROM Button_Action WHERE ButtonID='"+this->id +"'";
        if (!mysql_query(ms, query.c_str())) {
            MYSQL_RES *result = mysql_store_result(ms);
            if (result != NULL) {
                //Get the number of columns
                int num_rows = mysql_num_rows(result);
                int num_fields = mysql_num_fields(result);

                MYSQL_ROW row; //An array of strings
                while ((row = mysql_fetch_row(result))) {
                    if (num_fields >= 1) {
                        this->action->msid = atoi(row[0]);
                        this->action->areaID = atoi(row[1]);
                        this->action->Function = atoi(row[2]);
                        this->action->Intensity = atoi(row[3]);
                        this->action->FadeTime = atoi(row[4]);
                        this->action->ReleaseTime = atoi(row[5]);
                        this->action->SceneID = atoi(row[6]);    
                        this->action->MacroID = atoi(row[7]);
                        this->action->SystemFunction = atoi(row[8]);
                        this->loaded = true;
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

void Button_Member::SaveAction(int AreaID, int Function, int Intensity, uint32_t FadeTime, uint32_t ReleaseTime, int SceneID, int MacroID, int SystemFunction){
    bool update = false;
    int msid = action->msid;
      MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "INSERT INTO Button_Action(ButtonID, AreaID, Function, Intensity, FadeTime, ReleaseTime, SceneID, MacroID, SystemFunction) VALUES('"+ this->id +"', "+ inttostring(AreaID)+ ","+inttostring(Function)+","+ inttostring(Intensity)+", "+ inttostring(FadeTime)+","+ inttostring(ReleaseTime)+","+ inttostring(SceneID)+", "+ inttostring(MacroID)+","+ inttostring(SystemFunction)+")";
        if(this->action->msid != -1){
            update = true;
            query = "UPDATE Button_Action SET AreaID="+ inttostring(AreaID)+", Function="+inttostring(Function)+", Intensity="+ inttostring(Intensity)+", FadeTime="+ inttostring(FadeTime)+ ", ReleaseTime="+inttostring(ReleaseTime)+", SceneID="+ inttostring(SceneID)+", MacroID ="+ inttostring(MacroID) +", SystemFunction ="+inttostring(SystemFunction)+" WHERE ButtonID = '"+ this->id +"'";
        }
        if (!mysql_query(ms, query.c_str())) {
            
            if(update == false){
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
            }
             
            action->areaID = AreaID;
            action->Function = Function;
            action->Intensity = Intensity;
            action->FadeTime = FadeTime;
            action->ReleaseTime = ReleaseTime;
            action->SceneID = SceneID;
            action->MacroID = MacroID;
            action->SystemFunction = SystemFunction;
            action->msid = msid;
        
            
        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
        }
        Mysql_Disconnect(ms);
    }        
}

void Button_Member::set_ctrlid(int ctrl_id){
    this->ctrl_id = ctrl_id + 1;
}

void Button_Member::set_last_event(string id, string last_event){
 
 //this->id = id;
 this->last_event = last_event;
 cout << "Last Event = " << last_event << endl;
 //if(this->last_event == "initial_press"){
     cout << "Buttonpress detected" << endl;
     cout << this->host->getID() << endl;
     
     /*
     if(this->host->getID() == "8b0cfa39-0deb-422c-a90b-a4372a055d45"){
        // woonkamer
         Area *a = this->refmaster->areas->get_area(1);
         if(a != NULL){
             if(this->ctrl_id == 1){
                 a->set_button_brightness(100,0);
             }else if(this->ctrl_id == 2){
                 a->set_button_color(3000, 100);
             }else if(this->ctrl_id == 3){
                 a->set_button_brightness(0, 10*60);
             }else if(this->ctrl_id == 4){
                 a->set_button_brightness(0,0);
             }
         }  
     }
      */
     if(this->action->Function == 0){
         //System Function; 
     }else if(this->action->Function == 1){
         // Area Intensity
         Area *a = this->refmaster->areas->get_area(this->action->areaID);
         if(a!=NULL){        
             a->set_brightness(action->Intensity, action->FadeTime, action->ReleaseTime * 60, "Button", true); 
         }
     }else if(this->action->Function == 2){
         // Area Scene
          Area *a = this->refmaster->areas->get_area(this->action->areaID);
         if(a!=NULL){        
             a->Set_Scene(action->SceneID, action->FadeTime * 1000, action->ReleaseTime * 60 * 1000);
         }
     }else if(this->action->Function == 3){
         //Macro
         this->refmaster->macros->start_macro(action->MacroID);
     }else if(this->action->Function == 4){
         //None
     }
     
     
     
 //}
 
}

string Button_Member::get_id(){
    return this->id;
};

string Button_Member::get_last_event(){
    return this->last_event;
}

void Button_Member::set_id(string id){
    this->id = id;
    if(this->loaded == false){
    this->LoadAction();
    }
}

