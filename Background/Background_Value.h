/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Background_Value.h
 * Author: Fedde
 *
 * Created on 25 februari 2022, 21:46
 */

#ifndef BACKGROUND_VALUE_H
#define BACKGROUND_VALUE_H

#include "../Global.h"

class Background_Value {
public:
    Background_Value();
    Background_Value(const Background_Value& orig);
    virtual ~Background_Value();
    
    string lightid;
    int old_dim;
    int new_dim;
    int res_dim;
    int old_ct;
    int new_ct;
    int res_ct;
    int old_x;
    int new_x;
    int res_x;
    int old_y;
    int new_y;
    int res_y;
private:

};

#endif /* BACKGROUND_VALUE_H */

