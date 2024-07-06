/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/*
 * File:   Area.cpp
 * Author: Fedde
 *
 * Created on 16 februari 2022, 10:38
 */

#include "Area.h"

Area::Area(Master *refmaster, int msid, string name) {
    this->m = new mutex();
    this->tempmutex = new mutex();
    this->scenemutex = new mutex();
    this->refmaster = refmaster;
    this->msid = msid;
    this->name = name;
    this->motion_off = false;
    this->motion_on = false;
    this->motion_off_time = 0;
    this->manual_off_time = 0;
    this->lux_thr = 255;
    this->icon = "rooms-living";
    this->LUX_FALL_RISE_TIME = 180 * 4;
    this->MOTION_FALL_TIME = 180 * 4;
    this->lux_fall = LUX_FALL_RISE_TIME;
    this->motion_fall = MOTION_FALL_TIME;
    this->manual_brightness = 0.0F;
    this->heat_required = false;
    this->heat_manual = false;
    this->manual_release_at = 0;
    this->last_motion = 0;
    this->last_motion_ms = 0;
    this->Last_Display_Info_Send = this->refmaster->tree_mesh->get_timestamp_ms();
    this->display_resend_block = 0;
    this->area_pid = new Area_PID();
    this->enable_heating = false;
    this->next_temp = 255.0f;
    this->Log = new str_area_log();
    this->Log->last_saved = time(0) + this->msid;
    this->window_open_count = 0;
    this->door_open_count = 0;
    this->load_settings();
    this->LoadSetpoints();
    this->LoadScenes();
}

Area::Area(const Area& orig) {
}

Area::~Area() {
    m->lock();
    for (int i = 0; i < this->surround.size(); i++) {
        delete this->surround[i];
    }
    for (int i = 0; i < this->surround_brightness.size(); i++) {
        delete this->surround_brightness[i];
    }
    m->unlock();
    delete m;
}

void Area::LoadSetpoints() {
    this->tempmutex->lock();
    for (int i = 0; i < this->SetPoints.size(); i++) {
        delete this->SetPoints[i];
    }
    this->SetPoints.clear();
    this->SetPoints.shrink_to_fit();
    this->setpoint = HEATING_MIN_TEMP;
    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "SELECT time, min_temp, max_temp FROM Area_Temps WHERE areaid=" + inttostring(this->msid);
        if (!mysql_query(ms, query.c_str())) {
            MYSQL_RES *result = mysql_store_result(ms);
            if (result != NULL) {
                //Get the number of columns
                int num_rows = mysql_num_rows(result);
                int num_fields = mysql_num_fields(result);

                MYSQL_ROW row; //An array of strings
                while ((row = mysql_fetch_row(result))) {
                    if (num_fields >= 1) {
                        str_setpoint *p = new str_setpoint();
                        p->worktime = atoi(row[0]);
                        p->min_temp = atof(row[1]);
                        p->setpoint = atof(row[2]);
                        this->SetPoints.push_back(p);
                    }
                }
                mysql_free_result(result);

            }
        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
        }
        Mysql_Disconnect(ms);
    }

    if (this->SetPoints.size() == 0) {
        str_setpoint *p = new str_setpoint();
        p->worktime = (6 * 60) + 0;
        // p->hour = 6;
        // p->minute = 0;
        p->setpoint = HEATING_MIN_TEMP;
        p->min_temp = HEATING_MIN_TEMP;
        this->SetPoints.push_back(p);
    }

    sort(this->SetPoints.begin(), this->SetPoints.end(), [ ](const str_setpoint *lhs, const str_setpoint * rhs) {
        return lhs->worktime < rhs->worktime;
    });
    this->tempmutex->unlock();



}

void Area::LoadScenes() {
    this->scenemutex->lock();
    for (int i = 0; i < this->scenes.size(); i++) {
        delete this->scenes[i];
    }
    this->scenes.clear();
    this->scenes.shrink_to_fit();
    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "SELECT id, name FROM Scenes WHERE AreaID=" + inttostring(this->msid);
        if (!mysql_query(ms, query.c_str())) {
            MYSQL_RES *result = mysql_store_result(ms);
            if (result != NULL) {
                //Get the number of columns
                int num_rows = mysql_num_rows(result);
                int num_fields = mysql_num_fields(result);

                MYSQL_ROW row; //An array of strings
                while ((row = mysql_fetch_row(result))) {
                    if (num_fields >= 1) {
                        int id = atoi(row[0]);
                        string name = row[1];
                        Scene *s = new Scene(id, name);
                        s->Load_Scene();
                        this->scenes.push_back(s);
                    }
                }
                mysql_free_result(result);

            }
        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
        }
        Mysql_Disconnect(ms);
    }

    this->scenemutex->unlock();
}

int Area::add_scene(string name) {
    this->scenemutex->lock();
    int ret = -1;
    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "INSERT INTO Scenes(AreaID, Name) VALUES(" + inttostring(this->msid) + ", '" + name + "')";
        if (!mysql_query(ms, query.c_str())) {
            query = "SELECT LAST_INSERT_ID()";
            if (!mysql_query(ms, query.c_str())) {
                MYSQL_RES *result = mysql_store_result(ms);
                if (result != NULL) {
                    //Get the number of columns
                    int num_rows = mysql_num_rows(result);
                    int num_fields = mysql_num_fields(result);

                    MYSQL_ROW row; //An array of strings
                    while ((row = mysql_fetch_row(result))) {
                        if (num_fields >= 1) {
                            int id = atoi(row[0]);
                            ret = id;
                            Scene *s = new Scene(id, name);
                            this->scenes.push_back(s);
                        }
                    }
                }
                mysql_free_result(result);
            }


        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
        }
        Mysql_Disconnect(ms);
    }
    this->scenemutex->unlock();
    return ret;

};

