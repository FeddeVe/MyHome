/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   powermeter_values.h
 * Author: Fedde
 *
 * Created on 10 juli 2018, 8:29
 */

#ifndef POWERMETER_VALUES_H
#define POWERMETER_VALUES_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <ctime>  
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

class _powermeter_values{
public:
    time_t date;
    float raw_value;
    
    _powermeter_values();
    ~_powermeter_values();
    
};


#endif /* POWERMETER_VALUES_H */

