/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Scene.h
 * Author: Fedde
 *
 * Created on 23 februari 2022, 12:17
 */

#ifndef SCENE_H
#define SCENE_H

#include "../Global.h"
#include "Scene_Values.h"
 

class Scene {
public:
    Scene(int msid, string name);
    Scene(const Scene& orig);
    virtual ~Scene();
    void Load_Scene();
    string Get_Name();
    void Set_Name(string name);
    int Get_ID();
    string Get_Values();
    void Save_Scene_Values(Value &values);
    void Create_Scene_Values(vector<Scene_Values*> values);
    Scene_Values *Get_Value(string lightid);
    vector<Scene_Values*>Values;
    bool active;
private:
    mutex m;
    
    int msid;
    string name; 

};

#endif /* SCENE_H */

