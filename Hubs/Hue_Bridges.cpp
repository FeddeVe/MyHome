/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Hue_Bridges.cpp
 * Author: Fedde
 * 
 * Created on 9 februari 2022, 12:11
 */

#include "Hue_Bridges.h"

Hue_Bridges::Hue_Bridges(Master *refmaster) {
    this->refmaster = refmaster;
    this->request_discover_bool = false;
    this->last_discover = 0;
    this->load_known_bridges();
}

Hue_Bridges::Hue_Bridges(const Hue_Bridges& orig) {
}

Hue_Bridges::~Hue_Bridges() {
    for(int i = 0; i < this->bridges.size(); i++){
     cout << "Stopping bridge " << this->bridges[i]->GetID() << endl;
     this->bridges[i]->quit();
     this->bridges[i]->join();
     delete this->bridges[i];
    }
    this->bridges.clear();
}

void Hue_Bridges::load_known_bridges() {
    MYSQL *db = Mysql_Connect();

    if (db != NULL) {
        string query = "SELECT Bridge_id,Bridge_ip, Username FROM Hue_Bridge";
        if (!mysql_query(db, query.c_str())) {
            MYSQL_RES *result = mysql_store_result(db);
            if (result != NULL) {
                //Get the number of columns
                int num_rows = mysql_num_rows(result);
                int num_fields = mysql_num_fields(result);

                MYSQL_ROW row; //An array of strings
                while ((row = mysql_fetch_row(result))) {
                    if (num_fields >= 1) {
                        string id = row[0];
                        string ip = row[1];
                        string username = row[2];
                        Hue_Bridge *br = new Hue_Bridge(id, ip, username, this->refmaster);
                        this->bridges.push_back(br);
                    }
                }
            }
            mysql_free_result(result);
        } else {
            string ms_err = string(mysql_error(db));
            cout << "MYSQL Error " << ms_err << endl;
        }
        Mysql_Disconnect(db);
    }
};

void Hue_Bridges::Discover() {
   
    string readBuffer;
    long http_code;
    char* url;
    long response_code;
    double elapsed;
    std::string response_string;
    std::string header_string;
    CURL *curl;

    curl = curl_easy_init();
    CURLcode res;
    if (curl) {
        string tmp = "https://discovery.meethue.com";
        //string tmp = "http://" + this->ipaddress + "/api";
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_URL, tmp.c_str());
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            
            //fprintf(stderr, "curl_easy_perform() failed: %s\n",
            //        curl_easy_strerror(res));

            //cout << "Bridge not found on : "+ this->ipaddress << endl;
            Document document;
            document.Parse(response_string.c_str());
            if (document.IsArray()) {
                for (const auto& point : document.GetArray()) {
                    string ip = point["internalipaddress"].GetString();
                    string id = point["id"].GetString();
                    //check new
                    Hue_Bridge *br = this->get_bridge(id);
                    if (br != NULL) {
                        br->setIP(ip);
                    } else {
                        // new bridge found
                        m.lock();
                        Hue_Bridge *br = new Hue_Bridge(id, ip, "", this->refmaster);
                        this->bridges.push_back(br);
                        m.unlock();
                        br->start();
                        MYSQL *ms = Mysql_Connect();
                        if (ms != NULL) {
                            string query = "INSERT into Hue_Bridge(Bridge_id, Bridge_ip, Username)VALUES('" + id + "', '" + ip + "', '')";
                            if (!mysql_query(ms, query.c_str())) {
                            } else {
                                string ms_err = string(mysql_error(ms));
                                cout << "MYSQL Error " << ms_err << endl;
                            }
                            Mysql_Disconnect(ms);
                        }
                        
                    }
                    cout << "Bridge IP Found " << ip << "id = " << id << endl;

                }


            };
        }
    }
    curl_easy_cleanup(curl);


};

Hue_Bridge *Hue_Bridges::get_bridge(string id) {
     std::lock_guard<mutex> lg(m);
    for (int i = 0; i < this->bridges.size(); i++) {
        if (this->bridges[i]->GetID() == id) {
            return this->bridges[i];
        }
    }
    return NULL;
};

void Hue_Bridges::start(){
  for(int i = 0; i < this->bridges.size(); i++){
      this->bridges[i]->start();
  }
};

string Hue_Bridges::get_json(){
   std::lock_guard<mutex> lg(m);
  Document d;
  d.SetObject();
  Document::AllocatorType &allocator = d.GetAllocator();
  
  Value v;
  v.SetString("Bridges", allocator);
  d.AddMember("item", v, allocator);
  d.AddMember("id", -1, allocator);
  Value MyArray(rapidjson::kArrayType);
  for(int i = 0; i < this->bridges.size(); i++){
      MyArray.PushBack(this->bridges[i]->GetJsonValue(allocator), allocator);
  }
  d.AddMember("Bridges", MyArray, allocator);
  
   
   
  string ret = GetJsonString(d);
  
  
  return ret;
};

void Hue_Bridges::request_discover(){
    std::lock_guard<mutex> lg(m);
  this->request_discover_bool = true;  
  
};

void Hue_Bridges::do_work(){
   
       
    
    try{
     if(this->request_discover_bool){
          
        time_t diff = time(0) - this->last_discover;
        if(diff > 60){
         //   WriteError("Bridges Discover", "TEST");
            this->Discover();
            this->last_discover = time(0);
            this->request_discover_bool = false;
        }
         
    }
    }catch(exception &e){
        WriteError("Bridges Do_Work", e.what());
    }
     
    for(int i = 0; i < this->bridges.size(); i++){  
      //  WriteError("Bridge Do_Work", this->bridges[i]->get_ip());
     this->bridges[i]->do_work();   
    }
     
      
   
};


