/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Scene.cpp
 * Author: Fedde
 * 
 * Created on 23 februari 2022, 12:17
 */

#include "Scene.h"

Scene::Scene(int msid, string name) {
    this->msid = msid;
    this->name = name; 
    this->active = false;
    this->Load_Scene();
}

Scene::Scene(const Scene& orig) {
}

Scene::~Scene() {
    for (int i = 0; i < this->Values.size(); i++) {
        delete this->Values[i];
        this->Values[i] = NULL;
    }
    this->Values.shrink_to_fit();
    this->Values.clear();
}

string Scene::Get_Name() {
    return this->name;
};

void Scene::Set_Name(string name){
    this->name = name;
    
     MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "UPDATE Scenes SET name = '"+ name +"' WHERE SceneID=" + inttostring(this->msid);
        if (!mysql_query(ms, query.c_str())) {
            
        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
        }
        Mysql_Disconnect(ms);
    }

}

int Scene::Get_ID() {
    return this->msid;
}

void Scene::Load_Scene() {
    for (int i = 0; i < this->Values.size(); i++) {
        delete this->Values[i];
    }
    this->Values.clear();
    this->Values.shrink_to_fit();
    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "SELECT id, lightid, Brightness, CT, UseCT, Red, Green, Blue, InUse FROM Scene_Values WHERE SceneID=" + inttostring(this->msid);
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
                        string lightid = row[1];
                        Scene_Values *v = new Scene_Values(msid, lightid, this->msid);
                        v->set_brightness(atoi(row[2]));
                        v->set_kelvin(atoi(row[3]), atoi(row[4]));
                        v->set_rgb(atoi(row[5]), atoi(row[6]), atoi(row[7]));
                        v->set_in_use(atoi(row[8]));
                        this->Values.push_back(v);
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

string Scene::Get_Values() {
    Document d;
    d.SetObject();
    Document::AllocatorType &allocator = d.GetAllocator();
    m.lock();
    Value v;
    v.SetString("Scene", allocator);
    d.AddMember("item", v, allocator);
    d.AddMember("id", this->msid, allocator);
    Value v_name;
    v_name.SetString(this->name.c_str(), allocator);
    d.AddMember("Name", v_name, allocator);
    Value MyArray(rapidjson::kArrayType);
    for (int i = 0; i < this->Values.size(); i++) {
        Value val;
        val.SetObject();
        Value lightid;
        lightid.SetString(this->Values[i]->lightid.c_str(), allocator);
        val.AddMember("LightID", lightid, allocator);
        val.AddMember("InUse", this->Values[i]->inuse, allocator);
        val.AddMember("Intensity", this->Values[i]->raw_brightness, allocator);
        val.AddMember("Kelvin", this->Values[i]->kelvin, allocator);
        val.AddMember("UseCT", this->Values[i]->usect, allocator);
        val.AddMember("Red", this->Values[i]->red, allocator);
        val.AddMember("Green", this->Values[i]->green, allocator);
        val.AddMember("Blue", this->Values[i]->blue, allocator);
        MyArray.PushBack(val, allocator);
    }
    d.AddMember("Values", MyArray, allocator);
    m.unlock();
    string ret = GetJsonString(d);
    return ret;

};

Scene_Values *Scene::Get_Value(string lightid) {
    for (int i = 0; i < this->Values.size(); i++) {
        if (this->Values[i]->lightid == lightid) {
            return this->Values[i];
        }
    }
    return NULL;
};

void Scene::Create_Scene_Values(vector<Scene_Values*> values) {
    for (int i = 0; i < values.size(); i++) {
        string lightid = values[i]->lightid;
        Scene_Values *v = this->Get_Value(lightid);
        if (v == NULL) {
            v = new Scene_Values(lightid, this->msid);
            this->Values.push_back(v);
        }
        v->set_in_use(values[i]->inuse);
        v->set_brightness(values[i]->raw_brightness);
        v->set_kelvin(values[i]->kelvin, this->Values[i]->usect);       
        v->set_rgb(values[i]->red, values[i]->green, values[i]->blue);
        v->Save_Values(); 
    }
};

void Scene::Save_Scene_Values(Value &values) {
    
    
    if (values.IsObject()) {
        Value &val = values;
        string lightid = val["LightID"].GetString();
        Scene_Values *v = this->Get_Value(lightid);
        if (v == NULL) {
            v = new Scene_Values(lightid, this->msid);
            this->Values.push_back(v);
        }
        v->set_in_use(val["InUse"].GetBool());
        v->set_brightness(val["Intensity"].GetInt());
        v->set_kelvin(val["CT"].GetInt(), val["UseCT"].GetBool());

        int red = val["Red"].GetInt();
        int green = val["Green"].GetInt();
        int blue = val["Blue"].GetInt();
        v->set_rgb(red, green, blue);
        v->Save_Values();
        //this->Values.push_back(v);
    }

};