/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Button.cpp
 * Author: Fedde
 * 
 * Created on 15 februari 2022, 20:03
 */

#include "Button.h"

Button::Button(Master *refmaster,Hue_Device *host, string ownerid, string name, string productname) {
    this->refmaster = refmaster;
    this->ownerid = ownerid;
    this->name = name;
    this->productname = productname;
    this->detect = false;
    this->host = host;   
}

Button::Button(const Button& orig) {
}

Button::~Button() {
    for (int i = 0; i < this->buttons.size(); i++) {
        delete this->buttons[i];
    }
}

string Button::getID() {
    return this->ownerid;
}

void Button::Set_Button(string id, int controlid, string last_event) {
   lock_guard<mutex>lg(m);
    if (this->detect) {
        if (last_event == "initial_press") {
            m.unlock();
            this->refmaster->buttons->detect_result(this->ownerid);
            m.lock();
        }
    } else {
        int ctrlid = controlid - 1;
        while (this->buttons.size() < ctrlid + 1) {
            Button_Member *m = new Button_Member(this, this->refmaster, "");            
            this->buttons.push_back(m);            
        }
       // Button_Member *b = this->buttons[ctrlid];
        if (ctrlid == -1) {
            for (int i = 0; i < this->buttons.size(); i++) {
                if (this->buttons[i]->get_id() == id) {
                    this->buttons[i]->set_ctrlid(i);
                   // b = this->buttons[i];
                    this->buttons[i]->set_last_event(id, last_event);
                }
           }
        }else{
            this->buttons[ctrlid]->set_id(id);
            int bp = 0;
            bp++;
        }         
    }
   
};

void Button::set_name(string name) {
    lock_guard<mutex>lg(m);
    this->name = name;
     
};

Value Button::GetJsonValue(Document::AllocatorType &allocator) {
    lock_guard<mutex>lg(m);
    Value obj;
    obj.SetObject();
     
    Value id;
    id.SetString(this->ownerid.c_str(), allocator);
    obj.AddMember("ID", id, allocator);
    Value name;
    name.SetString(this->name.c_str(), allocator);
    obj.AddMember("Name", name, allocator);
    obj.AddMember("BatteryLevel", this->battery_level, allocator);
    Value batterystate;
    batterystate.SetString(this->battery_state.c_str(), allocator);
    obj.AddMember("BatteryState", batterystate, allocator);
    Value productname;
    productname.SetString(this->productname.c_str(), allocator);
    obj.AddMember("ProductName", productname, allocator);
    Value constate;
    constate.SetString(this->Con_State.c_str(), allocator);
    obj.AddMember("ConState", constate, allocator);
    obj.AddMember("Online", this->online, allocator);
    Value MyArray(rapidjson::kArrayType);
    for (int i = 0; i < this->buttons.size(); i++) {
        Value bt;
        bt.SetObject();
        bt.AddMember("NO", i, allocator);
        Value btnid;
        btnid.SetString(this->buttons[i]->get_id().c_str(), allocator);
        bt.AddMember("ID", btnid, allocator);
        Value last_state;
        last_state.SetString(this->buttons[i]->get_last_event().c_str(), allocator);
        bt.AddMember("LastEvent", last_state, allocator);
        bt.AddMember("ctrlid", i, allocator);
        Value action;
        action.SetObject();
        action.AddMember("AreaID", this->buttons[i]->action->areaID, allocator);
        action.AddMember("Function", this->buttons[i]->action->Function, allocator);
        action.AddMember("Intensity", this->buttons[i]->action->Intensity, allocator);
        action.AddMember("FadeTime", this->buttons[i]->action->FadeTime, allocator); 
        action.AddMember("ReleaseTime", this->buttons[i]->action->ReleaseTime, allocator);
        action.AddMember("AreaScene", this->buttons[i]->action->SceneID, allocator);
        action.AddMember("Macro", this->buttons[i]->action->MacroID, allocator);
        action.AddMember("SystemOption", this->buttons[i]->action->SystemFunction, allocator);
        bt.AddMember("Action", action, allocator);
        MyArray.PushBack(bt, allocator);
    }
    obj.AddMember("Buttons", MyArray, allocator);
    
    return obj;
};

void Button::set_batterylevel(float level) {
    lock_guard<mutex>lg(m);
    this->battery_level = level;
    
}

void Button::set_batterystate(string state) {
    lock_guard<mutex>lg(m);
    this->battery_state = state;
    
};

void Button::set_constate(string state, bool online) {
   lock_guard<mutex>lg(m);
    this->Con_State = state;
    this->online = online;
    
}

void Button::set_productname(string name) {
    lock_guard<mutex>lg(m);
    this->productname = name;
    

}

void Button::start_detect() {
   lock_guard<mutex>lg(m);
    this->detect = true;
     
};

void Button::stop_detect() {
   lock_guard<mutex>lg(m);
    this->detect = false;
     

};

void Button::save_button(string name, Value &data){
    lock_guard<mutex>lg(m);
    this->host->SetName(name);
    Value &buttons = data["Buttons"];
    for(int i = 0; i < buttons.Size(); i++){
        Value &button = buttons[i];
        string id = button["ID"].GetString();
        for(int y = 0; y < this->buttons.size(); y++){
            if(this->buttons[y]->get_id() == id){
                Value &vaction = button["Action"];
                int areaid = vaction["AreaID"].GetInt();
                int action = vaction["Function"].GetInt();
                int Intensity = vaction["Intensity"].GetInt();
                int FadeTime = vaction["FadeTime"].GetInt();
                int ReleaseTime = vaction["ReleaseTime"].GetInt();
                int SceneID = vaction["SceneID"].GetInt();
                int MacroID = vaction["MacroID"].GetInt();
                int SystemFunction = vaction["SystemFunction"].GetInt();
                
                this->buttons[y]->SaveAction(areaid, action, Intensity, FadeTime, ReleaseTime, SceneID, MacroID, SystemFunction);
            }
        }
    }
   
};