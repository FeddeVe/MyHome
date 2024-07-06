
/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Background.cpp
 * Author: Fedde
 * 
 * Created on 19 februari 2022, 20:20
 */

#include "Background.h"

Background::Background(Master *refmaster) {
    this->refmaster = refmaster;
    this->act_slot = NULL;
    this->prev_slot = NULL;
    this->act_scene = NULL;
    this->prev_scene = NULL;
    //this->overnight = false;
    this->first_select = false;

    this->load_slots();
    this->load_sunset();
}

Background::Background(const Background& orig) {
}

Background::~Background() {
    for (int i = 0; i < this->slots.size(); i++) {
        delete this->slots[i];
    }
}

void Background::quit() {
    this->exit = true;
};

void *Background::run() {
    this->exit = false;

    // while (this->exit == false) {





    //    sleep(10);
    //}
    return 0;

};

void Background::calc_values() {

    time_t diff = this->act_slot->time_end - this->prev_slot->time_end;
    //if (diff < 0) {
    //    diff = diff + S_DAY;
    //}
    int diff2 = this->nu - this->prev_slot->time_end;
    //if (diff2 < 0) {
    //    diff2 = diff2 + S_DAY;
    //}

    if (diff > 0) {
        this->act_pct = float(diff2) / float(diff);
    } else {
        this->act_pct = 1.0F;
    }



    for (int i = 0; i < this->prev_scene->Values.size(); i++) {
        if (this->prev_scene->Values[i]->inuse) {
            Background_Value *val = new Background_Value();
            val->lightid = this->prev_scene->Values[i]->lightid;
            val->old_dim = this->prev_scene->Values[i]->brightness;
            val->new_dim = 0;
            val->old_ct = this->prev_scene->Values[i]->ct;
            val->new_ct = this->prev_scene->Values[i]->ct;
            if (this->prev_scene->Values[i]->usect) {
                val->old_x = this->prev_scene->Values[i]->ct_x;
                val->new_x = this->prev_scene->Values[i]->ct_x;
                val->old_y = this->prev_scene->Values[i]->ct_y;
                val->new_y = this->prev_scene->Values[i]->ct_y;
            } else {
                val->old_x = this->prev_scene->Values[i]->x;
                val->new_x = this->prev_scene->Values[i]->x;
                val->old_y = this->prev_scene->Values[i]->y;
                val->new_y = this->prev_scene->Values[i]->y;
            }
            this->values.push_back(val);
        }
    }

    for (int i = 0; i < this->act_scene->Values.size(); i++) {
        if (this->act_scene->Values[i]->inuse) {
            bool nieuw = true;
            for (int y = 0; y < this->values.size(); y++) {
                if (this->values[y]->lightid == this->act_scene->Values[i]->lightid) {
                    nieuw = false;
                    this->values[y]->new_dim = this->act_scene->Values[i]->brightness;
                    this->values[y]->new_ct = this->act_scene->Values[i]->ct;
                    if (this->act_scene->Values[i]->usect) {
                        this->values[y]->new_x = this->act_scene->Values[i]->ct_x;
                        this->values[y]->new_y = this->act_scene->Values[i]->ct_y;
                    } else {
                        this->values[y]->new_x = this->act_scene->Values[i]->x;
                        this->values[y]->new_y = this->act_scene->Values[i]->y;
                    }
                    break;
                }
            }
            if (nieuw) {
                Background_Value *val = new Background_Value();
                val->lightid = this->act_scene->Values[i]->lightid;
                val->old_dim = 0;
                val->new_dim = this->act_scene->Values[i]->brightness;
                val->old_ct = this->act_scene->Values[i]->ct;
                val->new_ct = this->act_scene->Values[i]->ct;
                if (this->act_scene->Values[i]->usect) {
                    val->old_x = this->act_scene->Values[i]->ct_x;
                    val->new_x = this->act_scene->Values[i]->ct_x;
                    val->old_y = this->act_scene->Values[i]->ct_y;
                    val->new_y = this->act_scene->Values[i]->ct_y;
                } else {
                    val->old_x = this->act_scene->Values[i]->x;
                    val->new_x = this->act_scene->Values[i]->x;
                    val->old_y = this->act_scene->Values[i]->y;
                    val->new_y = this->act_scene->Values[i]->y;
                }
                this->values.push_back(val);
            }
        }
    }
    for (int i = 0; i < this->values.size(); i++) {
        int dimdiff = this->values[i]->new_dim - this->values[i]->old_dim;
        dimdiff = dimdiff * this->act_pct;
        this->values[i]->res_dim = this->values[i]->old_dim + dimdiff;
        int ctdiff = this->values[i]->new_ct - this->values[i]->old_ct;
        ctdiff = ctdiff * this->act_pct;
        this->values[i]->res_ct = this->values[i]->old_ct + ctdiff;
        int xdiff = this->values[i]->new_x - this->values[i]->old_x;
        xdiff = xdiff * this->act_pct;
        this->values[i]->res_x = this->values[i]->old_x + xdiff;
        int ydiff = this->values[i]->new_y - this->values[i]->old_y;
        ydiff = ydiff * this->act_pct;
        this->values[i]->res_y = this->values[i]->old_y + ydiff;
    }

    /// SENDING VALUES TO LIGHTS
    for (int i = 0; i < this->values.size(); i++) {
        Light *l = this->refmaster->lights->getlight(this->values[i]->lightid);
        if (l != NULL) {
            l->set_background(this->values[i]->res_dim, this->values[i]->res_ct, this->values[i]->res_x, this->values[i]->res_y);
        }
    }

    for (int i = 0; i < this->values.size(); i++) {
        delete this->values[i];
        this->values[i] = NULL;
    }
    this->values.clear();


    int bp = 0;
    bp++;


};