string Area::Get_Scenes() {
    Document d;
    d.SetObject();
    Document::AllocatorType &allocator = d.GetAllocator();
    m->lock();
    Value v;
    v.SetString("Area Scene", allocator);
    Value MyArray(rapidjson::kArrayType);
    for (int i = 0; i < this->scenes.size(); i++) {
        Value obj;
        obj.SetObject();
        Value name;
        name.SetString(this->scenes[i]->Get_Name().c_str(), allocator);
        obj.AddMember("Name", name, allocator);
        obj.AddMember("ID", this->scenes[i]->Get_ID(), allocator);
        obj.AddMember("Active", this->scenes[i]->active, allocator);

        MyArray.PushBack(obj, allocator);
    }
    d.AddMember("Scenes", MyArray, allocator);


    m->unlock();
    string ret = GetJsonString(d);
    return ret;
}

string Area::Get_Scene_Values(int sceneid) {
    for (int i = 0; i < this->scenes.size(); i++) {
        if (this->scenes[i]->Get_ID() == sceneid) {
            return this->scenes[i]->Get_Values();
        }
    }
    return "{\"Name\":\"\",\"Values\":[]}";
}

int Area::Save_Scene_Values(int sceneid, string name, Value &values) {
    if (sceneid == -1) {
        sceneid = this->add_scene(name);
    }
    
    for (int i = 0; i < this->scenes.size(); i++) {
        if (this->scenes[i]->Get_ID() == sceneid) {
            this->scenes[i]->Set_Name(name);
            this->scenes[i]->Save_Scene_Values(values);
        }
    }
    return sceneid;
};

bool Area::Get_Scene_Active(int sceneid) {
    for (int i = 0; i < this->scenes.size(); i++) {
        if (this->scenes[i]->Get_ID() == sceneid) {
            return this->scenes[i]->active;
        }
    }
    return false;
}

int Area::Toggle_Scene(int sceneid, uint32_t fadetime_ms, uint32_t releasetime_ms) {
    for (int i = 0; i < this->scenes.size(); i++) {
        if (this->scenes[i]->Get_ID() == sceneid) {
            Scene *s = this->scenes[i];
            if (s->active == true) {
                //disable scene
                s->active = false;
                for (int y = 0; y < s->Values.size(); y++) {
                    
                    Light *l = this->refmaster->lights->getlight(s->Values[y]->lightid);
                    if (l != NULL) {
                        l->remove_scene(sceneid);
                    }
                    
                }
                // this->manual_release_at = time(0) +this->motion_off_time;
                return 0;
            } else {
                s->active = true;
                for (int z = 0; z < s->Values.size(); z++) {
                    if(s->Values[z]->inuse){
                    Light *l = this->refmaster->lights->getlight(s->Values[z]->lightid);
                    if (l != NULL) {
                        int x = s->Values[z]->x;
                        int y = s->Values[z]->y;
                        if (s->Values[z]->usect) {
                            x = s->Values[z]->ct_x;
                            y = s->Values[z]->ct_y;
                        }
                        l->set_scene(s->Get_ID(), fadetime_ms, releasetime_ms, s->Values[z]->brightness, s->Values[z]->ct, x, y);
                    }
                    }
                }
                this->set_brightness(100, fadetime_ms / 1000, (releasetime_ms + fadetime_ms) / 1000, "Toggle Scene", false);
                return 1;
            }



        }
    }
    return -1;
}

void Area::Release_Scene(int sceneid) {
    for (int i = 0; i < this->scenes.size(); i++) {
        if (this->scenes[i]->Get_ID() == sceneid) {
            Scene *s = this->scenes[i];
            if (s->active == true) {
                //disable scene
                s->active = false;
                for (int y = 0; y < s->Values.size(); y++) {
                    Light *l = this->refmaster->lights->getlight(s->Values[y]->lightid);
                    if (l != NULL) {
                        l->remove_scene(sceneid);
                    }
                }

            }




        }
    }
}

void Area::Set_Scene(int sceneid, uint32_t fadetime_ms, uint32_t releasetime_ms) {
    for (int i = 0; i < this->scenes.size(); i++) {
        if (this->scenes[i]->Get_ID() == sceneid) {
            Scene *s = this->scenes[i];
            s->active = true;
            for (int z = 0; z < s->Values.size(); z++) {
                if (s->Values[z]->inuse) {
                    Light *l = this->refmaster->lights->getlight(s->Values[z]->lightid);
                    if (l != NULL) {
                        int x = s->Values[z]->x;
                        int y = s->Values[z]->y;
                        if (s->Values[z]->usect) {
                            x = s->Values[z]->ct_x;
                            y = s->Values[z]->ct_y;
                        }
                        l->set_scene(s->Get_ID(), fadetime_ms, releasetime_ms, s->Values[z]->brightness, s->Values[z]->ct, x, y);
                    }
                }
            }
            this->set_brightness(100, fadetime_ms / 1000, (releasetime_ms + fadetime_ms) / 1000, "Toggle Scene", false);





        }
    }
}

