/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Hue_Bridge.cpp
 * Author: Fedde
 * 
 * Created on 9 februari 2022, 21:09
 */
 
#include <mutex>

#include "Hue_Bridge.h"
#include "Hue_Bridges.h"

Hue_Bridge::Hue_Bridge(string id, string ip, string username, Master *refmaster) {
    this->refmaster = refmaster;
    cout << "Creating Hue Bridge " << id << endl;
    this->ip = ip;
    this->id = id;
    this->username = username;
    this->state = 0;
    this->stream = new Hue_EventStream(this);

}

Hue_Bridge::Hue_Bridge(const Hue_Bridge& orig) {
}

Hue_Bridge::~Hue_Bridge() {
    this->stream->quit();
    this->stream->join();
    delete this->stream;
    for (int i = 0; i < this->devices.size(); i++) {
        delete this->devices[i];
    }
    for (int i = 0; i < this->send_buffer.size(); i++) {
        delete this->send_buffer[i];
    }


}

void Hue_Bridge::setIP(string ip) {

    std::lock_guard<mutex> lg(m);
    if (this->ip != ip) {
        this->ip = ip;
        MYSQL *ms = Mysql_Connect();
        if (ms != NULL) {
            string query = "UPDATE Hue_Bridge SET Bridge_ip = '" + ip + "' WHERE Bridge_id = '" + this->id + "'";
            if (!mysql_query(ms, query.c_str())) {
            } else {
                string ms_err = string(mysql_error(ms));
                cout << "MYSQL Error " << ms_err << endl;
            }
            Mysql_Disconnect(ms);
        }

    }
    // m.unlock();
    //  cout <<"END SetIP" << endl;
};

string Hue_Bridge::GetID() {
    std::lock_guard<mutex> lg(m);
    return this->id;
};

void Hue_Bridge::quit() {
    std::lock_guard<mutex> lg(m);
    this->exit = true;
};

void *Hue_Bridge::run() {
    Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/device", "", BUF_TYPE::GET);
    this->add_send_buffer(buf);
    this->stream->start();
    
    this->exit = false;

    while (this->exit == false) {
       
        try {
            this->loop_worker();
        } catch (exception &e) {
            WriteError("Bridge " + this->name + " - Loop_worker", e.what());
        }
        
       // try {

        //    this->handle_streamevents();
        //} catch (exception &e) {
        //    WriteError("Bridge " + this->name + " - HandeStreamEvents", e.what());
        //}

        usleep(100 * 1000);
    }
    /*
    string readBuffer;
    long http_code;
    char* url;
    long response_code;
    double elapsed;
    std::string response_string;
    std::string header_string;
    CURL *curl;
    CURLcode res;

    while (this->exit == false) {
        if (this->state > 0) {
            curl = curl_easy_init();
            if (curl) {
                string tmp = "https://" + this->ip + "/eventstream/clip/v2";
                string hdr = "hue-application-key: " + this->username;
                //string tmp = "http://" + this->ipaddress + "/api";
                struct curl_slist *chunk = NULL;
                chunk = curl_slist_append(chunk, hdr.c_str());
                //chunk = curl_slist_append(chunk, "Host:k");
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
                curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
                curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);

                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
                curl_easy_setopt(curl, CURLOPT_URL, tmp.c_str());


                res = curl_easy_perform(curl);
                while (res == CURLE_OK) {

                    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
                    //fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    //        curl_easy_strerror(res));
                    if (http_code == 403) {
                        this->state = 0;
                    } else if (http_code == 200) {
                        this->state = 2;
                    }
                    cout << "Eventloop recieved" << response_string << endl;
                    Document d;
                    d.Parse(response_string.c_str());
                    if(d.IsObject()){
                        cout << "D is OBJECT "<<endl;
                    }
                    if (d.IsArray()) {
                        for (int i = 0; i < d.Size(); i++) {
                            Value &v = d[i];
                            
                            if (v.HasMember("data")) {
                                Value &d = v["data"];
                                if (d.IsArray()) {
                                    for (int i = 0; i < d.Size(); i++) {
                                        Value &a = d[i];
                                        if(a.HasMember("type")){
                                            string type = a["type"].GetString();
                                            if((type == "light") || (type == "motion")){
                                            }else{
                                            int bp = 0;
                                            bp++;
                                            }
                                        }
                                        if(a.HasMember("owner")){
                                        string owner_id = a["owner"]["rid"].GetString();
                                        Light *l = this->refmaster->lights->getlight(owner_id);
                                        if (l != NULL) {
                                            if (a.HasMember("color")) {
                                                l->set_act_color(a["color"]["xy"]["x"].GetFloat(), a["color"]["xy"]["y"].GetFloat());
                                            }
                                            if (a.HasMember("color_temperature")) {
                                                int mirek = 0;
                                                if (a["color_temperature"]["mirek"].IsInt()) {
                                                    mirek = a["color_temperature"]["mirek"].GetInt();
                                                }
                                                l->set_act_ct(mirek, a["color_temperature"]["mirek_valid"].GetBool());
                                            }
                                            if (a.HasMember("dimming")) {
                                                float brightness = a["dimming"]["brightness"].GetFloat();

                                                l->set_act_bri(brightness);

                                            }
                                            if (a.HasMember("on")) {
                                                bool on = a["on"]["on"].GetBool();
                                                l->set_act_on(on);
                                            }
                                            if (a.HasMember("metadata")) {

                                                string name = a["metadata"]["name"].GetString();

                                                l->set_name(name);
                                            }
                                            if(a.HasMember("status")){
                                                string status = a["status"].GetString();
                                                l->set_con_state(status);
                                                int bp = 0;
                                                bp++;
                                            }
                                            if(a.HasMember("metadata")){                                                 
                                                string name = a["metadata"]["name"].GetString();
                                                l->set_name(name);
                                            }
                                        }
                                        // end lights
                                        Sensor *s = this->refmaster->sensors->get_sensor(owner_id);
                                        if(s != NULL){
                                            if(a.HasMember("motion")){
                                                s->set_motion(a["motion"]["motion"].GetBool());
                                            }
                                            
                                        }
                                        //end sensors
                                             


                                        }
                                    }
                                }


                            }
                        }

                    }

                    response_string = "";

                    res = curl_easy_perform(curl);
                }
                this->state= 1;
                
            }
            curl_easy_cleanup(curl);
        }
        sleep(1); 
    }
     */
    return 0;
};

