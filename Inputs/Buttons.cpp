/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Buttons.cpp
 * Author: Fedde
 * 
 * Created on 15 februari 2022, 20:03
 */

#include "Buttons.h"

Buttons::Buttons() {
    this->detect = false;
}

Buttons::Buttons(const Buttons& orig) {
}

Buttons::~Buttons() {
    for (int i = 0; i < this->buttons.size(); i++) {
        delete this->buttons[i];
    }
}

Button *Buttons::get_button(string id) {
    m.lock();
    for (int i = 0; i < this->buttons.size(); i++) {
        if (this->buttons[i]->getID() == id) {
            m.unlock();
            return this->buttons[i];
        }
    }

    m.unlock();
    return NULL;

};

void Buttons::add_button(Button *b) {
    m.lock();
    bool nieuw = true;
    for (int i = 0; i < this->buttons.size(); i++) {
        if (this->buttons[i]->getID() == b->getID()) {
            nieuw = false;
        }
    }
    if (nieuw) {
        this->buttons.push_back(b);
    } else {
        delete b;
    }
    m.unlock();

};

string Buttons::GetJson() {
    Document d;
    d.SetObject();
    Document::AllocatorType &allocator = d.GetAllocator();
    m.lock();
    Value v;
    v.SetString("Bridges", allocator);
    d.AddMember("item", v, allocator);
    d.AddMember("id", -1, allocator);
    Value MyArray(rapidjson::kArrayType);
    for (int i = 0; i < this->buttons.size(); i++) {
        MyArray.PushBack(this->buttons[i]->GetJsonValue(allocator), allocator);
    }
    d.AddMember("Buttons", MyArray, allocator);
    m.unlock();
    string ret = GetJsonString(d);
    return ret;
};

void Buttons::start_detect() {
    m.lock();
    this->detectedid = "";
    this->detect = true;
    for (int i = 0; i < this->buttons.size(); i++) {
        this->buttons[i]->start_detect();
    }
    m.unlock();
};

void Buttons::detect_result(string id) {
    m.lock();
    this->detectedid = id;
    this->detect = false;
    for (int i = 0; i < this->buttons.size(); i++) {
        this->buttons[i]->stop_detect();
    }
    m.unlock();
};

void Buttons::cancel_detect() {
    m.lock();
    this->detectedid = "";
    this->detect = false;
    for (int i = 0; i < this->buttons.size(); i++) {
        this->buttons[i]->stop_detect();
    }
    m.unlock();
};

string Buttons::GetJsonDetect() {
    Document d;
    d.SetObject();
    Document::AllocatorType &allocator = d.GetAllocator();
    m.lock();
    Value v;
    v.SetString("ButtonRequest", allocator);
    d.AddMember("item", v, allocator);
    d.AddMember("id", -1, allocator);
    d.AddMember("Detect", this->detect, allocator);
    Value id;
    id.SetString(this->detectedid.c_str(), allocator);
    d.AddMember("DetectID", id, allocator);
    m.unlock();
    string ret = GetJsonString(d);
    return ret;

};

void Buttons::SaveButton(string id, string name, Value &data){
  
  Button *b = this->get_button(id);
  if(b != NULL){
      b->save_button(name, data);
  }
 
};