void Area::handle_scenes() {
    for (int i = 0; i < this->scenes.size(); i++) {
        bool active = false;
        for (int y = 0; y < this->lights.size(); y++) {
            Light *l = this->refmaster->lights->getlight(this->lights[y]);
            if (l != NULL) {
                if (l->is_scene_active(this->scenes[i]->Get_ID())) {
                    active = true;
                    break;
                }
            }
        }
        this->scenes[i]->active = active;



        if (this->scenes[i]->active == true) {
            for (int y = 0; y < this->scenes[i]->Values.size(); y++) {
                Light *l = this->refmaster->lights->getlight(this->scenes[i]->Values[y]->lightid);
                if (l != NULL) {
                    l->extent_scene(this->scenes[i]->Get_ID(), this->last_motion_ms);
                }
            }
        }
    }
}

void Area::Set_Temp_Manual(float temp) {
    this->tempmutex->lock();
    this->heat_manual = true;
    this->manual_setpoint = temp;
    this->tempmutex->unlock();
}

void Area::Set_Temp_Auto() {
    this->tempmutex->lock();
    this->heat_manual = false;
    this->tempmutex->unlock();
}

void Area::do_work() {
    m->lock();
    try {
        this->handle_sensors();
    } catch (exception &e) {
        WriteError("Area " + this->name + " -Handle Sensors", e.what());
    }
    try {
        this->handle_light();
    } catch (exception &e) {
        WriteError("Area " + this->name + " -Handle Light", e.what());
    }
    try {
        this->handle_heating();
    } catch (exception &e) {
        WriteError("Area " + this->name + " -Handle Heating", e.what());
    }
    try {
        this->Get_Display_Info();
    } catch (exception &e) {
        WriteError("Area " + this->name + " - Display Report", e.what());
    }
    try {
        this->handle_scenes();
    } catch (exception &e) {
        WriteError("Area " + this->name + " - Handle Scenes", e.what());
    }
    try {
        this->handle_log();
    } catch (exception &e) {
        WriteError("Area " + this->name + " - Handle Log", e.what());
    }
    m->unlock();
};

int Area::get_msid() {
    return this->msid;
}

void Area::load_settings() {
    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "SELECT name,Icon,Motion_On,Motion_Off,Motion_Off_Time,Manual_Off_Time,Lux_THR,Heating_Enabled FROM Area WHERE id=" + inttostring(this->msid);
        if (!mysql_query(ms, query.c_str())) {
            MYSQL_RES *result = mysql_store_result(ms);
            if (result != NULL) {
                //Get the number of columns
                int num_rows = mysql_num_rows(result);
                int num_fields = mysql_num_fields(result);

                MYSQL_ROW row; //An array of strings
                while ((row = mysql_fetch_row(result))) {
                    if (num_fields >= 1) {
                        this->name = row[0];
                        this->icon = row[1];
                        this->motion_on = atoi(row[2]);
                        this->motion_off = atoi(row[3]);
                        this->motion_off_time = atoi(row[4]);
                        this->manual_off_time = atoi(row[5]);
                        this->lux_thr = atoi(row[6]);
                        this->enable_heating = atoi(row[7]);
                    }
                }
            }
            mysql_free_result(result);
        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
        }
        query = "SELECT LightID From Area_Lights WHERE AreaID = " + inttostring(this->msid);
        if (!mysql_query(ms, query.c_str())) {
            MYSQL_RES *result = mysql_store_result(ms);
            if (result != NULL) {
                //Get the number of columns
                int num_rows = mysql_num_rows(result);
                int num_fields = mysql_num_fields(result);

                MYSQL_ROW row; //An array of strings
                while ((row = mysql_fetch_row(result))) {
                    if (num_fields >= 1) {
                        string lightid = row[0];
                        this->lights.push_back(lightid);
                    }
                }
            }
            mysql_free_result(result);
        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
        }
        query = "SELECT SensorID From Area_Sensors WHERE AreaID = " + inttostring(this->msid);
        if (!mysql_query(ms, query.c_str())) {
            MYSQL_RES *result = mysql_store_result(ms);
            if (result != NULL) {
                //Get the number of columns
                int num_rows = mysql_num_rows(result);
                int num_fields = mysql_num_fields(result);

                MYSQL_ROW row; //An array of strings
                while ((row = mysql_fetch_row(result))) {
                    if (num_fields >= 1) {
                        string sensorid = row[0];
                        this->sensors.push_back(sensorid);
                    }
                }
            }
            mysql_free_result(result);
        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
        }
        query = "SELECT SurAreaID, Level From Area_Surround WHERE AreaID = " + inttostring(this->msid);
        if (!mysql_query(ms, query.c_str())) {
            MYSQL_RES *result = mysql_store_result(ms);
            if (result != NULL) {
                //Get the number of columns
                int num_rows = mysql_num_rows(result);
                int num_fields = mysql_num_fields(result);

                MYSQL_ROW row; //An array of strings
                while ((row = mysql_fetch_row(result))) {
                    if (num_fields >= 1) {
                        int areaid = atoi(row[0]);
                        int level = atoi(row[1]);
                        Area_Surrounding *s = new Area_Surrounding();
                        s->area_id = areaid;
                        s->level = level;
                        this->surround.push_back(s);
                    }
                }
            }
            mysql_free_result(result);
        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
        }
        Mysql_Disconnect(ms);
    }

};