void Hue_Bridge::handle_incoming() {
    
    Document d;
    for (int i = 0; i < this->incoming.size(); i++) {
        ParseResult ok = d.Parse(this->incoming[i].c_str());
        if (!ok) {
            string error = GetParseError_En(ok.Code());
            WriteError("Bridge " + this->name + " - Recieve Error", error);
        } else {
            if (d.IsObject()) {
                if (d.HasMember("data")) {
                    Value &data = d["data"];
                    if (data.IsArray()) {
                        for (int i = 0; i < data.Size(); i++) {
                            Value &item = data[i];
                            this->handle_msg(item);
                        }
                    }
                }
            }
        }
    }
    this->incoming.clear();
    this->incoming.shrink_to_fit();

};

void Hue_Bridge::loop_worker() {
    
   
    auto t1 = high_resolution_clock::now();
    auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    int ms = 0;
    string readBuffer;
    long http_code;
    char* url;
    long response_code;
    double elapsed;
    std::string response_string;
    std::string header_string;
    string msg_url;
    CURL *curl = NULL;
    string body;
    CURLcode res;
    string hdr;
    string tmp;
    int msg_type;
    struct curl_slist *chunk;
    Document d;
    m.lock();
    while (this->send_buffer.size() > 0) {
        t1 = high_resolution_clock::now();
        this->msg_timeout = 0;
        msg_url = this->send_buffer[0]->url;
        body = this->send_buffer[0]->body;
        msg_type = this->send_buffer[0]->type;
        if (!curl) {
            curl = curl_easy_init();
        }
        if (curl) {
            response_string = "";
            tmp = "https://" + this->ip + "/clip/v2/" + msg_url;
            // cout << "Hub Request: " << tmp << endl;
            hdr = "hue-application-key: " + this->username;
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
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2l);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_URL, tmp.c_str());
            if (msg_type == BUF_TYPE::PUT) {
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str()); /* data goes here */
            }

            m.unlock();
            res = curl_easy_perform(curl);
            m.lock();
            if (res == CURLE_OK) {

                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
                if (http_code == 403) {
                    this->state = 0;
                } else if (http_code == 200) {
                    this->state = 2;
                }
                this->incoming.push_back(response_string);

            } else {
                this->state = 1;
            }
            curl_slist_free_all(chunk);
            delete this->send_buffer[0];
            this->send_buffer.erase(this->send_buffer.begin());
            this->send_buffer.shrink_to_fit();
            int bp = 0;
            bp++;
            //  cout <<"SEND BUFFER SIZE = " << this->send_buffer.size() << endl;


        }
        auto t2 = high_resolution_clock::now();

        /* Getting number of milliseconds as an integer. */
        auto ms_int = duration_cast<milliseconds>(t2 - t1);

        /* Getting number of milliseconds as a double. */
        // duration<double, std::milli> ms_double = t2 - t1;        
        ms = ms_int.count();
        m.unlock();
        if (ms > 100) {
            // cout << "BRIDGE RUNNING TIME EXEEDED 100ms : " << ms_int.count() << endl;
        } else {
            usleep((100 - ms) * 1000);
        }
        m.lock();
       
    }
    this->msg_timeout++;
    if (this->msg_timeout > 600) {
        this->msg_timeout = 0;
        Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/device", "", BUF_TYPE::GET);
        // m->unlock();
        this->add_send_buffer_NO_LOCK(buf);
        // m->lock();
    }
    if (curl) {
        curl_easy_cleanup(curl);
    }
    m.unlock();
    
    
};

