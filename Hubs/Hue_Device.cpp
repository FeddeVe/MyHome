/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Hue_Device.cpp
 * Author: Fedde
 * 
 * Created on 13 februari 2022, 9:18
 */

#include "Hue_Device.h"

Hue_Device::Hue_Device(string id, Hue_Bridge *host_bridge, Master *refmaster) {
    this->id = id;
    this->host_bridge = host_bridge;
    this->refmaster = refmaster;
    this->last_update = 0;
    this->request_devices = false;
    this->con_state = "Unknown state";
    this->online = false;
}

Hue_Device::Hue_Device(const Hue_Device& orig) {
}

Hue_Device::~Hue_Device() {
    for(int i = 0; i < this->services.size(); i++){
        delete this->services[i];
    }
    this->services.clear();
}

void Hue_Device::do_work() {
     std::lock_guard<mutex> lg(m);
    try{
    time_t nu = time(0);
    for (int i = 0; i < this->services.size(); i++) {
        time_t diff = nu - this->services[i]->last_update;
        if (diff > (5 * 60)) {
            this->services[i]->last_update = time(0);
            //request service update total;
            if (this->services[i]->service_type == SERVICE_TYPE::device_power) {
                Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/device_power", "", BUF_TYPE::GET);
                this->host_bridge->add_send_buffer_NO_LOCK(buf);
            } else if (this->services[i]->service_type == SERVICE_TYPE::light) {
                Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/light", "", BUF_TYPE::GET);
                this->host_bridge->add_send_buffer_NO_LOCK(buf);
            } else if (this->services[i]->service_type == SERVICE_TYPE::light_level) {
                Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/light_level", "", BUF_TYPE::GET);
                this->host_bridge->add_send_buffer_NO_LOCK(buf);
            } else if (this->services[i]->service_type == SERVICE_TYPE::motion) {
                Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/motion", "", BUF_TYPE::GET);
                this->host_bridge->add_send_buffer_NO_LOCK(buf);
            } else if (this->services[i]->service_type == SERVICE_TYPE::temperature) {
                Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/temperature", "", BUF_TYPE::GET);
                this->host_bridge->add_send_buffer_NO_LOCK(buf);
            } else if (this->services[i]->service_type == SERVICE_TYPE::zigbee_connectivity) {
                Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/zigbee_connectivity", "", BUF_TYPE::GET);
                this->host_bridge->add_send_buffer_NO_LOCK(buf);
            } else if (this->services[i]->service_type == SERVICE_TYPE::button) {
                Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/button", "", BUF_TYPE::GET);
                this->host_bridge->add_send_buffer_NO_LOCK(buf);
            }
        }


    }
    if (this->request_devices) {
        Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/device", "", BUF_TYPE::GET);
        this->host_bridge->add_send_buffer_NO_LOCK(buf);
        this->request_devices = false;
    }
    }catch(exception &e){
        WriteError("Hue Device: " + this->name, e.what());
    }
};

string Hue_Device::GetID() {
     std::lock_guard<mutex> lg(m);
    return this->id;
}

void Hue_Device::set_metadata(Value &data) {
     std::lock_guard<mutex> lg(m);
    if (data.HasMember("archetype")) {
        this->archetype = data["archetype"].GetString();
        for (int i = 0; i < this->services.size(); i++) {
            if (this->services[i]->service_type == SERVICE_TYPE::light) {
                Light *l = this->refmaster->lights->getlight(this->services[i]->service_id);
                if (l != NULL) {
                    l->set_archetype(this->archetype);
                }
            }
        }
    }
    if (data.HasMember("name")) {
        this->name = data["name"].GetString();
        for (int i = 0; i < this->services.size(); i++) {
            if (this->services[i]->service_type == SERVICE_TYPE::light) {
                Light *l = this->refmaster->lights->getlight(this->services[i]->service_id);
                if (l != NULL) {
                    l->set_name(this->name);
                }
            } else if ((this->services[i]->service_type == SERVICE_TYPE::motion) || (this->services[i]->service_type == SERVICE_TYPE::light_level) || (this->services[i]->service_type == SERVICE_TYPE::temperature)) {
                Sensor *s = this->refmaster->sensors->get_sensor(this->services[i]->service_id);
                if (s != NULL) {
                    string name = this->name;
                    if (this->services[i]->service_type == SERVICE_TYPE::motion) {
                        name += " Motion sensor";
                    }
                    if (this->services[i]->service_type == SERVICE_TYPE::light_level) {
                        name += " Light sensor";
                    }
                    if (this->services[i]->service_type == SERVICE_TYPE::temperature) {
                        name += " Temperature sensor";
                    }

                    s->set_name(name);
                }
            } else if (this->services[i]->service_type == SERVICE_TYPE::button) {
                Button *b = this->refmaster->buttons->get_button(this->id);
                if (b != NULL) {
                    b->set_name(this->name);
                }
            }
        }
    }
}