string Area::save_settings(Value &data) {
    string ret = "OK";
    m->lock();
    if (data.HasMember("Name")) {
        this->name = data["Name"].GetString();
    }
    if (data.HasMember("Icon")) {
        this->icon = data["Icon"].GetString();
    }
    if (data.HasMember("Motion_On")) {
        this->motion_on = data["Motion_On"].GetBool();
    }
    if (data.HasMember("Motion_Off")) {
        this->motion_off = data["Motion_Off"].GetBool();
    }
    if (data.HasMember("Motion_Off_Time")) {
        this->motion_off_time = data["Motion_Off_Time"].GetInt();
        this->motion_off_time = this->motion_off_time * 60;
    }
    if (data.HasMember("Manual_Off_Time")) {
        this->manual_off_time = data["Manual_Off_Time"].GetInt();
        this->manual_off_time = this->manual_off_time * 60;
    }
    if (data.HasMember("Lux_THR")) {
        this->lux_thr = data["Lux_THR"].GetInt();
    }
    if (data.HasMember("Heating_Enabled")) {
        this->enable_heating = data["Heating_Enabled"].GetBool();
    }
    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "UPDATE Area set name='" + this->name + "',Icon = '" + this->icon + "',Motion_On=" + booltostring(this->motion_on) + ",Motion_Off=" + booltostring(this->motion_off) + ",Motion_Off_Time=" + inttostring(this->motion_off_time) + ",Manual_Off_Time=" + inttostring(this->manual_off_time) + ",Lux_THR=" + inttostring(this->lux_thr) + ", Heating_Enabled = " + booltostring(this->enable_heating) + " WHERE id =" + inttostring(this->msid);
        if (!mysql_query(ms, query.c_str())) {

        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
            ret = ms_err;
        }
        Mysql_Disconnect(ms);
    }
    m->unlock();
    return ret;

};

void Area::handle_sensors() {
    time_t last_motion = 0;
    vector<float> temps;
    vector<float> lux;
    vector<float> humidity;
    vector<float> co2;
    vector<float> iaq;
    for (int i = 0; i < this->sensors.size(); i++) {
        Sensor *s = this->refmaster->sensors->get_sensor(this->sensors[i]);
        if (s != NULL) {
            string name = s->get_id();
            if (s->get_online()) {
                if (s->has_motion()) {
                    if (s->get_last_motion() > last_motion) {
                        last_motion = s->get_last_motion();
                    }
                }
                if (s->has_temperature()) {
                    temps.push_back(s->get_temperature());
                }
                if (s->has_lightlevel()) {
                    lux.push_back(s->get_light_level());
                }
                if (s->has_humidity()) {
                    humidity.push_back(s->get_humidity());
                }
                if (s->has_co2()) {
                    co2.push_back(s->get_co2());
                }
                if (s->has_iaq()) {
                    iaq.push_back(s->get_iaq());
                }
            }
        }
    }
    this->last_motion = last_motion;
    this->last_motion_ms = last_motion;
    this->last_motion_ms = this->last_motion_ms * 1000;
    if (temps.size() > 0) {
        float tmp = 0;
        for (int i = 0; i < temps.size(); i++) {
            tmp = tmp + temps[i];
        }
        this->temperature = tmp / temps.size();
    } else {
        this->temperature = 255.0F;
    }

    if (lux.size() > 0) {
        float tmp = 0;
        for (int i = 0; i < lux.size(); i++) {
            tmp = tmp + lux[i];
        }
        this->lux = tmp / lux.size();
    } else {
        this->lux = 0.0F;
    }

    if (humidity.size() > 0) {
        float tmp = 0;
        for (int i = 0; i < humidity.size(); i++) {
            tmp = tmp + humidity[i];
        }
        this->humidity = tmp / humidity.size();
    } else {
        this->humidity = 0.0F;
    }

    if (co2.size() > 0) {
        float tmp = 0;
        for (int i = 0; i < co2.size(); i++) {
            tmp = tmp + co2[i];
        }
        this->co2 = tmp / co2.size();
    } else {
        this->co2 = 0.0F;
    }

    if (iaq.size() > 0) {
        float tmp = 0;
        for (int i = 0; i < iaq.size(); i++) {
            tmp = tmp + iaq[i];
        }
        this->iaq = tmp / iaq.size();
    } else {
        this->iaq = 0.0F;
    }

    
    //this->window_open = false;
    bool open = false;
    for (int i = 0; i < this->window_sensors.size(); i++) {
        Sensor *s = this->refmaster->sensors->get_sensor(this->window_sensors[i]);
        if (s != NULL) {
            if (s->has_io()) {
                if (s->get_io_state() == 1) {
                    open = true;
                    this->window_open_count++;
                    if(this->window_open_count > WINDOW_OPEN_DELAY_TIME){
                        this->window_open_count = WINDOW_OPEN_DELAY_TIME;
                        this->window_open = true;
                    }
                    break;
                }
            }
        }
    }
    if(open == false){
        if(this->window_open_count > 0){
            this->window_open_count = this->window_open_count - 1;
        }else{
            this->window_open = false;
        }
    }
    //this->door_open = false;
    open = false;
    for (int i = 0; i < this->door_sensors.size(); i++) {
        Sensor *s = this->refmaster->sensors->get_sensor(this->door_sensors[i]);
        if (s != NULL) {
            if (s->has_io()) {
                if (s->get_io_state() == 0) {
                    open = true;
                    this->door_open_count++;
                    if(this->door_open_count > WINDOW_OPEN_DELAY_TIME){
                        this->door_open_count = WINDOW_OPEN_DELAY_TIME;
                        this->door_open = true;
                    }
                    break;
                }
            }
        }
    }
    if(open == false){
        if(this->door_open_count > 0){
            this->door_open_count = this->door_open_count - 1;
        }else{
            this->door_open = false;
        }
    }
};