bool Background::select_slots() {

    time_t t = time(0);

    //this->overnight = false;
    this->prev_scene = this->act_scene;
    this->prev_slot = this->act_slot;
    this->act_scene = NULL;
    this->act_slot = NULL;

    for (int i = 0; i < this->slots.size(); i++) {
        if (this->slots[i]->time_end < this->nu) {
            int new_index = i + 1;
            if (new_index == this->slots.size()) {
                //this->overnight = true;
                new_index = 0;
                return false;
            }
            this->act_slot = this->slots[new_index];
            std::random_device seeder;
            std::mt19937 rng(seeder());
            int min = 0;
            int max = this->act_slot->scenes.size() - 1;
            std::uniform_int_distribution<int> gen(min, max); // uniform, unbiased
            int r = gen(rng);
            this->act_scene = this->act_slot->scenes[r];


            if ((this->prev_scene == NULL) || (this->first_select == false)) {
                this->prev_slot = this->slots[i];
                min = 0;
                max = this->prev_slot->scenes.size() - 1;
                std::uniform_int_distribution<int> gen2(min, max); // uniform, unbiased
                int r2 = gen2(rng);
                this->prev_scene = this->prev_slot->scenes[r2];
            }
        }
    }
    if (this->act_slot == NULL) {
        // this->overnight = true;
        this->act_slot = this->slots[0];
        std::random_device seeder;
        std::mt19937 rng(seeder());
        int min = 0;
        int max = this->act_slot->scenes.size() - 1;
        std::uniform_int_distribution<int> gen(min, max); // uniform, unbiased
        int r = gen(rng);
        this->act_scene = this->act_slot->scenes[r];
        if ((this->prev_slot == NULL) || (this->first_select == false)) {
            this->prev_slot = this->slots[this->slots.size() - 1];
            int min = 0;
            int max = this->prev_slot->scenes.size() - 1;
            std::uniform_int_distribution<int> gen2(min, max); // uniform, unbiased
            int r2 = gen2(rng);
            this->prev_scene = this->prev_slot->scenes[r2];
        }

    }
    this->first_select = true;
    return true;
};