void Hue_Device::set_product_data(Value &data) {
     std::lock_guard<mutex> lg(m);
    if (data.HasMember("product_name")) {
        this->productname = data["product_name"].GetString();
        for (int i = 0; i < this->services.size(); i++) {
            int service_type = this->services[i]->service_type;
            string id = this->services[i]->service_id;
            if (service_type == SERVICE_TYPE::light) {
                Light *l = this->refmaster->lights->getlight(id);
                if (l != NULL) {
                    l->set_product_name(this->productname);
                }
            } else if ((service_type == SERVICE_TYPE::motion) || (service_type == SERVICE_TYPE::light_level) || (service_type == SERVICE_TYPE::temperature)) {
                Sensor *s = this->refmaster->sensors->get_sensor(id);
                if (s != NULL) {
                    s->set_productname(this->productname);
                }
            } else if (service_type == SERVICE_TYPE::button) {
                Button *b = this->refmaster->buttons->get_button(this->id);
                if (b != NULL) {
                    b->set_productname(this->productname);
                }
            }
        }
    }
}

void Hue_Device::set_services(Value &data) {
     std::lock_guard<mutex> lg(m);
    if (data.IsArray()) {
        for (int i = 0; i < data.Size(); i++) {
            Value &item = data[i];
            string service_id = item["rid"].GetString();
            string typestr = item["rtype"].GetString();
            bool nieuw = true;
            for (int i = 0; i < this->services.size(); i++) {
                if (this->services[i]->service_id == service_id) {
                    nieuw = false;
                    break;
                }
            }
            if (nieuw) {
                int type = -1;
                if (typestr == "temperature") {
                    type = SERVICE_TYPE::temperature;
                } else if (typestr == "device_power") {
                    type = SERVICE_TYPE::device_power;
                } else if (typestr == "zigbee_connectivity") {
                    type = SERVICE_TYPE::zigbee_connectivity;
                } else if (typestr == "motion") {
                    type = SERVICE_TYPE::motion;
                } else if (typestr == "light_level") {
                    type = SERVICE_TYPE::light_level;
                } else if (typestr == "light") {
                    type = SERVICE_TYPE::light;
                } else if (typestr == "bridge") {
                    type = SERVICE_TYPE::bridge;
                } else if (typestr == "entertainment") {
                    type = SERVICE_TYPE::entertainment;
                } else if (typestr == "button") {
                    type = SERVICE_TYPE::button;
                } else {
                    //cout << "Service Type not supported " << typestr << endl;
                }
                Hue_Service *s = new Hue_Service(type, service_id);
                this->services.push_back(s);
                if (type == SERVICE_TYPE::light) {

                    Light *l = new Light(service_id, this);
                    l->set_archetype(this->archetype);
                    l->set_name(this->name);
                    l->set_con_state(this->con_state, this->online);
                    l->set_product_name(this->productname);

                    this->refmaster->lights->add_light(l);

                }
                if ((type == SERVICE_TYPE::motion) || (type == SERVICE_TYPE::temperature) || (type == SERVICE_TYPE::light_level)) {

                    Sensor *s = new Sensor(service_id, this, false);
                    s->set_batterylevel(this->batterylevel);
                    s->set_constate(this->con_state, this->online);
                    string name = this->name;
                    if (this->services[i]->service_type == SERVICE_TYPE::motion) {
                        name += " Motion sensor";
                    }
                    if (this->services[i]->service_type == SERVICE_TYPE::light_level) {
                        name += " Light sensor";
                    }
                    if (this->services[i]->service_type == SERVICE_TYPE::temperature) {
                        name += " Temperature sensor";
                    }
                    s->set_name(name);
                    s->set_productname(this->productname);

                    this->refmaster->sensors->Add_Sensor(s);
                }
                if (type == SERVICE_TYPE::button) {
                    Button *b = this->refmaster->buttons->get_button(this->id);
                    if (b == NULL) {
                        b = new Button(this->refmaster,this, this->id, this->name, this->productname);
                        this->refmaster->buttons->add_button(b);
                    }
                }
            }
        }
    }

};