void Area::handle_light() {
    time_t nu = time(0);
    if (nu > this->manual_release_at) {
        if (this->last_motion + this->motion_off_time > nu) {
            // no fadein
            if (this->motion_on) {
                this->motion_fall = MOTION_FALL_TIME;
            }
        } else {
            // fade out
            if (this->motion_off) {
                if (this->motion_fall > 0) {
                    this->motion_fall = this->motion_fall - 1;
                }
            }
        }
        if (this->lux < this->lux_thr) {
            // fade in
            if (this->lux_fall < LUX_FALL_RISE_TIME) {
                this->lux_fall = this->lux_fall + 1;
            }
        } else {
            if (this->lux_fall > 0) {
                this->lux_fall = this->lux_fall - 1;
            }
        }
        float tmp_lux_fall = float(this->lux_fall) / LUX_FALL_RISE_TIME;
        float tmp_motion_fall = float(this->motion_fall) / MOTION_FALL_TIME;
        float res = (tmp_lux_fall * tmp_motion_fall);
        this->room_brightness = res;
        if (this->lux < this->lux_thr) {
            float tmp_surround = 0;
            for (int i = 0; i < this->surround_brightness.size(); i++) {
                if (this->surround_brightness[i]->brightness > tmp_surround) {
                    tmp_surround = this->surround_brightness[i]->brightness;
                }
            }
            if (tmp_surround > this->room_brightness) {
                this->room_brightness = tmp_surround;
            }
        }
    } else {
        if (this->motion_off) {
            time_t tmp = this->last_motion + this->manual_off_time;
            if (tmp > this->manual_release_at) {
                this->manual_release_at = this->last_motion + this->manual_off_time;
            }
        }

        float tmp_bri = this->room_brightness + this->manual_fade_step;
        if (this->manual_fade_step < 0) {
            if (tmp_bri < this->manual_destination_brightness) {
                tmp_bri = this->manual_destination_brightness;
            }
        }
        if (this->manual_fade_step > 0) {
            if (tmp_bri > this->manual_destination_brightness) {
                tmp_bri = this->manual_destination_brightness;
            }
        }
        this->room_brightness = tmp_bri;
    }


    for (int i = 0; i < this->lights.size(); i++) {
        Light *l = this->refmaster->lights->getlight(this->lights[i]);
        if (l != NULL) {
            l->set_room_brightness(this->room_brightness);
            if (this->motion_off) {

                // l->set_manual_release_time(this->last_motion + this->manual_off_time);
            }
        }
    }

    for (int i = 0; i < this->surround.size(); i++) {
        if (this->surround[i]->area_id != this->msid) {
            Area *a = this->refmaster->areas->get_area(this->surround[i]->area_id);

            if (a != NULL) {
                float pct = float(this->surround[i]->level) / 100;
                pct = this->room_brightness * pct;
                a->set_surround_brightness(this->msid, pct);
            }
        }
    }
};

void Area::handle_log() {
    time_t nu = time(0);
    if (this->Log->last_saved + AREA_LOG_SAVE_INTERVAL < nu) {
        this->Log->brightness = this->room_brightness * 100;
        this->Log->co2 = this->co2;
        this->Log->flame_on = this->heat_required;
        this->Log->humidity = this->humidity;
        this->Log->iaq = this->iaq;
        this->Log->last_saved = nu;
        this->Log->manual_temp_set = this->heat_manual;
        this->Log->motion = 0;
        this->Log->next_temp = this->next_temp;
        this->Log->radiator_on = this->radiator_open;
        this->Log->setpoint = this->setpoint;
        this->Log->temperature = this->temperature;
        this->Log->lux = this->lux;

        MYSQL *ms = Mysql_Connect();
        if (ms != NULL) {
            string query = "INSERT INTO Area_Log(AreaID, Temperature, Next_Temp, SetPoint, Flame_On, Radiator_On, Manual_Temp_Set, IAQ, CO2, Humidity, Motion, Brightness, Lux) VALUES (" + inttostring(this->msid) + "," + floattostring(Log->temperature) + "," + floattostring(Log->next_temp) + "," + floattostring(this->setpoint) + "," + inttostring(Log->flame_on) + "," + inttostring(Log->radiator_on) + "," + inttostring(Log->manual_temp_set) + "," + inttostring(Log->iaq) + "," + floattostring(Log->co2) + "," + floattostring(Log->humidity) + "," + inttostring(Log->motion) + "," + inttostring(Log->brightness) + ", " + floattostring(Log->lux) + ")";
            if (!mysql_query(ms, query.c_str())) {

            } else {
                string ms_err = string(mysql_error(ms));
                cout << "MYSQL Error " << ms_err << endl;
            }
            Mysql_Disconnect(ms);
        }


    }


}

str_setpoint *Area::Get_Setpoint() {
    str_setpoint *ret = new str_setpoint;
    if (this->refmaster->heating_enabled == false) {
        ret->min_temp = HEATING_MIN_TEMP;
        ret->setpoint = HEATING_MIN_TEMP;
        return ret;
    }
    if (this->heat_manual) {
        time_t nu = time(0);
        time_t manual_release = this->last_motion + this->manual_off_time;
        if (nu > manual_release) {
            this->heat_manual = false;
        } else {
            ret->min_temp = HEATING_MIN_TEMP;
            ret->setpoint = manual_setpoint;
            return ret;
        }
    }
    time_t nu = time(0);
    std::tm* now = std::localtime(&nu);
    str_setpoint *setpoint = NULL;
    uint32_t now_int = (now->tm_hour * 60) + now->tm_min;
    for (int i = 0; i < this->SetPoints.size(); i++) {
        //uint32_t setpoint_int = (this->SetPoints[i]->hour *60)+this->SetPoints[i]->minute;
        if (now_int > this->SetPoints[i]->worktime) {
            setpoint = this->SetPoints[i];
        }
    }
    if (setpoint == NULL) {
        setpoint = this->SetPoints[this->SetPoints.size() - 1];
    }
    ret->min_temp = setpoint->min_temp;
    ret->setpoint = setpoint->setpoint;
    return ret;


}

