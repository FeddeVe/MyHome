/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Master.h
 * Author: Fedde
 *
 * Created on 9 februari 2022, 11:56
 */

#ifndef MASTER_H
#define MASTER_H

#include "Global.h"
#include "Thread.h"


#include "Hubs/Tree_Mesh/Tree_Server.h"
#include "Hubs/Hue_Bridges.h"
#include "tcp.h"
#include "Outputs/Lights.h"
#include "Inputs/Sensors.h"
#include "Inputs/Buttons.h"
#include "Area/Areas.h"
#include "Background/Background.h" 
#include "powermeter.h"
#include "hardware.h"
#include "Macro/Macros.h"

class Tree_Server;
class _tcp;
class Hue_Bridges;
class Lights;
class Sensors;
class Buttons;
class Areas;
class Background; 
class _powermeter;
class _hardware;
class Macros;


class Master:public Thread {
public:
    
    Master();
    Master(const Master& orig);
    virtual ~Master();
    void *run();
    void Exit();
    
    Hue_Bridges *hue_bridges;
    _tcp *tcp;
    Lights *lights;
    Sensors *sensors;
    Buttons *buttons;
    Areas *areas;
    Background *background; 
    _powermeter *powermeter;
    _hardware *hardware;
    Tree_Server *tree_mesh;
    Macros *macros;
    
    bool heating_enabled;
    
private:
    mutex m;
    bool exit;
    int master_loop();
    void ClearLog();

};

#endif /* MASTER_H */

