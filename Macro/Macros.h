/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Macros.h
 * Author: Fedde
 *
 * Created on 18 november 2023, 12:25
 */

#ifndef MACROS_H
#define MACROS_H

#include "Macro.h"
#include "../Global.h"
#include "../Master.h"

class Macro;

class Macros {
public:
    Macros(Master *refmaster);
    Macros(const Macros& orig);
    virtual ~Macros();
    
    void reload_macro(int msid);
    void delete_macro(int msid);
    bool isrunning(int macroid);
    void do_work();
    void start_macro(int macroid);
    void toggle_macro(int macroid);
    void disable_macro(int areaid = -1);
private:
    
    Master *refmaster;
    vector<Macro*>macros;
    mutex m;
    
    void load_macros();

};

#endif /* MACROS_H */