void Hue_Bridge::handle_streamevents() {
     
    Document d;
    // m2->lock();
    if (this->EventStreamBuffer.size() > 0) {
        ParseResult ok = d.Parse(this->EventStreamBuffer[0].c_str());
        if (!ok) {
            string error = GetParseError_En(ok.Code());
            WriteError("Bridge " + this->name + " - EventStream Error", error);
        } else {
            if (d.IsArray()) {
                for (int i = 0; i < d.Size(); i++) {
                    if (d[i].HasMember("data")) {
                        Value &data = d[i]["data"];
                        if (data.IsArray()) {
                            for (int y = 0; y < data.Size(); y++) {
                                Value &item = data[y];
                                this->handle_msg(item);
                            }
                        }
                    }
                }
                d.Clear();
            }
        }
        this->EventStreamBuffer.erase(this->EventStreamBuffer.begin());
    }
    // m2->unlock();
    //m.unlock();
};

void Hue_Bridge::handle_msg(Value & data) {

    if (data.HasMember("type")) {
        string type = data["type"].GetString();
        //  cout << "Handle_Msg " << type << endl;
        if (type == "device") {
            this->handle_device_data(data);
        } else if (type == "device_power") {
            this->handle_device_power(data);
        } else if (type == "light") {
            this->handle_light(data);
        } else if (type == "light_level") {
            this->handle_light_level(data);
        } else if (type == "motion") {
            this->handle_motion(data);
        } else if (type == "temperature") {
            this->handle_temperature(data);
        } else if (type == "zigbee_connectivity") {
            this->handle_zigbee_connectivity(data);
        } else if (type == "button") {
            this->handle_button(data);

        } else {

           // cout << "Type not supported " << type << endl;
        }

        //  cout << "End Handle_Msg " << type << endl;
    }



};

void Hue_Bridge::handle_device_data(Value & data) {

    string id = data["id"].GetString();
    Hue_Device *dev = this->get_device(id);
    if (dev == NULL) {

        dev = new Hue_Device(id, this, this->refmaster);
        this->devices.push_back(dev);

    }

    if (data.HasMember("metadata")) {
        Value &metadata = data["metadata"];
        dev->set_metadata(metadata);
    }

    if (data.HasMember("product_data")) {
        Value &pruduct_data = data["product_data"];
        dev->set_product_data(pruduct_data);
    }

    if (data.HasMember("services")) {
        Value &services = data["services"];
        dev->set_services(services);
    }


};

void Hue_Bridge::handle_device_power(Value &data) {
    if (data.HasMember("owner")) {
        string ownerid = data["owner"]["rid"].GetString();
        Hue_Device *dev = this->get_device(ownerid);
        if (dev != NULL) {
            if (data.HasMember("power_state")) {
                Value &power_state = data["power_state"];
                dev->set_device_power(power_state);
            }
        } else {
            //nieuw device???
            Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/device", "", BUF_TYPE::GET);

            this->add_send_buffer_NO_LOCK(buf);

        }
    }
};

