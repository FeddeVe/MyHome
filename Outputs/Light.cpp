/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Light.cpp
 * Author: Fedde
 * 
 * Created on 11 februari 2022, 20:30
 */

#include "Light.h"

Light::Light(string id, Hue_Device *Host_Device) {
  //  this->set_scene(1, 100 * 1000, 600 * 1000, 100, 30, 400, 300);

    this->room_brightness = 0.0F;
    this->id = id;
    this->host_device = Host_Device;
    this->des_state = new LightState();
    this->cur_state = new LightState();
    // this->prev_state = new LightState();
    this->background_state = new LightState();
    this->manual_state = new LightState();
    this->manual_state->releasetime_ms = 0;
    this->manual_state->transistion = 1000;

    this->background_state->transistion = 2000;
    //    this->prev_state->releaseat = 0;
    //    this->prev_state->transistion = 0;
    this->cur_state->on = true;
    this->area_id = -1;
    this->ct_min = 0;
    this->ct_max = 500;
    this->min_dim_level = 0;
    this->con_state = "connected";
    this->have_color = false;
    this->have_ct = false;
    this->have_dim = false;
    this->block = 0;
    this->background_set = true;
    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "Select areaid FROM Lights_Area WHERE lightid = '" + this->id + "'";
        if (!mysql_query(ms, query.c_str())) {
            MYSQL_RES *result = mysql_store_result(ms);
            if (result != NULL) {
                //Get the number of columns
                int num_rows = mysql_num_rows(result);
                int num_fields = mysql_num_fields(result);

                MYSQL_ROW row; //An array of strings
                while ((row = mysql_fetch_row(result))) {
                    if (num_fields >= 1) {
                        this->area_id = atoi(row[0]);
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
}

Light::Light(const Light& orig) {
}

Light::~Light() {
    delete this->des_state;
    delete this->cur_state;
    delete this->manual_state;
    delete this->background_state;
    //    delete this->prev_state;
};

void Light::do_work() {
    //code to run 4times a second   
    m.lock();
    try {
        if (this->background_set) {

            if (this->block == 0) {
                this->set_des_state();

                if (this->host_device != NULL) {
                    int tmp_block = 0;
                    bool do_send = false;
                    Document d;
                    Document::AllocatorType &allocator = d.GetAllocator();
                    d.SetObject();
                    if (this->have_dim == false) {
                        if (this->des_state->brightness > 0) {
                            this->des_state->on = true;
                        } else {
                            this->des_state->on = false;
                        }
                        if (this->des_state->on != this->cur_state->on) {
                            do_send = true;
                            Value on;
                            on.SetObject();
                            on.AddMember("on", this->des_state->on, allocator);
                            d.AddMember("on", on, allocator);
                        }
                    } else {

                        if (this->des_state->on != this->cur_state->on) {
                            do_send = true;
                            Value on;
                            on.SetObject();
                            on.AddMember("on", this->des_state->on, allocator);
                            d.AddMember("on", on, allocator);
                            if (this->des_state->on == false) {
                                do_send = true;
                                Value dimming;
                                dimming.SetObject();
                                dimming.AddMember("brightness", 0, allocator);
                                d.AddMember("dimming", dimming, allocator);
                            }
                        }
                        if (this->des_state->on) {
                            int diff = this->cur_state->brightness - this->des_state->brightness;
                            if (diff < 0) {
                                diff = diff * -1;
                            }
                            if (diff > 100) {
                                do_send = true;
                                Value dimming;
                                dimming.SetObject();
                                dimming.AddMember("brightness", this->des_state->raw_brightness, allocator);
                                d.AddMember("dimming", dimming, allocator);
                            }
                            if (this->have_color) {
                                // use xy;
                                if ((this->cur_state->x != this->des_state->x) || (this->des_state->y != this->cur_state->y)) {
                                    do_send = true;
                                    Value color;
                                    color.SetObject();
                                    Value xy;
                                    xy.SetObject();
                                    xy.AddMember("x", this->des_state->raw_x, allocator);
                                    xy.AddMember("y", this->des_state->raw_y, allocator);
                                    color.AddMember("xy", xy, allocator);
                                    d.AddMember("color", color, allocator);
                                }

                            } else {
                                // use ct;
                                if (this->des_state->colortemp < this->ct_min) {
                                    this->des_state->colortemp = this->ct_min;
                                }
                                if (this->des_state->colortemp > this->ct_max) {
                                    this->des_state->colortemp = this->ct_max;
                                }
                                if (this->cur_state->colortemp != this->des_state->colortemp) {
                                    do_send = true;
                                    Value color_temperature;
                                    color_temperature.SetObject();
                                    color_temperature.AddMember("mirek", this->des_state->colortemp, allocator);
                                    d.AddMember("color_temperature", color_temperature, allocator);
                                }
                            }
                        }


                    } // if on
                    Value dynamics;
                    dynamics.SetObject();
                    dynamics.AddMember("duration", this->des_state->transistion, allocator);
                    d.AddMember("dynamics", dynamics, allocator);

                    tmp_block = this->des_state->transistion / 250;
                    tmp_block = tmp_block + 1;

                    if (do_send) {
                        // cout << "DOSEND" << endl;
                        string data = GetJsonString(d);
                        this->host_device->put_data("resource/light/" + this->id, data);
                        this->block = tmp_block;
                    }
                }
            } else {
                this->block = this->block - 1;
            }
        }
    } catch (exception &e) {
        WriteError("Light " + this->name, e.what());
    }
    m.unlock();

}

void Light::set_des_state() {
    uint64_t nu = timeSinceEpochMillisec();
    if (this->manual_state->releasetime_ms > nu) {
        this->des_state->brightness = this->manual_state->brightness;
        this->des_state->raw_brightness = float(this->manual_state->brightness) / 100;
        this->des_state->colortemp = this->manual_state->colortemp;
        this->des_state->x = this->manual_state->x;
        this->des_state->y = this->manual_state->y;
        this->des_state->raw_x = float(this->manual_state->x) / 1000;
        this->des_state->raw_y = float(this->manual_state->y) / 1000;
        this->des_state->transistion = this->manual_state->transistion;
        // cout << this->name << " X=" << this->prev_state->x << " // Y="<<this->prev_state->y << endl;
    } else {
        this->des_state->brightness = this->background_state->brightness;
       // this->des_state->raw_brightness = float(this->background_state->brightness) / 100;
        this->des_state->colortemp = this->background_state->colortemp;
        this->des_state->x = this->background_state->x;
        this->des_state->y = this->background_state->y;
        this->des_state->raw_x = float(this->background_state->x) / 1000;
        this->des_state->raw_y = float(this->background_state->y) / 1000;
        this->des_state->transistion = this->background_state->transistion;

        for (int i = 0; i<this->scene_states.size(); i++) {
            
            if (this->scene_states[i]->releasetime_ms + this->scene_states[i]->fadetime_ms < nu) {
                delete this->scene_states[i];
                this->scene_states.erase(this->scene_states.begin() + i);
                i = i - 1;
            } else {
                LightState *s = this->scene_states[i];
                double percent = 0.0F;
                uint64_t diff = nu - s->starttime_ms;
                if (diff > s->fadetime_ms) {
                    if (nu < s->releasetime_ms) {
                        percent = 1.0F;
                    } else {
                        diff = nu - s->releasetime_ms;
                        percent = double(diff) / double(s->fadetime_ms);
                        percent = 1.0F - percent;
                        
                    }
                } else {
                    percent = double(diff) / double(s->fadetime_ms);
                }
                double des_percent = 1.0F - percent;
                this->des_state->brightness = (this->des_state->brightness * des_percent) + (s->brightness * percent);
               // this->des_state->raw_brightness = float(this->des_state->brightness) / 100;
                this->des_state->colortemp = (this->des_state->colortemp * des_percent) + (s->colortemp * percent);
                this->des_state->x = (this->des_state->x * des_percent) + (s->x * percent);
                this->des_state->y = (this->des_state->y * des_percent) + (s->y * percent);
                this->des_state->raw_x = float(this->des_state->x) / 1000;
                this->des_state->raw_y = float(this->des_state->y) / 1000;
            }//else delete vector item (timerelease);
        }
        this->des_state->brightness = this->des_state->brightness * this->room_brightness;
        this->des_state->raw_brightness = this->des_state->brightness / 100;
    }


   
    
    if (this->des_state->raw_brightness < this->min_dim_level) {
        this->des_state->on = false;
    } else {

        this->des_state->on = true;
    }
    this->calc_rgb();
};

void Light::set_name(string name) {

    m.lock();
    this->name = name;
    m.unlock();
}

void Light::set_archetype(string archetype) {

    m.lock();
    this->archetype = archetype;
    m.unlock();
}

void Light::set_area_id(int area_id) {
    m.lock();
    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        string query = "INSERT INTO Lights_Area (lightid, areaid) VALUES('" + this->id + "', " + inttostring(area_id) + ")  ON DUPLICATE KEY UPDATE areaid = " + inttostring(area_id);
        if (!mysql_query(ms, query.c_str())) {
        } else {

            string ms_err = string(mysql_error(ms));
            cout << "MYSQL Error " << ms_err << endl;
        }
        Mysql_Disconnect(ms);
    }
    m.unlock();
};

void Light::set_product_name(string name) {

    m.lock();
    this->product_name = name;
    m.unlock();
}

void Light::set_batterylevel(float level) {

    m.lock();
    this->battery_level = level;

    m.unlock();
}

void Light::set_batterystate(string state) {

    m.lock();
    this->battery_state = state;
    m.unlock();
}

void Light::set_act_on(bool on) {
    m.lock();
    this->cur_state->on = on;
    if (on == false) {

        this->cur_state->brightness = 0;
        this->cur_state->raw_brightness = 0;
    }
    m.unlock();
}

void Light::set_act_bri(float brightness) {
    m.lock();
    this->have_dim = true;
    if (this->cur_state->on) {
        this->cur_state->raw_brightness = brightness;
        this->cur_state->brightness = brightness * 100;
    } else {
        this->cur_state->brightness = 0;
        this->cur_state->raw_brightness = 0;
    }


    m.unlock();
}

void Light::set_act_ct(int ct, bool ct_valid) {

    m.lock();
    this->have_ct = true;
    this->cur_state->colortemp = ct;
    this->cur_state->ct_active = ct_valid;
    m.unlock();
}

void Light::set_act_color(float x, float y) {

    m.lock();
    this->have_color = true;
    this->cur_state->raw_x = x;
    this->cur_state->x = x * 1000;
    this->cur_state->raw_y = y;
    this->cur_state->y = y * 1000;
    m.unlock();
}

void Light::set_ct_min_max(int min, int max) {

    m.lock();
    this->ct_max = max;
    this->ct_min = min;
    m.unlock();
}

void Light::set_gammut_type(string type) {

    m.lock();
    this->gammut_type = type;
    m.unlock();
}

void Light::set_min_dim_level(float level) {

    m.lock();
    this->min_dim_level = level;
    m.unlock();
}

void Light::set_con_state(string con_state, bool online) {

    m.lock();
    this->online = online;
    this->con_state = con_state;
    m.unlock();
}

string Light::get_id() {

    return this->id;
}

Value Light::get_json_value(Document::AllocatorType &allocator) {
    Value v;
    v.SetObject();
    m.lock();
    Value name;
    name.SetString(this->name.c_str(), allocator);
    v.AddMember("Name", name, allocator);
    Value id;
    id.SetString(this->id.c_str(), allocator);
    v.AddMember("ID", id, allocator);
    Value archetype;
    archetype.SetString(this->archetype.c_str(), allocator);
    v.AddMember("Archetype", archetype, allocator);
    Value productname;
    productname.SetString(this->product_name.c_str(), allocator);
    v.AddMember("ProductName", productname, allocator);
    v.AddMember("HasColor", this->have_color, allocator);
    v.AddMember("HasCT", this->have_ct, allocator);
    v.AddMember("HasDim", this->have_dim, allocator);
    v.AddMember("AreaID", this->area_id, allocator);
    Value con_state;
    con_state.SetString(this->con_state.c_str(), allocator);
    v.AddMember("Con_State", con_state, allocator);
    Value cur_state;
    cur_state.SetObject();
    cur_state.AddMember("Brightness", this->cur_state->raw_brightness, allocator);
    cur_state.AddMember("CT", this->cur_state->colortemp, allocator);
    cur_state.AddMember("CT_Active", this->cur_state->ct_active, allocator);
    cur_state.AddMember("ON", this->cur_state->on, allocator);
    cur_state.AddMember("X", this->cur_state->raw_x, allocator);
    cur_state.AddMember("Y", this->cur_state->raw_y, allocator);
    v.AddMember("CurState", cur_state, allocator);
    m.unlock();

    return v;
}

string Light::GetJson() {

    return "TEST";
};

void Light::set_light() {
    // magic here   

}


void Light::set_preview(Value &data) {
    m.lock();
   // LightState *prevState = new LightState();
    if (data.HasMember("intensity")) {
        this->manual_state->raw_brightness = data["intensity"].GetInt();
        this->manual_state->brightness = this->manual_state->raw_brightness * 100;
    }
    if (data.HasMember("ct")) {
        this->manual_state->colortemp = 1000000 / data["ct"].GetInt();
    }
    if (data.HasMember("use_ct")) {
        this->manual_state->ct_active = data["use_ct"].GetBool();
    }
    if ((data.HasMember("r")) && (data.HasMember("g")) && (data.HasMember("b"))) {
        int r = data["r"].GetInt();
        int g = data["g"].GetInt();
        int b = data["b"].GetInt();

        float red = float(r) / 255;
        float green = float(g) / 255;
        float blue = float(b) / 255;

        //gamma correction
        red = (red > 0.04045f) ? pow((red + 0.055f) / (1.0f + 0.055f), 2.4f) : (red / 12.92f);
        green = (green > 0.04045f) ? pow((green + 0.055f) / (1.0f + 0.055f), 2.4f) : (green / 12.92f);
        blue = (blue > 0.04045f) ? pow((blue + 0.055f) / (1.0f + 0.055f), 2.4f) : (blue / 12.92f);

        //to xyz
        float X = red * 0.664511f + green * 0.154324f + blue * 0.162028f;
        float Y = red * 0.283881f + green * 0.668433f + blue * 0.047685f;
        float Z = red * 0.000088f + green * 0.072310f + blue * 0.986039f;

        // to xy;
        float x = X / (X + Y + Z);
        float y = Y / (X + Y + Z);

        this->manual_state->raw_x = x;
        this->manual_state->raw_y = y;
        this->manual_state->x = x * 1000;
        this->manual_state->y = y * 1000;
    }
    this->manual_state->starttime_ms = timeSinceEpochMillisec();
    this->manual_state->releasetime_ms = timeSinceEpochMillisec() + (60*1000); // 60 sec
    if ((this->manual_state->ct_active) && (this->have_color)) {
        MYSQL *ms = Mysql_Connect();
        if (ms != NULL) {
            string query = "SELECT x1, y1 FROM cttoxy WHERE ct=" + inttostring(data["ct"].GetInt());
            if (!mysql_query(ms, query.c_str())) {
                MYSQL_RES *result = mysql_store_result(ms);
                if (result != NULL) {
                    //Get the number of columns
                    int num_rows = mysql_num_rows(result);
                    int num_fields = mysql_num_fields(result);

                    MYSQL_ROW row; //An array of strings
                    while ((row = mysql_fetch_row(result))) {
                        if (num_fields >= 1) {
                            this->manual_state->raw_x = atof(row[0]);
                            this->manual_state->raw_y = atof(row[1]);
                            this->manual_state->x = this->manual_state->raw_x * 1000;
                            this->manual_state->y = this->manual_state->raw_y * 1000;

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
    }
    m.unlock();
};
 

void Light::set_background(int brightness, int mirek, int x, int y) {
    m.lock();
    this->background_set = true;
    this->background_state->brightness = brightness;
    this->background_state->raw_brightness = float(brightness) / 100;
    this->background_state->colortemp = mirek;
    this->background_state->x = x;
    this->background_state->raw_x = float(x) / 1000;
    this->background_state->y = y;
    this->background_state->raw_y = float(y) / 1000;
    this->background_state->transistion = 2000;
    int bp = 0;
    bp++;
    m.unlock();

}

void Light::set_room_brightness(float brightness) {

    m.lock();
    this->room_brightness = brightness;
    m.unlock();

};

/*
void Light::stop_manual() {
    m.lock();
    this->manual_state->releaseat = 0;
    this->manual_state->manual_brightness = false;
    this->manual_state->manual_color = false;
    m.unlock();
};
 */

/*
void Light::set_manual_brightness(int brightness, time_t release) {
    m.lock();
    this->manual_state->brightness = brightness * 100;
    this->manual_state->manual_brightness = true;
    this->manual_state->releaseat = release;
    m.unlock();
};
 */

/*
void Light::set_manual_ct(int ct, time_t release) {
    m.lock();
    this->manual_state->colortemp = 1000000 / ct;
    this->manual_state->manual_color = true;
    this->manual_state->releaseat = release;
    if (this->have_color) {
        MYSQL *ms = Mysql_Connect();
        if (ms != NULL) {
            string query = "SELECT x1, y1 FROM cttoxy WHERE ct=" + inttostring(ct);
            if (!mysql_query(ms, query.c_str())) {
                MYSQL_RES *result = mysql_store_result(ms);
                if (result != NULL) {
                    //Get the number of columns
                    int num_rows = mysql_num_rows(result);
                    int num_fields = mysql_num_fields(result);

                    MYSQL_ROW row; //An array of strings
                    while ((row = mysql_fetch_row(result))) {
                        if (num_fields >= 1) {
                            this->manual_state->raw_x = atof(row[0]);
                            this->manual_state->raw_y = atof(row[1]);
                            this->manual_state->x = this->manual_state->raw_x * 1000;
                            this->manual_state->y = this->manual_state->raw_y * 1000;
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
    m.unlock();
};
 */

/*
void Light::set_manual_color(int x, int y, time_t release) {
    m.lock();
    if (this->have_color) {
        this->manual_state->x = x;
        this->manual_state->y = y;
        this->manual_state->manual_color = true;
        this->manual_state->releaseat = release;
    }
    m.unlock();
};
 */

/*
void Light::set_manual_release_time(time_t time) {
    m.lock();
    if ((this->manual_state->manual_brightness) || (this->manual_state->manual_color)) {
        if (time > this->manual_state->releaseat) {
            this->manual_state->releaseat = time;
        }
    }
    m.unlock();


};
 */

void Light::Identify() {
    if (this->host_device != NULL) {
        this->host_device->Ident();
    }

};

/*
void Light::set_mesh_data(int intensity, int red, int green, int blue, string name, int nodeid, int index) {
    m.lock();
     
    if (intensity > 0) {
        this->cur_state->on = true;
    } else {
        this->cur_state->on = false;
    }
    float tmp = (float(intensity) / 255) * 100;
    this->cur_state->raw_brightness = tmp;
    this->cur_state->brightness = tmp * 100;
    this->cur_state->r = red;
    this->cur_state->g = green;
    this->cur_state->b = blue;
    this->name = name;
    this->mesh_nodeid = nodeid;
    this->mesh_index = index;
    this->archetype = "MeshRGB";
    this->have_color = true;
    this->have_dim = true;
    m.unlock();
}
 */

void Light::calc_rgb() {
    float x = this->des_state->x; // the given x value
    float y = this->des_state->y; // the given y value
    float z = 1.0f - x - y;
    float Y = 1.0f; // The given brightness value
    float X = (Y / y) * x;
    float Z = (Y / y) * z;

    float r = X * 1.656492f - Y * 0.354851f - Z * 0.255038f;
    float g = -X * 0.707196f + Y * 1.655397f + Z * 0.036152f;
    float b = X * 0.051713f - Y * 0.121364f + Z * 1.011530f;

    if (r > b && r > g && r > 1.0f) {
        // red is too big
        g = g / r;
        b = b / r;
        r = 1.0f;
    } else if (g > b && g > r && g > 1.0f) {
        // green is too big
        r = r / g;
        b = b / g;
        g = 1.0f;
    } else if (b > r && b > g && b > 1.0f) {
        // blue is too big
        r = r / b;
        g = g / b;
        b = 1.0f;
    }

    // Apply gamma correction
    // r = r <= 0.0031308f ? 12.92f * r : (1.0f + 0.055f) * pow(r, (1.0f / 2.4f)) - 0.055f;
    //  g = g <= 0.0031308f ? 12.92f * g : (1.0f + 0.055f) * pow(g, (1.0f / 2.4f)) - 0.055f;
    //  b = b <= 0.0031308f ? 12.92f * b : (1.0f + 0.055f) * pow(b, (1.0f / 2.4f)) - 0.055f;

    /*
    if (r > b && r > g) {
        // red is biggest
        if (r > 1.0f) {
            g = g / r;
            b = b / r;
            r = 1.0f;
        }
    } else if (g > b && g > r) {
        // green is biggest
        if (g > 1.0f) {
            r = r / g;
            b = b / g;
            g = 1.0f;
        }
    } else if (b > r && b > g) {
        // blue is biggest
        if (b > 1.0f) {
            r = r / b;
            g = g / b;
            b = 1.0f;
        }
    }
     */

    this->des_state->r = round(r * 255);
    this->des_state->g = round(g * 255);
    this->des_state->b = round(b * 255);



    if (this->des_state->r > 255) {
        this->des_state->r = 255;
    }
    if (this->des_state->r < 0) {
        this->des_state->r = 0;
    }
    if (this->des_state->g > 255) {
        this->des_state->g = 255;
    }
    if (this->des_state->g < 0) {
        this->des_state->g = 0;
    }
    if (this->des_state->b > 255) {
        this->des_state->b = 255;
    }
    if (this->des_state->b < 0) {
        this->des_state->b = 0;
    }

};

void Light::set_scene(int sceneid, uint32_t fade_in_ms, uint32_t releasetime_ms, int brightness, int mirek, int x, int y) {
    m.lock();
    uint64_t nu = timeSinceEpochMillisec();
     
    for (int i = 0; i < this->scene_states.size(); i++) {
        if (this->scene_states[i]->sceneid == sceneid) {
            delete this->scene_states[i];
            this->scene_states.erase(this->scene_states.begin()+i);
        }
    }
    
        LightState *s = new LightState();
        s->sceneid = sceneid;
        s->fadetime_ms = fade_in_ms;
        s->starttime_ms = nu;
        s->releasetime_ms = nu + releasetime_ms;
        s->raw_release_time = releasetime_ms;
        s->brightness = brightness;
        s->colortemp = mirek;
        s->x = x;
        s->y = y;
        this->scene_states.push_back(s);
    
    m.unlock();
}

void Light::remove_scene(int sceneid){
    m.lock();
     for (int i = 0; i < this->scene_states.size(); i++) {
        if (this->scene_states[i]->sceneid == sceneid) {
            delete this->scene_states[i];
            this->scene_states.erase(this->scene_states.begin()+i);
            this->scene_states.shrink_to_fit();
            break;
        }
     }    
    m.unlock();
}

void Light::extent_scene(int sceneid, uint64_t motiontime_ms){
    m.lock();
    for (int i = 0; i < this->scene_states.size(); i++) {
        if (this->scene_states[i]->sceneid == sceneid) {
            if(this->scene_states[i]->releasetime_ms < motiontime_ms + this->scene_states[i]->raw_release_time){
            this->scene_states[i]->releasetime_ms = motiontime_ms + this->scene_states[i]->raw_release_time;
            }
            break;            
        }
    }
    m.unlock();
}

bool Light::is_scene_active(int sceneid){
    bool ret = false;
    m.lock();
    for(int i = 0; i<this->scene_states.size(); i++){
        if(this->scene_states[i]->sceneid == sceneid){
            ret = true;
            break;
        }
    }
    m.unlock();
    return ret;
}

void Light::set_manual(int brightness, uint64_t releasetime_ms){
    m.lock();
    this->manual_state->brightness = brightness * 100;
    this->manual_state->raw_brightness = brightness;
     uint64_t nu = timeSinceEpochMillisec();
    this->manual_state->releasetime_ms = nu + releasetime_ms;
    m.unlock();
}