void Hue_Device::set_device_power(Value &data) {
     std::lock_guard<mutex> lg(m);
    for (int i = 0; i < this->services.size(); i++) {
        if (this->services[i]->service_type == SERVICE_TYPE::device_power) {
            this->services[i]->last_update = time(0);
        }
    }

    if (data.HasMember("battery_level")) {
        this->batterylevel = data["battery_level"].GetInt();
        for (int i = 0; i< this->services.size(); i++) {
            int service_type = this->services[i]->service_type;
            string id = this->services[i]->service_id;
            if (service_type == SERVICE_TYPE::light) {
                Light *l = this->refmaster->lights->getlight(id);
                if (l != NULL) {
                    l->set_batterylevel(this->batterylevel);
                }
            } else if ((service_type == SERVICE_TYPE::motion) || (service_type == SERVICE_TYPE::light_level) || (service_type == SERVICE_TYPE::temperature)) {
                Sensor *s = this->refmaster->sensors->get_sensor(id);
                if (s != NULL) {
                    s->set_batterylevel(this->batterylevel);
                }
            } else if (service_type == SERVICE_TYPE::button) {
                Button *b = this->refmaster->buttons->get_button(this->id);
                if (b != NULL) {
                    b->set_batterylevel(this->batterylevel);
                }
            }
        }
    }
    if (data.HasMember("battery_state")) {
        this->bat_state = data["battery_state"].GetString();
        for (int i = 0; i< this->services.size(); i++) {
            int service_type = this->services[i]->service_type;
            string id = this->services[i]->service_id;
            if (service_type == SERVICE_TYPE::light) {
                Light *l = this->refmaster->lights->getlight(id);
                if (l != NULL) {
                    l->set_batterystate(this->bat_state);
                }
            } else if ((service_type == SERVICE_TYPE::motion) || (service_type == SERVICE_TYPE::light_level) || (service_type == SERVICE_TYPE::temperature)) {
                Sensor *s = this->refmaster->sensors->get_sensor(id);
                if (s != NULL) {
                    s->set_batterystate(this->bat_state);
                }
            } else if (service_type == SERVICE_TYPE::button) {
                Button *b = this->refmaster->buttons->get_button(this->id);
                if (b != NULL) {
                    b->set_batterystate(this->bat_state);
                }
            }
        }

    }

};