void Hue_Bridge::handle_light(Value &data) {
    if (data.HasMember("owner")) {
        string ownerid = data["owner"]["rid"].GetString();
        Hue_Device *dev = this->get_device(ownerid);
        if (dev != NULL) {
            dev->set_light(data);
        } else {
            //nieuw device???
            Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/device", "", BUF_TYPE::GET);

            this->add_send_buffer_NO_LOCK(buf);

        }
    }
};

void Hue_Bridge::handle_light_level(Value &data) {
    if (data.HasMember("owner")) {
        string ownerid = data["owner"]["rid"].GetString();
        Hue_Device *dev = this->get_device(ownerid);
        if (dev != NULL) {
            dev->set_light_level(data);
        } else {
            //nieuw device???
            Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/device", "", BUF_TYPE::GET);

            this->add_send_buffer_NO_LOCK(buf);

        }
    }
};

void Hue_Bridge::handle_motion(Value &data) {
    if (data.HasMember("owner")) {
        string ownerid = data["owner"]["rid"].GetString();
        Hue_Device *dev = this->get_device(ownerid);
        if (dev != NULL) {
            dev->set_motion(data);
        } else {
            //nieuw device???
            Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/device", "", BUF_TYPE::GET);

            this->add_send_buffer_NO_LOCK(buf);

        }
    }
};

void Hue_Bridge::handle_temperature(Value &data) {
    if (data.HasMember("owner")) {
        string ownerid = data["owner"]["rid"].GetString();
        Hue_Device *dev = this->get_device(ownerid);
        if (dev != NULL) {
            dev->set_temperature(data);
        } else {
            //nieuw device???
            Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/device", "", BUF_TYPE::GET);

            this->add_send_buffer_NO_LOCK(buf);

        }
    }
};

void Hue_Bridge::handle_zigbee_connectivity(Value &data) {
    if (data.HasMember("owner")) {
        string ownerid = data["owner"]["rid"].GetString();
        Hue_Device *dev = this->get_device(ownerid);
        if (dev != NULL) {
            dev->set_zigbee_connectivity(data);
        } else {
            //nieuw device???
            Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/device", "", BUF_TYPE::GET);

            this->add_send_buffer_NO_LOCK(buf);

        }
    }
};

void Hue_Bridge::handle_button(Value &data) {
    if (data.HasMember("owner")) {
        string ownerid = data["owner"]["rid"].GetString();
        Hue_Device *dev = this->get_device(ownerid);
        if (dev != NULL) {
            dev->set_button(data);
        } else {
            //nieuw device???
            Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/device", "", BUF_TYPE::GET);

            this->add_send_buffer_NO_LOCK(buf);

        }
    }
};

Value Hue_Bridge::GetJsonValue(Document::AllocatorType & allocator) {
    lock_guard<mutex>lg(m);
    Value obj;
    obj.SetObject();

    Value id;
    id.SetString(this->id.c_str(), allocator);
    obj.AddMember("id", id, allocator);
    Value ip;
    ip.SetString(this->ip.c_str(), allocator);
    obj.AddMember("IP", ip, allocator);
    obj.AddMember("Status", this->state, allocator);
    Value name;
    name.SetString(this->name.c_str(), allocator);
    obj.AddMember("Name", name, allocator);

    return obj;
};

string Hue_Bridge::GetJson() {
    lock_guard<mutex>lg(m);
    Document d;
    d.SetObject();
    Document::AllocatorType &allocator = d.GetAllocator();

    Value v;
    v.SetString("Bridge", allocator);
    d.AddMember("item", v, allocator);
    Value id;
    id.SetString(this->id.c_str(), allocator);
    d.AddMember("ID", id, allocator);
    Value ip;
    ip.SetString(this->ip.c_str(), allocator);
    d.AddMember("IP", ip, allocator);
    Value name;
    name.SetString(this->name.c_str(), allocator);
    d.AddMember("Name", name, allocator);
    d.AddMember("Status", this->state, allocator);

    string ret = GetJsonString(d);


    return ret;
};

