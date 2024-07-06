/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Hue_EventStream.cpp
 * Author: Fedde
 * 
 * Created on 13 februari 2022, 9:28
 */

#include "Hue_EventStream.h"

Hue_EventStream::Hue_EventStream(Hue_Bridge *host_bridge) {
    this->host_bridge = host_bridge;
}

Hue_EventStream::Hue_EventStream(const Hue_EventStream& orig) {
}

Hue_EventStream::~Hue_EventStream() {
}

void *Hue_EventStream::run(){
    this->exit = false;  
    

    while (this->exit == false) {
        this->loop_worker();
        usleep(100*1000);
    }   
    return NULL;
};

void Hue_EventStream::loop_worker(){
    try{
    string readBuffer;
    long http_code;
    char* url;
    long response_code;
    double elapsed;
    std::string response_string;
    std::string header_string;
    CURL *curl;
    CURLcode res;
    struct curl_slist *chunk;
    if (this->host_bridge->get_state() > 0) {
            curl = curl_easy_init();
            if (curl) {
                string tmp = "https://" + this->host_bridge->get_ip() + "/eventstream/clip/v2";
                string hdr = "hue-application-key: " + this->host_bridge->get_username();
                //string tmp = "http://" + this->ipaddress + "/api";
                chunk = NULL;
                chunk = curl_slist_append(chunk, hdr.c_str());
                //chunk = curl_slist_append(chunk, "Host:k");
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
                curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
                curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
                curl_easy_setopt(curl, CURLOPT_URL, tmp.c_str());


                res = curl_easy_perform(curl);
                while ((res == CURLE_OK)&&(this->exit == false)) {
                   // cout << "Eventloop recieved" << response_string << endl;
                    this->host_bridge->add_EventStreamBuffer(response_string);
                    response_string = "";
                    res = curl_easy_perform(curl);
                }
                curl_slist_free_all(chunk);
                curl_easy_cleanup(curl);
            }
            cout << "Eventstream Restarting" << endl;
        }    
    }catch(exception &e){
        WriteError("Hue EventStream", e.what());                 
    }
};

void Hue_EventStream::quit(){
  this->exit = true;  
};

