/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Global.h
 * Author: Fedde
 *
 * Created on 7 februari 2022, 21:23
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#define RAPIDJSON_HAS_STRSTRING 1
#define _GNU_SOURCE 1

#define HEATING_MIN_TEMP 12

#include <cstddef>
#include <string>
#include <iostream>
#include <mariadb/mysql.h>
#include <unistd.h>
#include <mutex>
#include <curl/curl.h>
#include <vector> 
#include <sstream>
#include <math.h>
#include <algorithm>
#include <chrono>
#include <exception>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"

using namespace std;
using namespace std::chrono;
using namespace rapidjson;

 size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp);
 MYSQL *Mysql_Connect();
 void Mysql_Disconnect(MYSQL *db);
 string GetJsonString(Document &doc);
 string booltostring(bool b);
 string inttostring(int i);
// int hour_to_local(int hour);
 string floattostring(float i);
 string uint32tostring(uint32_t i);
 uint64_t timeSinceEpochMillisec(); 
 string longtostring(long i);
 
// int hour_to_local2(int hour);
 
 void WriteError(string item, string msg);

#endif /* GLOBAL_H */