string Hue_Bridge::link_bridge() {
    lock_guard<mutex>lg(m);
    string ret = "Something went wrong";

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
        string tmp = "https://" + this->ip + "/api";
        string post = "{\"devicetype\":\"My_Home#FV\", \"generateclientkey\":true}";
        cout << post << endl;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_URL, tmp.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            //fprintf(stderr, "curl_easy_perform() failed: %s\n",
            //        curl_easy_strerror(res));
            if (http_code == 403) {
                this->state = 0;
            }
            //cout << "Bridge not found on : "+ this->ipaddress << endl;
            Document document;
            document.Parse(response_string.c_str());
            if (document.IsArray()) {
                for (int i = 0; i < document.Size(); i++) {
                    int bp = 0;
                    bp++;
                }
                // for (const auto& point : document.GetArray()) {
                Value &p = document[0];
                if (p.HasMember("success")) {
                    Value &res = p["success"];
                    string username = res["username"].GetString();
                    string clientkey = res["clientkey"].GetString();
                    MYSQL *ms = Mysql_Connect();
                    if (ms != NULL) {
                        string query = "UPDATE Hue_Bridge SET Username='" + username + "', Clientkey='" + clientkey + "' WHERE Bridge_id ='" + this->id + "'";
                        if (!mysql_query(ms, query.c_str())) {
                            this->username = username;
                            ret = "Link is ok";
                        } else {
                            string ms_err = string(mysql_error(ms));
                            cout << "MYSQL Error " << ms_err << endl;
                        }
                        Mysql_Disconnect(ms);
                    }

                } else if (p.HasMember("error")) {
                    rapidjson::Value& err = p["error"];
                    if (err.HasMember("description")) {
                        ret = err["description"].GetString();
                    }
                }

                //}
            }


        };
    }

    curl_easy_cleanup(curl);

    return ret;
};

void Hue_Bridge::do_work() {
    lock_guard<mutex>lg(m);
    try{
        this->handle_incoming(); 
    }catch(exception &e){
         WriteError("Bridge Handle Incoming", e.what());
    }
    try{
        this->handle_streamevents(); 
    }catch(exception &e){
         WriteError("Bridge Handle StreamEvents", e.what());
    }
    try {
        if (this->state == 1) {
            //     cout << "Bridge Request Discover " << endl;
            this->refmaster->hue_bridges->request_discover();
            //   cout << "After Bridge Request Discover " << endl;
        }
    } catch (exception &e) {
        WriteError("Bridge Do Work", e.what());
    }


    //   cout << "Bridge Unlock " << endl;
    for (int i = 0; i < this->devices.size(); i++) {
        //      cout << "Device " << this->devices[i]->GetID() <<" Work " << endl;
        this->devices[i]->do_work();
    }
    //cout << "End Bridge Loop" << endl;
};

Hue_Device * Hue_Bridge::get_device(string id) {

    for (int i = 0; i<this->devices.size(); i++) {
        if (this->devices[i]->GetID() == id) {
            return this->devices[i];
        }
    }
    return NULL;
}

void Hue_Bridge::add_send_buffer(Hue_Send_Buffer * buf) {
    //  cout << "Add Send Buffer " << endl;
    std::lock_guard<mutex> lg(m);
    bool nieuw = true;
    for (int i = 0; i < this->send_buffer.size(); i++) {
        if ((this->send_buffer[i]->url == buf->url) && (this->send_buffer[i]->type == buf->type)) {
            this->send_buffer[i]->body = buf->body;
            nieuw = false;
            break;
        }
    }
    if (nieuw) {
        this->send_buffer.push_back(buf);
    } else {
        delete buf;
    }

};

void Hue_Bridge::add_send_buffer_NO_LOCK(Hue_Send_Buffer * buf) {
    //  cout << "Add Send Buffer " << endl;

    bool nieuw = true;
    for (int i = 0; i < this->send_buffer.size(); i++) {
        if ((this->send_buffer[i]->url == buf->url) && (this->send_buffer[i]->type == buf->type)) {
            this->send_buffer[i]->body = buf->body;
            nieuw = false;
            break;
        }
    }
    if (nieuw) {
        this->send_buffer.push_back(buf);
    } else {
        delete buf;
    }

    //cout << "End Add Send Buffer " << endl;
};

void Hue_Bridge::add_EventStreamBuffer(string msg) {
    std::lock_guard<mutex> lg(m);
    this->EventStreamBuffer.push_back(msg);

}

int Hue_Bridge::get_state() {
    std::lock_guard<mutex> lg(m);
    return this->state;
}

string Hue_Bridge::get_ip() {
    std::lock_guard<mutex> lg(m);
    return this->ip;
};

string Hue_Bridge::get_username() {
    std::lock_guard<mutex> lg(m);
    return this->username;
}
