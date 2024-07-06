/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/*
 * File:   Sensor.cpp
 * Author: Fedde
 *
 * Created on 12 februari 2022, 20:03
 */

#include "Sensor.h"

Sensor::Sensor(string id, Hue_Device *Host_Device, bool tree_mesh) {
    this->host_device = Host_Device;
    this->Tree_Mesh = tree_mesh;
    this->id = id;
    this->have_co2 = false;
    this->have_humidity = false;
    this->have_iaq = false;
    this->have_lightlevel = false;
    this->have_motion = false;
    this->have_temperature = false;
    this->have_battery = false;
    this->have_pressure = false;
    this->have_io = false;
    this->last_motion = 0;
    this->io_state = 255;
    this->temperature = 255;
    this->light_level = 255;
    this->Con_State = "connected";
    this->humidity = 0;
    this->co2 = 0;
    this->iaq = 0;
    this->online = false;
    this->last_update = 0;
}

Sensor::Sensor(const Sensor& orig) {
}

Sensor::~Sensor() {
}

void Sensor::set_motion(bool motion) {
    m.lock();
    this->last_update = time(0);
    this->have_motion = true;

    if (motion) {
        this->last_motion = time(0);
    }
    m.unlock();
};

void Sensor::set_temperature(float temperature) {
    m.lock();
    this->last_update = time(0);
    this->have_temperature = true;
    this->temperature = temperature;
    m.unlock();
}

void Sensor::set_lightlevel(int lightlevel, bool raw_lux) {
    double tmp = 0;
    if (raw_lux == false) {
        tmp = double(lightlevel) / 10000;
        tmp = pow(10, (tmp));
        tmp = tmp - 1;
    } else {
        tmp = lightlevel;
    }
    //pow(10, ($row['lightlevel'] - 1) / 10000
    m.lock();
    this->last_update = time(0);
    this->have_lightlevel = true;
    this->light_level = tmp;
    m.unlock();
}

void Sensor::set_batterylevel(float level) {
    m.lock();

    this->have_battery = true;

    this->battery_level = level;
    m.unlock();
};

void Sensor::set_batterystate(string state) {
    m.lock();
    this->battery_state = state;
    m.unlock();
}

void Sensor::set_constate(string state, bool online) {
    m.lock();
    this->online = online;
    this->Con_State = state;
    m.unlock();
}

void Sensor::set_name(string name) {
    m.lock();

    this->name = name;
    m.unlock();
};

void Sensor::set_productname(string name) {
    m.lock();

    this->productname = name;
    m.unlock();
};

void Sensor::set_io_state(int io_state) {
    m.lock();
    this->have_io = true;
    this->last_update = time(0);
    this->io_state = io_state;
    m.unlock();
}

time_t Sensor::get_last_motion() {
    return this->last_motion;
}

float Sensor::get_temperature() {
    return this->temperature;
}

float Sensor::get_light_level() {
    return this->light_level;
}

float Sensor::get_humidity() {
    return this->humidity;
}

float Sensor::get_co2() {
    return this->co2;
}

float Sensor::get_iaq() {
    return this->iaq;
}

string Sensor::get_id() {
    return this->id;
}

bool Sensor::get_online() {
    return this->online;
}

int Sensor::get_io_state(){
    return this->io_state;
}

bool Sensor::has_motion() {
    return this->have_motion;
}

bool Sensor::has_temperature() {
    return this->have_temperature;
}

bool Sensor::has_lightlevel() {
    return this->have_lightlevel;
}

bool Sensor::has_humidity() {
    return this->have_humidity;
}

bool Sensor::has_co2() {
    return this->have_co2;
}

bool Sensor::has_iaq() {
    return this->have_iaq;
}

bool Sensor::has_io(){
    return this->have_io;
}

void Sensor::do_work() {
    if (this->Tree_Mesh) {
        int diff = time(0) - this->last_update;
        if (diff < 120) {
            this->online = true;
        } else {
            this->online = false;
        }
    }

};

Value Sensor::get_json_value(Document::AllocatorType &allocator) {
    Value v;
    v.SetObject();
    m.lock();
    Value name;
    name.SetString(this->name.c_str(), allocator);
    v.AddMember("Name", name, allocator);
    Value id;
    id.SetString(this->id.c_str(), allocator);
    v.AddMember("ID", id, allocator);
    Value productname;
    productname.SetString(this->productname.c_str(), allocator);
    v.AddMember("ProductName", productname, allocator);
    v.AddMember("HasMotion", this->have_motion, allocator);
    v.AddMember("HasTemperature", this->have_temperature, allocator);
    v.AddMember("HasLightLevel", this->have_lightlevel, allocator);
    v.AddMember("HasHumidity", this->have_humidity, allocator);
    v.AddMember("HasCo2", this->have_co2, allocator);
    v.AddMember("HasIAQ", this->have_iaq, allocator);
    v.AddMember("HasBattery", this->have_battery, allocator);
    v.AddMember("LastMotion", int64_t(this->last_motion), allocator);
    v.AddMember("Temperature", this->temperature, allocator);
    v.AddMember("LightLevel", this->light_level, allocator);
    v.AddMember("Humidity", this->humidity, allocator);
    v.AddMember("CO2", this->co2, allocator);
    v.AddMember("IAQ", this->iaq, allocator);
    v.AddMember("Online", this->online, allocator);
    v.AddMember("Battery", this->battery_level, allocator);
    Value bat_state;
    bat_state.SetString(this->battery_state.c_str(), allocator);
    v.AddMember("Battery_State", bat_state, allocator);
    Value con_state;
    con_state.SetString(this->Con_State.c_str(), allocator);
    v.AddMember("Con_State", con_state, allocator);
    m.unlock();
    return v;
}

void Sensor::Identify() {
    if (this->host_device != NULL) {
        this->host_device->Ident();
    }
}

void Sensor::set_bme680(float st_iaq, int iaq_accuracy, float co2, float humidity, float temperature, float pressure) {
    m.lock();
    this->last_update = time(0);
    this->iaq = st_iaq;
    this->co2 = co2;
    this->humidity = humidity;
    this->temperature = temperature;
    this->pressure = pressure;

    if ((iaq_accuracy == 3) || (iaq_accuracy == 1)) {
        this->have_iaq = true;
        this->have_co2 = true;

    } else {
        this->have_iaq = false;
        this->have_co2 = false;
    }
    this->have_humidity = true;
    this->have_temperature = true;
    this->have_pressure = true;

    m.unlock();
};