void Hue_Device::set_light(Value &data) {
     std::lock_guard<mutex> lg(m);
    string id = data["id"].GetString();
    for (int i = 0; i < this->services.size(); i++) {
        if (this->services[i]->service_id == id) {
            this->services[i]->last_update = time(0);
        }
    }
    Light *l = this->refmaster->lights->getlight(id);
    if (l != NULL) {
        if (data.HasMember("color")) {
            Value &color = data["color"];
            if (color.HasMember("gamut_type")) {
                l->set_gammut_type(color["gamut_type"].GetString());
            }
            if (color.HasMember("xy")) {
                float x = color["xy"]["x"].GetFloat();
                float y = color["xy"]["y"].GetFloat();
                l->set_act_color(x, y);
            }
        }
        if (data.HasMember("color_temperature")) {
            Value &ct = data["color_temperature"];
            if ((ct.HasMember("mirek")) && (ct.HasMember("mirek_valid"))) {
                int mirek = 0;
                if (ct["mirek"].IsInt()) {
                    mirek = ct["mirek"].GetInt();
                }
                l->set_act_ct(mirek, ct["mirek_valid"].GetBool());
            }
            if (ct.HasMember("mirek_schema")) {
                l->set_ct_min_max(ct["mirek_schema"]["mirek_minimum"].GetInt(), ct["mirek_schema"]["mirek_maximum"].GetInt());
            }
        }
        if (data.HasMember("dimming")) {
            Value &dimming = data["dimming"];
            if (dimming.HasMember("brightness")) {
                l->set_act_bri(dimming["brightness"].GetFloat());
            }
            if (dimming.HasMember("min_dim_level")) {
                l->set_min_dim_level(dimming["min_dim_level"].GetFloat());
            }
        }
        if (data.HasMember("metadata")) {
            Value &metadata = data["metadata"];
            if (metadata.HasMember("archetype")) {
                l->set_archetype(metadata["archetype"].GetString());
            }
            if (metadata.HasMember("name")) {
                l->set_name(metadata["name"].GetString());
            }
        }
        if (data.HasMember("on")) {
            l->set_act_on(data["on"]["on"].GetBool());
        }






    } else {
        //huh new light??strange, get the device list;
        this->request_devices = true;

    }
};

void Hue_Device::set_light_level(Value &data) {
     std::lock_guard<mutex> lg(m);
    string id = data["id"].GetString();

    Sensor *s = this->refmaster->sensors->get_sensor(id);
    if (s != NULL) {
        if (data.HasMember("light")) {
            Value &light = data["light"];
            if (light.HasMember("light_level")) {
                bool isvalid = light["light_level_valid"].GetBool();
                if (isvalid) {
                    s->set_lightlevel(light["light_level"].GetInt(), false);
                    for (int i = 0; i < this->services.size(); i++) {
                        if (this->services[i]->service_id == id) {
                            this->services[i]->last_update = time(0);
                        }
                    }
                }
            }
        }

    } else {
        //huh new sensor??
        this->request_devices = true;
    }
};

void Hue_Device::set_motion(Value &data) {
     std::lock_guard<mutex> lg(m);
    string id = data["id"].GetString();

    Sensor *s = this->refmaster->sensors->get_sensor(id);
    if (s != NULL) {
        if (data.HasMember("motion")) {
            Value &motion = data["motion"];
            if (motion.HasMember("motion")) {
                bool isvalid = motion["motion_valid"].GetBool();
                if (isvalid) {
                    s->set_motion(motion["motion"].GetBool());
                    for (int i = 0; i < this->services.size(); i++) {
                        if (this->services[i]->service_id == id) {
                            this->services[i]->last_update = time(0);
                        }
                    }
                }
            }
        }

    } else {
        //huh new sensor??
        this->request_devices = true;
    }
};

void Hue_Device::set_temperature(Value &data) {
     std::lock_guard<mutex> lg(m);
    string id = data["id"].GetString();
    for (int i = 0; i < this->services.size(); i++) {
        if (this->services[i]->service_id == id) {
            this->services[i]->last_update = time(0);
        }
    }
    Sensor *s = this->refmaster->sensors->get_sensor(id);
    if (s != NULL) {
        if (data.HasMember("temperature")) {
            Value &temperature = data["temperature"];
            if (temperature.HasMember("temperature")) {
                bool isvalid = temperature["temperature_valid"].GetBool();
                if (isvalid) {
                    s->set_temperature(temperature["temperature"].GetFloat());
                    for (int i = 0; i < this->services.size(); i++) {
                        if (this->services[i]->service_id == id) {
                            this->services[i]->last_update = time(0);
                        }
                    }
                }
            }
        }

    } else {
        //huh new sensor??
        this->request_devices = true;
    }
};

