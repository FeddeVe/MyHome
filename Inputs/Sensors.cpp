/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Sensors.cpp
 * Author: Fedde
 * 
 * Created on 12 februari 2022, 20:02
 */

#include "Sensors.h"



Sensors::Sensors() {
    
}

Sensors::Sensors(const Sensors& orig) {
}

Sensors::~Sensors() {
    for(int i = 0; i < this->sensors.size(); i++){
        cout << "Killing Sensor " << this->sensors[i]->get_id() << endl;
     delete this->sensors[i];   
    }
}

void Sensors::do_work(){
  for(int i = 0; i < this->sensors.size(); i++){
      this->sensors[i]->do_work();
  }
};

void Sensors::Add_Sensor(Sensor *s){
    m.lock();
    bool nieuw = true;
    for(int i = 0; i < this->sensors.size(); i++){
        if(this->sensors[i]->get_id() == s->get_id()){
            nieuw = false;
        }
    }
    if(nieuw){
        this->sensors.push_back(s);
    }else{
        delete s;
    }
    m.unlock();
};

Sensor *Sensors::get_sensor(string id){
    m.lock();
    for(int i = 0; i < this->sensors.size(); i++){
        if(this->sensors[i]->get_id() == id){
            m.unlock();
            return this->sensors[i];
        }
                
    }
    m.unlock();
    return NULL;
}

string Sensors::GetJson() {
    Document d;
    d.SetObject();
    Document::AllocatorType &allocator = d.GetAllocator();
    m.lock();
    Value v;
    v.SetString("Sensors", allocator);
    d.AddMember("item", v, allocator);
    d.AddMember("id", -1, allocator);
    Value MyArray(rapidjson::kArrayType);
    for (int i = 0; i < this->sensors.size(); i++) {
        MyArray.PushBack(this->sensors[i]->get_json_value(allocator), allocator);
    }
    d.AddMember("Sensors", MyArray, allocator);

    m.unlock();


    string ret = GetJsonString(d);


    return ret;
};

void Sensors::sort_by_name(){
   sort(this->sensors.begin(), this->sensors.end(), [ ](const Sensor *lhs, const Sensor * rhs) {
        return lhs->name > rhs->name;
    });  
};