void Area::handle_heating() {
    this->tempmutex->lock();
    /*
    time_t t = std::time(0);
    tm* now = std::localtime(&t);
    int nu = (now->tm_hour * 60 * 60) + (now->tm_min * 60) + now->tm_sec;
    if (this->msid == 12) {
        if ((nu < 25200) || (nu > 68400)) {
            if (this->temperature < 18.0F) {
                // turn on heat
                this->heat_required = true;
            }
            if (this->temperature > 18.5F) {
                // turn heat off
                this->heat_required = false;
            }
        } else {
            this->heat_required = false;
        }

        Light *l = this->refmaster->lights->getlight("f1d106e7-11d2-430f-aab9-0a040b2ff309");
        // Light *l = this->refmaster->lights->getlight("06b36f21-94b0-4d25-a25f-392329779609");
        if (l != NULL) {
            /*
            if (this->heat_required) {
                l->set_manual_brightness(100, time(0) + 60);
            } else {
                l->set_manual_brightness(0, time(0) + 60);
            }
     */
    // }

    //}
    //*/


    // nieuwe code
    time_t nu = time(0);
    this->heat_required = false;
    this->radiator_open = false;
    if (this->enable_heating == true) {

        this->area_pid->get_next_temp(this->temperature, &this->next_temp);
        str_setpoint *point = this->Get_Setpoint();
        this->setpoint = point->min_temp;
        if (next_temp < point->min_temp) {
            this->heat_required = true;
            this->radiator_open = true;
        } else {
            this->setpoint = point->setpoint;
            if (next_temp < point->setpoint) {
                this->radiator_open = true;
                if (this->last_motion + this->motion_off_time > nu) {
                    if ((this->door_open == false) && (this->window_open == false)) {
                        this->heat_required = true;
                    } else {
                        this->heat_required = false;
                    }
                } else {
                    this->heat_required = false;
                }
            } else {
                this->radiator_open = false;
                this->heat_required = false;
            }
        }
        delete point;
    } else {
        this->radiator_open = false;
        this->heat_required = false;
    }
    
     if (this->msid == 12) {
         Light *l = this->refmaster->lights->getlight("f1d106e7-11d2-430f-aab9-0a040b2ff309");
        // Light *l = this->refmaster->lights->getlight("06b36f21-94b0-4d25-a25f-392329779609");
        if (l != NULL) {
            
            if (this->heat_required) {
                l->set_manual(100, 5000);
            } else {
                l->set_manual(0, 5000);
            }
        }
     }
    
    this->tempmutex->unlock();
};

Value Area::GetJsonValue(Document::AllocatorType &allocator) {
    Value obj;
    obj.SetObject();
    m->lock();
    obj.AddMember("ID", this->msid, allocator);
    Value name;
    name.SetString(this->name.c_str(), allocator);
    obj.AddMember("Name", name, allocator);
    obj.AddMember("LastMotion", uint64_t(this->last_motion), allocator);
    obj.AddMember("LightLevel", this->lux, allocator);
    obj.AddMember("Temperature", this->temperature, allocator);
    obj.AddMember("Setpoint", this->setpoint, allocator);
    obj.AddMember("Motion_On", this->motion_on, allocator);
    obj.AddMember("Motion_Off", this->motion_off, allocator);
    obj.AddMember("Motion_Off_Time", this->motion_off_time, allocator);
    obj.AddMember("Manual_Off_Time", this->manual_off_time, allocator);
    obj.AddMember("Lux_THR", this->lux_thr, allocator);
    obj.AddMember("Brightness", this->room_brightness, allocator);
    obj.AddMember("Humidity", this->humidity, allocator);
    obj.AddMember("IAQ", this->iaq, allocator);
    obj.AddMember("Heat_Required", this->heat_required, allocator);
    obj.AddMember("Heat_Manual", this->heat_manual, allocator);
    obj.AddMember("Radiator_State", this->radiator_open, allocator);
    obj.AddMember("DoorState", this->door_open, allocator);
    obj.AddMember("WindowState", this->window_open, allocator);
    obj.AddMember("ch_enable_heating", this->enable_heating, allocator);
    bool man_bright = false;
    if (this->manual_release_at > time(0)) {
        man_bright = true;
    }
    obj.AddMember("Manual_Brightness", man_bright, allocator);
    Value icon;
    icon.SetString(this->icon.c_str(), allocator);
    obj.AddMember("Icon", icon, allocator);
    Value surrounding(rapidjson::kArrayType);
    for (int i = 0; i < this->surround.size(); i++) {
        Value surround;
        surround.SetObject();
        surround.AddMember("AreaID", this->surround[i]->area_id, allocator);
        surround.AddMember("Level", this->surround[i]->level, allocator);
        surrounding.PushBack(surround, allocator);
    }
    obj.AddMember("Surrounding", surrounding, allocator);
    Value LightArray(rapidjson::kArrayType);
    for (int i = 0; i < this->lights.size(); i++) {
        Value light;
        light.SetObject();
        Value id;
        id.SetString(this->lights[i].c_str(), allocator);
        light.AddMember("ID", id, allocator);
        LightArray.PushBack(light, allocator);
    }
    obj.AddMember("Lights", LightArray, allocator);
    m->unlock();
    return obj;
};