void Hue_Device::set_zigbee_connectivity(Value &data) {
     std::lock_guard<mutex> lg(m);
    for (int i = 0; i < this->services.size(); i++) {
        if (this->services[i]->service_type == SERVICE_TYPE::zigbee_connectivity) {
            this->services[i]->last_update = time(0);
        }
    }
    if (data.HasMember("status")) {
        this->con_state = data["status"].GetString();
        bool online = true;
        if (this->con_state != "connected") {
            online = false;
        }
        this->online = online;
        for (int i = 0; i < this->services.size(); i++) {
            int service_type = this->services[i]->service_type;
            string id = this->services[i]->service_id;
            if (service_type == SERVICE_TYPE::light) {
                Light *l = this->refmaster->lights->getlight(id);
                if (l != NULL) {
                    l->set_con_state(this->con_state, online);
                }
            } else if ((service_type == SERVICE_TYPE::motion) || (service_type == SERVICE_TYPE::light_level) || (service_type == SERVICE_TYPE::temperature)) {
                Sensor *s = this->refmaster->sensors->get_sensor(id);
                if (s != NULL) {
                    s->set_constate(this->con_state, online);
                }
            } else if (service_type == SERVICE_TYPE::button) {
                Button *b = this->refmaster->buttons->get_button(this->id);
                if (b != NULL) {
                    b->set_constate(this->con_state, online);
                }
            }
        }
    }
}

void Hue_Device::set_button(Value &data) {
     std::lock_guard<mutex> lg(m);
    string id = data["id"].GetString();
    int ctrlid = 0;
    string last_event = "";
    for (int i = 0; i < this->services.size(); i++) {
        if (this->services[i]->service_id == id) {
            this->services[i]->last_update = time(0);
        }
    }
    if (data.HasMember("metadata")) {
        Value &metadata = data["metadata"];
        ctrlid = metadata["control_id"].GetInt();
    }
    if (data.HasMember("button")) {
        Value &button = data["button"];
        if(button.HasMember("last_event")){
            last_event = button["last_event"].GetString();
        }else{
         int bp = 0;
         bp++;
        }
    }
    Button *b = this->refmaster->buttons->get_button(this->id);
    if (b != NULL) {
        b->Set_Button(id, ctrlid, last_event);
    } else {
        this->request_devices = true;
    }
};

void Hue_Device::put_data(string url, string body) {
    
    Hue_Send_Buffer *buf = new Hue_Send_Buffer(url, body, BUF_TYPE::PUT);
    this->host_bridge->add_send_buffer(buf);
};

void Hue_Device::SetName(string name) {
      std::lock_guard<mutex> lg(m);
    Document d;
    Document::AllocatorType &allocator = d.GetAllocator();
    d.SetObject();
    Value metadata;
    metadata.SetObject();
    Value Vname;
    Vname.SetString(name.c_str(), allocator);
    metadata.AddMember("name", Vname, allocator);
    d.AddMember("metadata", metadata, allocator);
    string body = GetJsonString(d);
    cout << "resource/device/" + this->id << endl;
    cout << body << endl;
    Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/device/" + this->id, body, BUF_TYPE::PUT);
    this->host_bridge->add_send_buffer(buf);

}

void Hue_Device::Ident(){
     std::lock_guard<mutex> lg(m);
    Document d;
    Document::AllocatorType &allocator = d.GetAllocator();
    d.SetObject();
    Value identify;
    identify.SetObject();
    Value Vname;
    Vname.SetString("identify", allocator);
    identify.AddMember("action", Vname, allocator);
    d.AddMember("identify", identify, allocator);
    string body = GetJsonString(d);
    cout << "resource/device/" + this->id << endl;
    cout << body << endl;
    Hue_Send_Buffer *buf = new Hue_Send_Buffer("resource/device/" + this->id, body, BUF_TYPE::PUT);
    this->host_bridge->add_send_buffer(buf);    
};