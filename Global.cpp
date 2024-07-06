/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Global.cpp
 * Author: Fedde
 * 
 * Created on 7 februari 2022, 21:23
 */

#include "Global.h" 

size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

MYSQL *Mysql_Connect(){
    
    MYSQL *ms = new MYSQL();
    mysql_init(ms); 
    if (ms == NULL) {
        return NULL;
    }
    if (mysql_real_connect(ms, "localhost", "user", "pass", "MyHome", 0, NULL, 0) == NULL) {
        string ms_err = string(mysql_error(ms));
        cout << "MYSQL ERROR " << ms_err << endl;
        delete ms;
        ms = NULL;
        return NULL;
    } else {
       // cout << "Database connection successful." << endl;
        return ms;
        //   printf("Database connection successful.\n");
    }
    
};

void Mysql_Disconnect(MYSQL *db){
  mysql_close(db); 
  delete db;
  db=NULL;
  //mysql_library_end();
  int bp = 0;
  bp++;
};

string GetJsonString(Document &doc)
{
  rapidjson::StringBuffer buffer;

  buffer.Clear();

  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);

  return string( buffer.GetString() );
}

string booltostring(bool b)
{
  return b ? "true" : "false";
}

string inttostring(int i){
  
stringstream ss;
ss << i;
return ss.str();
   
}

int hour_to_local(int hour){
    int ret = hour + 1;
    if (ret > 23){
        ret = ret - 24;
    }        
    return ret;
};

int hour_to_local2(int hour){
    int ret = hour + 1;
     
    return ret;
};

string floattostring(float i){
  
stringstream ss;
ss << i;
return ss.str();
   
}

string uint32tostring(uint32_t i){
       stringstream ss;
    ss << i;
    return ss.str();
};

uint64_t timeSinceEpochMillisec() {
 
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

string longtostring(long i){
    stringstream ss;
    ss << i;
    return ss.str();
    
}

void WriteError(string item, string msg){
    cout << "ERROR IN " << item << " - " << msg << endl;
    
      time_t t = std::time(0); 
      tm* now = std::gmtime(&t);
      string date = inttostring(now->tm_mday) + "-"+ inttostring(now->tm_mon) +"-"+ inttostring(now->tm_year + 1900);
      string time = inttostring(hour_to_local(now->tm_hour)) + ":" + inttostring(now->tm_min)+"." + inttostring(now->tm_sec);
      MYSQL *ms = Mysql_Connect();
      if(ms != NULL){
        string query = "INSERT INTO ErrorLog (Date, Time,Item, Msg) VALUES('"+ date +"','"+ time +"', '"+ item +"', '"+ msg +"')";
       if (!mysql_query(ms, query.c_str())) {

        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
            
        }
       Mysql_Disconnect(ms);
      }
};