string Area::set_surrounding(int areaid, int level) {
    m->lock();
    string ret = "OK";
    bool nieuw = true;
    bool update = false;
    bool del = false;
    for (int i = 0; i < this->surround.size(); i++) {
        if (this->surround[i]->area_id == areaid) {
            nieuw = false;
            if (level == 0) {
                del = true;
                delete this->surround[i];
                this->surround.erase(this->surround.begin() + i);
                break;
            } else {
                if (level > 100) {
                    level = 100;
                }
                update = true;
                this->surround[i]->level = level;

            }
        }
    }

    if (nieuw) {
        Area_Surrounding *s = new Area_Surrounding();
        s->area_id = areaid;
        s->level = level;
        this->surround.push_back(s);
    }

    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "INSERT INTO Area_Surround (AreaID, SurAreaID,Level) VALUES(" + inttostring(this->msid) + ", " + inttostring(areaid) + ", " + inttostring(level) + ")";
        if (del == true) {
            query = "DELETE FROM Area_Surround WHERE AreaID =" + inttostring(this->msid) + " AND SurAreaID =" + inttostring(areaid);
        }
        if (update == true) {
            query = "UPDATE Area_Surround set Level=" + inttostring(level) + " WHERE AreaID=" + inttostring(this->msid) + " AND SurAreaID=" + inttostring(areaid);
        }
        if (!mysql_query(ms, query.c_str())) {

        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
            ret = ms_err;
        }
        Mysql_Disconnect(ms);
    }

    m->unlock();
    return ret;
};

string Area::GetJsonLights() {
    m->lock();
    Document obj;
    obj.SetObject();
    Document::AllocatorType &allocator = obj.GetAllocator();
    obj.AddMember("AreaID", this->msid, allocator);
    Value LightArray(rapidjson::kArrayType);
    for (int i = 0; i < this->lights.size(); i++) {
        Value light;
        light.SetObject();
        Value id;
        id.SetString(this->lights[i].c_str(), allocator);
        light.AddMember("ID", id, allocator);
        LightArray.PushBack(light, allocator);
    }
    obj.AddMember("Lights", LightArray, allocator);

    string ret = GetJsonString(obj);
    m->unlock();
    return ret;

}

string Area::toggle_light(string id) {
    m->lock();
    bool nieuw = true;
    for (int i = 0; i < this->lights.size(); i++) {
        if (this->lights[i] == id) {
            nieuw = false;
            this->lights.erase(this->lights.begin() + i);
            break;
        }
    }
    if (nieuw) {
        this->lights.push_back(id);
    }

    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "INSERT INTO Area_Lights (AreaID, LightID) VALUES(" + inttostring(this->msid) + ", '" + id + "')";
        if (nieuw == false) {
            query = "DELETE FROM Area_Lights WHERE AreaID =" + inttostring(this->msid) + " AND LightID ='" + id + "'";
        }
        if (!mysql_query(ms, query.c_str())) {

        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
        }
        Mysql_Disconnect(ms);
    }

    m->unlock();
    return this->GetJsonLights();
};

string Area::GetJsonSensors() {
    m->lock();
    Document obj;
    obj.SetObject();
    Document::AllocatorType &allocator = obj.GetAllocator();
    obj.AddMember("AreaID", this->msid, allocator);
    Value SensorArray(rapidjson::kArrayType);
    for (int i = 0; i < this->sensors.size(); i++) {
        Value sensor;
        sensor.SetObject();
        Value id;
        id.SetString(this->sensors[i].c_str(), allocator);
        sensor.AddMember("ID", id, allocator);
        SensorArray.PushBack(sensor, allocator);
    }
    obj.AddMember("Sensors", SensorArray, allocator);
    string ret = GetJsonString(obj);
    m->unlock();
    return ret;

}

string Area::toggle_sensor(string id) {
    m->lock();
    bool nieuw = true;
    for (int i = 0; i < this->sensors.size(); i++) {
        if (this->sensors[i] == id) {
            nieuw = false;
            this->sensors.erase(this->sensors.begin() + i);
            break;
        }
    }
    if (nieuw) {
        this->sensors.push_back(id);
    }

    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "INSERT INTO Area_Sensors (AreaID, SensorID) VALUES(" + inttostring(this->msid) + ", '" + id + "')";
        if (nieuw == false) {
            query = "DELETE FROM Area_Sensors WHERE AreaID =" + inttostring(this->msid) + " AND SensorID ='" + id + "'";
        }
        if (!mysql_query(ms, query.c_str())) {

        } else {
            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
        }
        Mysql_Disconnect(ms);
    }

    m->unlock();
    return this->GetJsonSensors();
};

void Area::set_surround_brightness(int area_id, float surround_brightness) {

    // m.lock();
    bool nieuw = true;
    for (int i = 0; i < this->surround_brightness.size(); i++) {
        if (this->surround_brightness[i]->area_id == area_id) {
            nieuw = false;
            this->surround_brightness[i]->brightness = surround_brightness;
            break;
        }
    }
    if (nieuw) {
        Area_Surround_Brightness *br = new Area_Surround_Brightness();
        br->area_id = area_id;
        br->brightness = surround_brightness;
        this->surround_brightness.push_back(br);
    }


    //  m.unlock();

};

