/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Lights.cpp
 * Author: Fedde
 * 
 * Created on 11 februari 2022, 20:29
 */

#include "Lights.h"

Lights::Lights() {
     
}

Lights::Lights(const Lights& orig) {
}

Lights::~Lights() {
    for (int i = 0; i < this->lights.size(); i++) {
        cout << "Killing light "<< this->lights[i]->get_id() << endl;
        delete this->lights[i];
    }
}

Light *Lights::getlight(string id) {
    m.lock();
    for (int i = 0; i < this->lights.size(); i++) {
        if (this->lights[i]->get_id() == id) {
            m.unlock();
            return this->lights[i];
        }
    }
    m.unlock();
    return NULL;

};

void Lights::add_light(Light *l) {
    bool nieuw = true;
    m.lock();
    for (int i = 0; i < this->lights.size(); i++) {
        if (this->lights[i]->get_id() == l->get_id()) {
            nieuw = false;
        }
    }
    if (nieuw) {
        
        this->lights.push_back(l);
      
    }else{
     delete l;   
    }
    this->sort_by_name();
    m.unlock();
}

void Lights::do_work() {
   // m.lock();
    for (int i = 0; i < this->lights.size(); i++) {
        this->lights[i]->do_work();
    }
    //m.unlock();
}

string Lights::GetJson() {
    Document d;
    d.SetObject();
    Document::AllocatorType &allocator = d.GetAllocator();
    m.lock();
    Value v;
    v.SetString("Lights", allocator);
    d.AddMember("item", v, allocator);
    d.AddMember("id", -1, allocator);
    Value MyArray(rapidjson::kArrayType);
    for (int i = 0; i < this->lights.size(); i++) {
        MyArray.PushBack(this->lights[i]->get_json_value(allocator), allocator);
    }
    d.AddMember("Lights", MyArray, allocator);

    m.unlock();


    string ret = GetJsonString(d);


    return ret;
};

void Lights::sort_by_name(){
   sort(this->lights.begin(), this->lights.end(), [ ](const Light *lhs,const Light * rhs) {
        return lhs->name < rhs->name;
    });  
};
