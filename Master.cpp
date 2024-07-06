/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Master.cpp
 * Author: Fedde
 * 
 * Created on 9 februari 2022, 11:56
 */

#include <condition_variable>

#include "Master.h" 

Master::Master() {
    this->ClearLog();
    WriteError("Master", "Starting");    
    this->heating_enabled = true;
    curl_global_init(CURL_GLOBAL_ALL);
    this->lights = new Lights();
    this->sensors = new Sensors();
    this->buttons = new Buttons();
    this->areas = new Areas(this);
    this->background = new Background(this);
    this->macros = new Macros(this);
    this->hue_bridges = new Hue_Bridges(this);    
    this->powermeter = new _powermeter();
    this->hardware = new _hardware(this, this->powermeter);
    this->tcp = new _tcp(this);
    this->tree_mesh = new Tree_Server(this);



    //INIT ALL
    this->tcp->init();

    //START ALL
    this->hue_bridges->start();
  //  this->background->start();
   
    //this->hardware->start();
    //this->powermeter->start();
    this->tree_mesh->start();
    this->tcp->start();
    
    //this->websocket->start();
}

Master::Master(const Master& orig) {
}

Master::~Master() {
}

void Master::ClearLog(){
  MYSQL *ms = Mysql_Connect();
      if(ms != NULL){
        string query = "DELETE FROM ErrorLog";
       if (!mysql_query(ms, query.c_str())) {

        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
            
        }
       Mysql_Disconnect(ms);
      }
  int bp = 0;
  bp++;
};

int Master::master_loop(){
    auto t1 = high_resolution_clock::now();
    auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    int ms = 0;

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;
     t1 = high_resolution_clock::now();
       //   cout << "Bridges do work" << endl;
     //WriteError("HueBridges Do_Work", "TEST");
        this->hue_bridges->do_work();
      //  cout << "Backround do work"<<endl;
       // WriteError("Background Do_Work", "TEST");
        this->background->do_work();
       // WriteError("Mesh Do_Work", "TEST");
         
        // cout << "Sensors do work" << endl;
        try {
        //    WriteError("Sensors Do_Work", "TEST");
            this->sensors->do_work();
        } catch (exception &e) {
            WriteError("Sensors Do_Work", e.what());
        }
        this->macros->do_work();
       //     cout << "Areas do work" << endl;
      //  WriteError("Areas Do_Work", "TEST");
        this->areas->do_work();
        //   cout << "Lights do work" << endl;
       // WriteError("Lights Do_Work", "TEST");
        
        
        this->lights->do_work();
       // cout << "End do work" << endl;
      // WriteError("Hardware Do_Work", "TEST");
        this->hardware->do_work();
      //  WriteError("PowerMeter Do_Work", "TEST");
        this->powermeter->do_work();
        
        this->tree_mesh->do_work();
        t2 = high_resolution_clock::now();

        /* Getting number of milliseconds as an integer. */
        ms_int = duration_cast<milliseconds>(t2 - t1);

        /* Getting number of milliseconds as a double. */
        // duration<double, std::milli> ms_double = t2 - t1;
        // cout << "RUNNING TIME WAS " << ms_int.count() << endl;
        ms = ms_int.count();
        return ms;
}

void *Master::run() {
    this->exit = false;
    int sleep = 0;
    while (this->exit == false) {
        sleep = this->master_loop();
       
        if (sleep > 250) {
            cout << "Master Loop reached 250+: " << sleep << endl;
        } else {
            usleep((250 - sleep) * 1000);
        }

    }
    cout << "Stopping Background" << endl;
    //this->background->quit();
    //this->background->join();

    cout << "Stopping Server" << endl;


    this->tcp->quit();
    this->tcp->join();
    cout << "Stopping Mesh" << endl;
    
     
    
    cout << "Cleaning up" << endl;
    delete this->tcp;
    delete this->hue_bridges;
    delete this->areas;
    delete this->background;
    delete this->lights;
    delete this->sensors;
    delete this->buttons;
    cout << "Killing Powermeter " << endl;
    delete this->powermeter;
    cout << "Killing Hardware " << endl;
    delete this->hardware;
    cout << "Killing MESH " << endl;
    

    curl_global_cleanup();

    return 0;

};

void Master::Exit() {
    m.lock();
    this->exit = true;
    m.unlock();
};
