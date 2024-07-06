/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   powermeter.h
 * Author: Fedde
 *
 * Created on 9 juli 2018, 13:21
 */

#ifndef POWERMETER_H
#define POWERMETER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <ctime>

#include "Global.h"


#include <pthread.h>
#include "Thread.h"
#include <unistd.h>

#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <curl/curl.h> 
#include "rapidjson/document.h"
#include <vector>
#include <mutex> 
#include "powermeter_values.h"
#include <malloc.h>
#include "PowerMeterSolar.h"


class _powermeter_values;
class PowerMeterSolar;

class _powermeter {
private:
    mutex m;
    bool exit;
     
    time_t last_save;
    long tarif1; // WAT UUR!!
    long tarif2; // WAT UUR!!
    long tarif_total; //WAT UUR!!
    long tarif1Returned;
    long tarif2Returned;
    long totalReturned;
    long current; // WAT!!
    long returned;
    long volt_l1;
    long volt_l2;
    long volt_l3;
    long amp_l1;
    long amp_l2;
    long amp_l3;
    long gas; 
    long cur_solar;
    long lifetime_solar;
    
    
    void save_data();
    
    PowerMeterSolar *solar;
     
public:
    _powermeter();
    ~_powermeter();
    
    void do_work();
    void quit();
    
    //void set_nrf_value(unsigned char *nrf_data);
    void set_data(uint32_t t1, uint32_t t2, int32_t live, uint32_t gas, uint32_t ret1, uint32_t ret2, uint32_t l1_volt, uint32_t l2_volt, uint32_t l3_volt, uint32_t l1_amp, uint32_t l2_amp, uint32_t l3_amp, uint32_t live_returned);
    void set_solar(uint32_t cur_solar, uint64_t lifetimesolar);
    string get_json();
    
    
    
    

};

#endif /* POWERMETER_H */

