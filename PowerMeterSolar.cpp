/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   PowerMeterSolar.cpp
 * Author: Fedde
 * 
 * Created on 16 juni 2024, 22:01
 */

#include "PowerMeterSolar.h"

PowerMeterSolar::PowerMeterSolar(_powermeter *powermeter) {
    this->m_powermeter = powermeter;
    this->token = "token check ";
}

PowerMeterSolar::PowerMeterSolar(const PowerMeterSolar& orig) {
}

PowerMeterSolar::~PowerMeterSolar() {
}

void *PowerMeterSolar::run() {
    this->exit = false;
    while (this->exit == false) {
        this->loop_worker();
        sleep(16);
    }
    return NULL;
}

void PowerMeterSolar::loop_worker() {
    try {
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

        curl = curl_easy_init();
        if (curl) {
            string tmp = "http://127.0.0.1/PowerMeter/Enphase.php";

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
            curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, CURL_MAX_READ_SIZE);
            curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
            //  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
            //curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, this->token.c_str());
            // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            // curl_easy_setopt(curl, CURLOPT_POST, 1L);  
            curl_easy_setopt(curl, CURLOPT_HTTP_TRANSFER_DECODING, 0);

            curl_easy_setopt(curl, CURLOPT_URL, tmp.c_str());


            res = curl_easy_perform(curl);
            if ((res == CURLE_OK)&&(this->exit == false)) {
             //   cout << response_string << endl;
                Document d;
                ParseResult ok = d.Parse(response_string.c_str());
                if (!ok) {
                    string error = GetParseError_En(ok.Code());

                } else {
                    int msgType = 0;
                    uint32_t solarNu = 0;
                    uint64_t solarLife = 0;
                    string data = "";
                    if (d.IsObject()) {
                        if (d.HasMember("msgType")) {
                            if (d["msgType"].IsInt()) {
                                msgType = d["msgType"].GetInt();
                            }
                        }
                        if (d.HasMember("data")) {
                            if (d["data"].IsString()) {
                                data = d["data"].GetString();
                            }
                        }
                        if (msgType == 1) {
                            this->token = data;
                        }
                        if (msgType == 2) {
                            Document d2;
                            ParseResult ok2 = d2.Parse(data.c_str());
                            if (!ok2) {
                                string error2 = GetParseError_En(ok.Code());

                            } else {
                                if (d2.IsObject()) {
                                    if (d2.HasMember("wattsNow")) {
                                        if (d2["wattsNow"].IsInt()) {
                                            solarNu = d2["wattsNow"].GetInt();
                                        }
                                    }
                                    if (d2.HasMember("wattHoursLifetime")) {
                                        if (d2["wattHoursLifetime"].IsUint64()) {
                                            solarLife = d2["wattHoursLifetime"].GetUint64();
                                        }
                                    }

                                    int bp = 0;
                                    bp++;

                                    //  }
                                }
                                int bp = 0;
                                bp++;
                            }
                        }


                    }
                    if(solarLife > 0){
                        this->m_powermeter->set_solar(solarNu, solarLife);
                    }
                }

                // cout << "Eventloop recieved" << response_string << endl;
                //this->host_bridge->add_EventStreamBuffer(response_string);
                // response_string = "";
                // res = curl_easy_perform(curl);
            } else {
                string err = curl_easy_strerror(res);
                // string err2  = curl_share_strerror(res);
                //string err3 = curl_easy_strerror(curl);
                int bp = 0;
                bp++;
                //curl not ok;
            }
            // curl_slist_free_all(chunk);

        }
        if (curl) {
            curl_easy_cleanup(curl);
        }


    } catch (exception &e) {
        WriteError("Hue EventStream", e.what());
    }
}