void Area::set_brightness(int des_bri, int fade_time, time_t release_time, string from, bool clear_scenes) {

    m->lock();
    if (clear_scenes) {
        for (int i = 0; i < this->lights.size(); i++) {
            Light *l = this->refmaster->lights->getlight(this->lights[i]);
            if (l != NULL) {
                cout << "Area Brightness release all manual and scenens hier " << endl;
                for (int y = 0; y < this->scenes.size(); y++) {
                    l->remove_scene(this->scenes[y]->Get_ID());

                }
            }

            for (int y = 0; y < this->scenes.size(); y++) {
                this->scenes[y]->active = false;
            }

            this->refmaster->macros->disable_macro(this->msid);
        }
    }
    if (release_time == 0) {
        release_time = time(0) + this->manual_off_time;
    } else {
        release_time = time(0) + release_time;
    }
    float tmp = float(des_bri) / 100;
    this->manual_destination_brightness = tmp;
    WriteError("AreaButton - " + this->name, "Set by " + from);
    this->manual_release_at = release_time;
    float diff = tmp - this->room_brightness;

    if (fade_time > 0) {
        fade_time = fade_time * 4; //4 times per second;
        this->manual_fade_step = diff / fade_time;
    } else {
        this->manual_fade_step = diff;
    }

    int bp = 0;
    bp++;

    m->unlock();

}

void Area::set_button_color(int ct, int bri, time_t release_time, string from) {
    /*
   m->lock();
     if (release_time == 0) {
       release_time = time(0) + this->manual_off_time;
   } else {
       release_time = time(0) + release_time;
   }
    
    this->manual_release_at = release_time;
    float tmp = 1.0;
    this->manual_destination_brightness = tmp;
   float diff = tmp - this->room_brightness; 
    
       this->manual_fade_step = diff;
    
    
     WriteError("AreaButtonColor - "+ this->name, "Set by "+ from);
   for (int i = 0; i < this->lights.size(); i++) {
       Light *l = this->refmaster->lights->getlight(this->lights[i]);
       if (l != NULL) {
           if (release_time == 0) {
               release_time = time(0) + this->manual_off_time;
           }
           l->set_manual_brightness(bri, release_time);
           l->set_manual_ct(ct, release_time);
       }
   }
   m->unlock();
     */
};

void Area::Get_Display_Info() {
    bool do_send = false;
    //Backlight
    this->area_info.areaid = this->msid;
    time_t nu = time(0);
    uint8_t tmp_backlight = 0;
    if (nu < this->manual_release_at) {
        //manual is set
        tmp_backlight = this->Get_Backlight();
    } else {
        if (this->lux_fall == this->LUX_FALL_RISE_TIME) {
            tmp_backlight = this->Get_Backlight();

        } else {
            if (this->last_motion + this->motion_off_time > nu) {
                tmp_backlight = 100;
            }
        }
    }


    // cout << "Area - " << this->msid << " Backlight = "<< (int)tmp_backlight << endl;
    if (this->area_info.backlight != tmp_backlight) {
        do_send = true;
        this->area_info.backlight = tmp_backlight;
    }
    this->area_info.iaq = this->iaq;
    string tmp = inttostring(this->iaq);
    memcpy(area_info.iaq_str, tmp.c_str(), tmp.size() + 1);
    // this->area_info->iaq_str = inttostring(this->iaq);
    this->area_info.temperature = this->temperature;
    tmp = floattostring(roundf(this->temperature * 10) / 10) + "°";
    memcpy(area_info.temperature_str, tmp.c_str(), tmp.size() + 1);
    this->area_info.temp_set_point = this->setpoint;
    tmp = inttostring(area_info.backlight);
    memcpy(area_info.temp_set_point_str, tmp.c_str(), tmp.size() + 1);
    area_info.humidity = this->humidity;
    tmp = floattostring(roundf(this->humidity * 10) / 10) + "%";
    memcpy(area_info.humidity_str, tmp.c_str(), tmp.size() + 1);
    area_info.co2 = this->co2;
    tmp = floattostring(roundf(this->co2)) + "ppm";
    memcpy(area_info.co2_str, tmp.c_str(), tmp.size() + 1);
    area_info.flame_state = this->heat_required;

    area_info.radiator_state = this->radiator_open;
    area_info.window_state = this->window_open;
    area_info.door_state = this->door_open;

    uint64_t diff = this->refmaster->tree_mesh->get_timestamp_ms() - this->Last_Display_Info_Send;
    if (diff > this->refmaster->tree_mesh->mesh_info_interval) {
        this->Last_Display_Info_Send = this->refmaster->tree_mesh->get_timestamp_ms();
        do_send = true;
    }
    if (this->display_resend_block != 0) {
        this->display_resend_block = this->display_resend_block - 1;
        do_send = false;
    }

    if (do_send) {

        char data[sizeof (this->area_info)];
        memcpy(data, &this->area_info, sizeof (this->area_info));
        // Tree_Node *from, uint32_t to, uint16_t msg_type, uint32_t msgid, uint32_t msg_size, char *data
        Tree_Message *m = new Tree_Message(NULL, BROADCAST_ADDR, msg_types::Area_Report, 0, sizeof (this->area_info), data);

        this->refmaster->tree_mesh->send_message(m);
        this->display_resend_block = 4;
    }





}

uint8_t Area::Get_Backlight() {
    uint8_t ret = 0;
    uint64_t total = 0;
    uint32_t count = 0;
    for (int i = 0; i < this->lights.size(); i++) {
        Light *l = this->refmaster->lights->getlight(this->lights[i]);
        if (l != NULL) {
            //if(l->des_state->raw_brightness > 0){
            //    total = total + l->des_state->raw_brightness;
            //    if(this->msid == 1){
            //        int bp =0;
            //        bp++;
            //    }
            //    count++;
            //}
            if (l->des_state->raw_brightness > ret) {
                ret = l->des_state->raw_brightness;
            }
        }
    }
    // if(count > 0){
    //     total = total / count;
    // }
    // ret = total;

    return ret;

}