void Background::load_sunset() {

    this->last_sunset_check = time(0);
    sun.setPosition(52.3076865, 4.767424099999971, +1);
    std::time_t t = std::time(0);
    tm* now = std::gmtime(&t);
    sun.setCurrentDate(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday + 1);
    int sunrise = (sun.calcSunrise() + (60 * 2)) * 60;
    if (sunrise > 32400) {
        sunrise = 32400;
    }
    int sunset = (sun.calcSunset() - (60)) * 60;
    if (sunset < 61200) {
        sunset = 61200;
    }
    int diffnight = sunrise + (86400 - sunset);
    int diffday = sunset - sunrise;
    if (diffnight < 0) {
        diffnight = diffnight * -1;
    }
    int day_steps = diffday / 3;
    int night_steps = diffnight / 4;

    time_t nu = time(0);
    struct tm *curtime = localtime(&nu);
    curtime->tm_hour = 0;
    curtime->tm_min = 0;
    curtime->tm_sec = 0;
    time_t startday = mktime(curtime);

    int act_slot_id = -1;
    if (this->act_slot != NULL) {
        act_slot_id = this->act_slot->id;
    }
    time_t res = 0;
    //   delete curtime;
    for (int i = 0; i < this->slots.size(); i++) {
        int id = this->slots[i]->id;
        switch (id) {
            case 0:
                // night
                if (act_slot_id != 0) {
                    res = startday + sunset + night_steps + night_steps + night_steps;
                    if (res < t) {
                        res = res + 86400;
                    }
                    if(res - t > 86400){
                            res = res - 86400;
                        }
                    this->slots[i]->time_end = res;
                }
                break;
            case 1:
                // Morning
                if (act_slot_id != 1) {
                    res = startday + sunrise;
                    if (res < t) {
                        res = res + 86400;
                    }
                    if(res - t > 86400){
                            res = res - 86400;
                        }
                    this->slots[i]->time_end = res;
                }
                break;
            case 2:
                // Day 1
                if (act_slot_id != 2) {
                    res = startday + sunrise + day_steps;
                    if (res < t) {
                        res = res + 86400;
                    }
                    if(res - t > 86400){
                            res = res - 86400;
                        }
                    this->slots[i]->time_end = res;
                }
                break;
            case 3:
                // Day 2
                if (act_slot_id != 3) { 
                        res = startday + sunrise + day_steps + day_steps;
                        if (res < t) {
                            res = res + 86400;
                        }
                        if(res - t > 86400){
                            res = res - 86400;
                        }
                        this->slots[i]->time_end = res;
                    }
                    break;
                    case 4:
                    // Diner
                    if (act_slot_id != 4) {
                        res = startday + sunset;
                        if (res < t) {
                            res = res + 86400;
                        }
                        if(res - t > 86400){
                            res = res - 86400;
                        }
                        this->slots[i]->time_end = res;
                    }
                    break;
                    case 5:
                    // Evening
                    if (act_slot_id != 5) {
                        res = startday + sunset + night_steps;
                        if (res < t) {
                            res = res + 86400;
                        }
                        if(res - t > 86400){
                            res = res - 86400;
                        }
                        this->slots[i]->time_end = res;
                    }
                    break;
                    case 6:
                    // Evening
                    if (act_slot_id != 6) {
                        res = startday + sunset + night_steps + night_steps;
                        if (res < t) {
                            res = res + 86400;
                        }
                        if(res - t > 86400){
                            res = res - 86400;
                        }
                        this->slots[i]->time_end = res;
                    }
                    break;

                }
        }




        //for (int i = 0; i<this->slots.size(); i++) {
        //    if (this->slots[i]->time_end >= 86400) {
        //        this->slots[i]->time_end = this->slots[i]->time_end - 86400;
        //    }
        //}

        sort(this->slots.begin(), this->slots.end(), [ ](const Background_Slot *lhs, const Background_Slot * rhs) {
            return lhs->time_end < rhs->time_end;
        });



    };

    void Background::load_slots() {

        Background_Slot *s = new Background_Slot(0);

        s->name = "Night";
        this->slots.push_back(s);
        Background_Slot *s1 = new Background_Slot(1);

        s1->name = "Morning";
        this->slots.push_back(s1);
        Background_Slot *s2 = new Background_Slot(2);

        s2->name = "Early_Day";
        this->slots.push_back(s2);

        Background_Slot *s3 = new Background_Slot(3);

        s3->name = "Later Day";
        this->slots.push_back(s3);
        Background_Slot *s4 = new Background_Slot(4);

        s4->name = "Diner";
        this->slots.push_back(s4);
        Background_Slot *s5 = new Background_Slot(5);

        s5->name = "Early Evening";
        this->slots.push_back(s5);

        Background_Slot *s6 = new Background_Slot(6);

        s6->name = "Late Evening";
        this->slots.push_back(s6);
    };

    string Background::GetJson() {
        lock_guard<mutex>lg(m);
        Document d;
        d.SetObject();
        Document::AllocatorType &allocator = d.GetAllocator();

        Value v;
        v.SetString("Background", allocator);
        d.AddMember("item", v, allocator);
        d.AddMember("id", -1, allocator);
        Value timenu;
        timenu.SetUint64(this->nu);
        d.AddMember("Time", timenu, allocator);
        Value timesuncalc;
        timesuncalc.SetUint64(this->last_sunset_check);
        d.AddMember("SunCalc", timesuncalc, allocator);
        Value MyArray(rapidjson::kArrayType);
        for (int i = 0; i < this->slots.size(); i++) {
            Value slot;
            slot.SetObject();
            Value name;
            name.SetString(this->slots[i]->name.c_str(), allocator);
            slot.AddMember("ID", this->slots[i]->id, allocator);
            slot.AddMember("Name", name, allocator);
            Value time_end;
            time_end.SetUint64(this->slots[i]->time_end);
            slot.AddMember("Time", time_end, allocator);
            Value scArray(rapidjson::kArrayType);
            for (int y = 0; y< this->slots[i]->scenes.size(); y++) {
                Value scene;
                scene.SetObject();
                scene.AddMember("ID", this->slots[i]->scenes[y]->Get_ID(), allocator);
                Value sc_name;
                sc_name.SetString(this->slots[i]->scenes[y]->Get_Name().c_str(), allocator);
                scene.AddMember("Name", sc_name, allocator);

                if (this->slots[i]->scenes[y]->Get_ID() == this->act_scene->Get_ID()) {
                    string msg = "> " + inttostring(this->act_pct * 100);
                    Value status;
                    status.SetString(msg.c_str(), allocator);
                    scene.AddMember("Status", status, allocator);
                } else if (this->slots[i]->scenes[y]->Get_ID() == this->prev_scene->Get_ID()) {
                    string msg = "< " + inttostring(100 - (this->act_pct * 100));
                    Value status;
                    status.SetString(msg.c_str(), allocator);
                    scene.AddMember("Status", status, allocator);
                } else {
                    string msg = "";
                    Value status;
                    status.SetString(msg.c_str(), allocator);
                    scene.AddMember("Status", status, allocator);
                }
                scArray.PushBack(scene, allocator);
            }
            slot.AddMember("Scenes", scArray, allocator);
            MyArray.PushBack(slot, allocator);
        }
        d.AddMember("Slots", MyArray, allocator);

        string ret = GetJsonString(d);

        return ret;
    };

    void Background::add_slot_scene(int slotid, string name) {
        lock_guard<mutex>lg(m);
        for (int i = 0; i < this->slots.size(); i++) {
            if (this->slots[i]->id == slotid) {
                this->slots[i]->add_scene(name);

            }
        }

    };

    string Background::Get_Slot_Scene_Values(int slotid, int sceneid) {
        lock_guard<mutex>lg(m);
        for (int i = 0; i < this->slots.size(); i++) {
            if (this->slots[i]->id == slotid) {

                return this->slots[i]->Get_Scene_Values(sceneid);
            }
        }
        return "NOK";
    };

    void Background::do_work() {
        lock_guard<mutex>lg(m);


        std::time_t t;
        tm* now;
        try {
            t = std::time(0);

            now = std::gmtime(&t);

            //this->nu = (hour_to_local(now->tm_hour) * 60 * 60) + (now->tm_min * 60) + now->tm_sec;
            this->nu = time(0);
            //if (nu < 100) {
            //    overnight = false;
            //}
            if (this->act_scene == NULL) {
                if (this->select_slots() == false) {
                    this->load_sunset();
                    this->select_slots();
                };
            }
            //if (overnight) {

            //} else {
            if (nu > this->act_slot->time_end) {
                this->load_sunset();
                if (this->select_slots() == false) {

                    this->select_slots();
                }
            }
            //}

            this->calc_values();
        } catch (exception &e) {
            WriteError("Background - Loop", e.what());
        }

    };

    void Background::Save_Scene_Values(int slotid, int sceneid, Value & values) {
        lock_guard<mutex>lg(m);
        for (int i = 0; i < this->slots.size(); i++) {
            if (this->slots[i]->id == slotid) {
                this->slots[i]->Save_Scene_Values(sceneid, values);
            }
        }

    }

