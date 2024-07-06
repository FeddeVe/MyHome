/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Tree_Node.cpp
 * Author: Fedde
 * 
 * Created on 25 maart 2023, 15:49
 */

#include "Tree_Node.h"

Tree_Node::Tree_Node(Tree_Server *reftcp, Tree_Message *m, Master *refmaster) {
    this->reftcp = reftcp;
    this->refmaster = refmaster;
    this->msg_id = 0;
    this->online = true;
    this->UID = m->header->from;
    this->firmware = m->header->firmware;
    this->nodetype = m->header->node_type;
    this->config_loaded = false;
    this->last_message_send = time(0);
    this->config_reload_time = 0;
    this->config_reload_time_set = false;
    this->config_update_running = 0;
    this->layer = 255;
    for (uint32_t i = 0; i < 100; i++) {
        this->arr_avg_turn_time[i] = UINT16_MAX;
    };
    this->arr_avg_turn_time_index = 0;
    this->avg_turn_time = UINT16_MAX;
    this->reset_sensor_log();
    this->s_log.last_save = time(0);
    this->buttonstate = 999999;
    int bp = 0;
    bp++;


}

Tree_Node::Tree_Node(const Tree_Node& orig) {
}

Tree_Node::~Tree_Node() {
}

void Tree_Node::calc_avg_turn_time(Tree_Message *m) {
    uint64_t nu = this->reftcp->get_timestamp_ms();
    uint64_t diff = nu - m->time_stamp;
    if (diff > UINT16_MAX) {
        diff = UINT16_MAX;
    }
    if (this->arr_avg_turn_time_index >= 100) {
        this->arr_avg_turn_time_index = 0;
    }
    this->arr_avg_turn_time[this->arr_avg_turn_time_index] = diff;
    this->arr_avg_turn_time_index++;
    uint64_t tmp = 0;
    for (int i = 0; i < 100; i++) {
        tmp = tmp + this->arr_avg_turn_time[i];
    }
    tmp = tmp / 100;
    if (tmp > UINT16_MAX) {
        tmp = UINT16_MAX;
    }
    this->avg_turn_time = tmp;
    cout << "AVG TURN TIME = " << this->avg_turn_time << endl;


};

void Tree_Node::send_test_msg() {
    m.lock();
    if (this->online == true) {
        uint32_t size = 18;
        char data[size];
        int index = 0;
        for (uint32_t i = 0; i < size; i++) {
            char c = '-';
            switch (index) {
                case 0:
                    c = 'H';
                    index++;
                    break;
                case 1:
                    c = 'e';
                    index++;
                    break;
                case 2:
                    c = 'l';
                    index++;
                    break;
                case 3:
                    c = 'l';
                    index++;
                    break;
                case 4:
                    c = 'o';
                    index++;
                    break;
                case 5:
                    c = ' ';
                    index++;
                    break;
                case 6:
                    c = 'W';
                    index++;
                    break;
                case 7:
                    c = 'o';
                    index++;
                    break;
                case 8:
                    c = 'r';
                    index++;
                    break;
                case 9:
                    c = 'l';
                    index++;
                    break;
                case 10:
                    c = 'd';
                    index++;
                    break;
                case 11:
                    index = 0;
                    break;
            }
            data[i] = c;
        }
        data[size - 1] = '!';
        data[size - 2] = '!';
        data[size - 3] = '!';
        Tree_Message *m = new Tree_Message(this, this->UID, msg_types::test, this->get_msg_id(), size, data);
        this->reftcp->send_message(m);
    }
    m.unlock();
};

void Tree_Node::loop_worker() {
    m.lock();
    if (this->ota_running == true) {
        time_t diff = time(0) - this->ota_running_time;
        if (diff > 120) {
            this->ota_running = false;
        }
    }

    if (this->config_reload_time_set == true) {
        uint64_t nu = timeSinceEpochMillisec();
        if (nu - this->config_update_running > 60 * 1000) {
            if (nu > this->config_reload_time) {
                this->load_config();
                this->config_reload_time_set = false;
            }
        }
    }

    this->Get_Button_State();
    m.unlock();
    uint64_t diff = this->reftcp->get_timestamp_ms() - this->last_message_send;
    if (diff > this->reftcp->mesh_info_interval) {
        this->last_message_send = this->reftcp->get_timestamp_ms();
        Tree_Message *mes = new Tree_Message(this, this->UID, msg_types::Hello, this->get_msg_id(), 0, (char*) "");
        this->reftcp->send_message(mes);
    }
};

bool Tree_Node::handle_msg(Tree_Message *m) {
    this->m.lock();
    if (m->err) {
        this->m.unlock();
        return false;
    }

    this->online = true;
    this->firmware = m->header->firmware;
    this->nodetype = m->header->node_type;
    this->layer = m->header->layer;
    if (this->config_loaded == false) {
        this->load_config();
    }
    // check if fully recieved

    //  cout << "Node Reset State: CPU0 " << (uint8_t) m->header->core_0_reset_info << " - " << this->print_reset_reason(m->header->core_0_reset_info) << endl;
    //  cout << "Node Reset State: CPU1 " << (uint8_t) m->header->core_1_reset_info << " - " << this->print_reset_reason(m->header->core_1_reset_info) << endl;
    this->uptime = m->header->uptime;
    this->send_ack(m->header->from, m->header->msg_id);

    switch (m->header->type) {
        case msg_types::OTA_Request:
        {
            Tree_Message *new_req = this->reftcp->OTA->handle_OTA_Request(this, m);
            if (new_req != NULL) {
                this->reftcp->send_message(new_req);
                //this->send_ack(m->header->from, m->header->msg_id);
                this->ota_running_time = time(0);
                // this->m.unlock();
                // return true;
            } else {
                cout << "OTA REQUEST IS NULL " << endl;
            }
        }
            break;

        case msg_types::test:
        {
        }
            break;
        case msg_types::Config_Request:
        {
            Tree_Structs::str_Config_Request *request = (Tree_Structs::str_Config_Request*) m->header->data;
            this->upload_config(request->Index);
        }
            break;
        case msg_types::Config_Request_Announce:
        {
            this->announce_config_update();
        }
            break;
        case msg_types::BSEC_Data:
        {
            Tree_Structs::str_BME680 *data = (Tree_Structs::str_BME680*)m->header->data;
            this->temp_calibrated = data->Calibrated;
            if (data->Calibrated != 9999) {
                string sensor_UID = uint32tostring(this->UID) + "_BSEC";
                Sensor *s = this->refmaster->sensors->get_sensor(sensor_UID);
                if (s == NULL) {
                    s = new Sensor(sensor_UID, NULL, true);
                    s->set_name(this->name + " BME680");
                    this->refmaster->sensors->Add_Sensor(s);
                    if (this->config_loaded == true) {
                        Area *a = this->refmaster->areas->get_area(this->area_id);
                        if (a != NULL) {
                            bool nieuw = true;
                            for (int i = 0; i < a->sensors.size(); i++) {
                                if (a->sensors[i] == sensor_UID) {
                                    nieuw = false;
                                    break;
                                }
                            }
                            if (nieuw) {
                                a->sensors.push_back(sensor_UID);
                            }
                        }
                    }

                }
                s->set_bme680(data->static_iaq, data->static_iaq_accuracy, data->co2, data->compensated_humidity, data->compensated_temperature, data->pressure);

            }
            this->s_log.co2 = data->co2;
            this->s_log.comp_humidity = data->compensated_humidity;
            this->s_log.comp_temperature = data->compensated_temperature;
            this->s_log.cpu_0_res_reason = m->header->core_0_reset_info;
            this->s_log.cpu_1_res_reason = m->header->core_1_reset_info;
            this->s_log.iaq_accuracy = data->iaq_accuracy;
            this->s_log.pressure = data->pressure;
            this->s_log.raw_humidity = data->raw_humidity;
            this->s_log.raw_temperature = data->raw_temperature;
            this->s_log.static_iaq = data->static_iaq;
            this->s_log.uptime = m->header->uptime;
            this->save_sensor_log();

        }
            break;
        case msg_types::Pir_Sensor:
        {
            //if (m->header->data[0] == 1) {
            string sensor_UID = uint32tostring(this->UID) + "_PIR";
            Sensor *s = this->refmaster->sensors->get_sensor(sensor_UID);
            if (s == NULL) {
                s = new Sensor(sensor_UID, NULL, true);
                s->set_name(this->name + " Motion Sensor");
                this->refmaster->sensors->Add_Sensor(s);
                if (this->config_loaded == true) {
                    Area *a = this->refmaster->areas->get_area(this->area_id);
                    if (a != NULL) {
                        bool nieuw = true;
                        for (int i = 0; i < a->sensors.size(); i++) {
                            if (a->sensors[i] == sensor_UID) {
                                nieuw = false;
                                break;
                            }
                        }
                        if (nieuw) {
                            a->sensors.push_back(sensor_UID);
                        }
                    }
                }
            }

            this->s_log.uptime = m->header->uptime;
            this->save_sensor_log();
            if (m->header->data[0] == 1) {
                this->s_log.motion = 1;
                s->set_motion(true);
            } else {
                s->set_motion(false);
            }
            //}
        }
            break;
        case msg_types::Lux_report:
        {
            Tree_Structs::str_lux *lux = (Tree_Structs::str_lux*)m->header->data;
            string sensor_UID = uint32tostring(this->UID) + "_LUX";
            Sensor *s = this->refmaster->sensors->get_sensor(sensor_UID);
            if (s == NULL) {
                s = new Sensor(sensor_UID, NULL, true);
                this->refmaster->sensors->Add_Sensor(s);
                s->set_name(this->name + " Light Sensor");
                if (this->config_loaded == true) {
                    Area *a = this->refmaster->areas->get_area(this->area_id);
                    if (a != NULL) {
                        bool nieuw = true;
                        for (int i = 0; i < a->sensors.size(); i++) {
                            if (a->sensors[i] == sensor_UID) {
                                nieuw = false;
                                break;
                            }
                        }
                        if (nieuw) {
                            a->sensors.push_back(sensor_UID);
                        }
                    }
                }
            }
            this->s_log.lux = lux->buffered_lux;
            this->s_log.uptime = m->header->uptime;
            this->save_sensor_log();
            s->set_lightlevel(lux->buffered_lux, true);

        }
            break;
        case msg_types::Temperature:
        {
            Tree_Structs::str_temp *temp = (Tree_Structs::str_temp*)m->header->data;
            if (this->nodetype == NODE_TYPES::MSH_DISPLAY) {
                this->s_log.internal_temperature = temp->temperature;
                this->save_sensor_log();
            }
        }
            break;
        case msg_types::Button_Click:
        {
            int btn = m->header->data[0];
            this->handle_buttons(btn);

        }
            break;
        case msg_types::Door_Window_Sensor:
        {
            if (this->io_actions[0]->function == 1) {
                //door
                string sensor_UID = uint32tostring(this->UID) + "_IO_Input";
                Sensor *s = this->refmaster->sensors->get_sensor(sensor_UID);
                if (s == NULL) {
                    s = new Sensor(sensor_UID, NULL, true);
                    this->refmaster->sensors->Add_Sensor(s);
                    s->set_name(this->name + " Door Sensor");
                    Area *a = this->refmaster->areas->get_area(this->area_id);
                    if (a != NULL) {
                        bool nieuw = true;
                        for (int i = 0; i < a->door_sensors.size(); i++) {
                            if (a->door_sensors[i] == sensor_UID) {
                                nieuw = false;
                                break;
                            }
                        }
                        if (nieuw) {
                            a->door_sensors.push_back(sensor_UID);
                        }
                    }
                }
                s->set_io_state(m->header->data[0]);
            } else if (this->io_actions[0]->function == 2) {
                //door
                string sensor_UID = uint32tostring(this->UID) + "_IO_Input";
                Sensor *s = this->refmaster->sensors->get_sensor(sensor_UID);
                if (s == NULL) {
                    s = new Sensor(sensor_UID, NULL, true);
                    this->refmaster->sensors->Add_Sensor(s);
                    s->set_name(this->name + " Window Sensor");
                    Area *a = this->refmaster->areas->get_area(this->area_id);
                    if (a != NULL) {
                        bool nieuw = true;
                        for (int i = 0; i < a->window_sensors.size(); i++) {
                            if (a->window_sensors[i] == sensor_UID) {
                                nieuw = false;
                                break;
                            }
                        }
                        if (nieuw) {
                            a->window_sensors.push_back(sensor_UID);
                        }
                    }
                }
                s->set_io_state(m->header->data[0]);
            }
        }
            break;
        case msg_types::TempCalibPublish:
        {
            Tree_Message *ret = new Tree_Message(this, 0xFFFFFFFF, msg_types::TempCalibReport, this->get_msg_id(), m->header->data_size, m->header->data);
            this->reftcp->send_message(ret);
        }
        break;
        case msg_types::Error_msg:
        {
            Tree_Structs::_str_error *error = (Tree_Structs::_str_error*)m->header->data;
            MYSQL *ms = Mysql_Connect();
            if (ms != NULL) {
                string query = "INSERT INTO Tree_Node_Error(nodeid, msg, code) VALUES (" + uint32tostring(this->UID) + ",'" + error->msg + "', " + inttostring(error->code) + ")";
                if (!mysql_query(ms, query.c_str())) {

                } else {
                    string ms_err = string(mysql_error(ms));
                    cout << "MYSQL Error " << ms_err << endl;
                }
                Mysql_Disconnect(ms);
            }



        }
            break;
        case msg_types::PWRMeter:
        {
            Tree_Structs::str_pwrmeter *pwr = (Tree_Structs::str_pwrmeter*)m->header->data;
           // float t1 = (float) pwr->t1 / 1000;
           // float t2 = (float) pwr->t2 / 1000;
           // float live = (float) pwr->live / 1000;
           // float gas = (float) pwr->gas / 1000;
            this->refmaster->powermeter->set_data(pwr->deliverd1, pwr->deliverd2, pwr->current_deliverd, pwr->gas, pwr->returned1, pwr->returned2, pwr->l1_volt, pwr->l2_volt, pwr->l3_volt, pwr->l1_amp, pwr->l2_amp, pwr->l3_amp, pwr->current_returned);
           // this->refmaster->powermeter->set_data(t1, t2, live, gas);
        }
            break;
        default:
        {
            int bp = 0;
            bp++;
        }
            break;

    }





    //  if (m-> >= m->header->total_size) {
    // complete package recieved
    //this->refmesh->message_recieved(m);
    if (this->ota_running == false) {
        Tree_Message *m_ota = this->reftcp->OTA->check_firmware(this);
        if (m_ota != NULL) {
            cout << "NODE UPDATE NEEDED" << endl;
            this->reftcp->send_message(m_ota);
            this->ota_running = true;
            //delete m_ota;
            this->ota_running_time = time(0);
        }
    }



    this->m.unlock();
    return true;
    //  }
    /*
   if (m->header->index = 0) {
       // recieved the first from more
       int index = this->get_free_mes_buffer();
       if (index == -1) {
           log_d("HandleMYMSG : No free space in buffer");
           return false;
       }
       this->mes_buffer[index] = m;
       this->send_ack(m->header->from, m->bytes_recieved + 1, m->header->UID);
       return true;
   }

   // recieved the rest
   uint32_t msgid = m->header->UID;
   for (int i = 0; i < BUFFER_SIZE; i++) {
       if (this->mes_buffer[i] != NULL) {
           if (this->mes_buffer[i]->header->UID == msgid) {
               this->mes_buffer[i]->add_data(m);
               if (this->mes_buffer[i]->bytes_recieved >= this->mes_buffer[i]->header->total_size) {
                   this->refmesh->message_recieved(this->mes_buffer[i]);
                   this->send_ack(this->mes_buffer[i]->header->from, this->mes_buffer[i]->bytes_recieved + 1, this->mes_buffer[i]->header->UID);
                   delete this->mes_buffer[i];
                   this->mes_buffer[i] = NULL;
               } else {
                   this->send_ack(this->mes_buffer[i]->header->from, this->mes_buffer[i]->bytes_recieved + 1, this->mes_buffer[i]->header->UID);
               }
           }
       }
   }
   if (m != NULL) {
       delete m;
   }
   return true;
     */

};

string Tree_Node::print_reset_reason(int reason) {
    switch (reason) {
        case 1: return "POWERON_RESET";
            break; /**<1,  Vbat power on reset*/
        case 3: return "SW_RESET";
            break; /**<3,  Software reset digital core*/
        case 4: return "OWDT_RESET";
            break; /**<4,  Legacy watch dog reset digital core*/
        case 5: return "DEEPSLEEP_RESET";
            break; /**<5,  Deep Sleep reset digital core*/
        case 6: return "SDIO_RESET";
            break; /**<6,  Reset by SLC module, reset digital core*/
        case 7: return "TG0WDT_SYS_RESET";
            break; /**<7,  Timer Group0 Watch dog reset digital core*/
        case 8: return "TG1WDT_SYS_RESET";
            break; /**<8,  Timer Group1 Watch dog reset digital core*/
        case 9: return "RTCWDT_SYS_RESET";
            break; /**<9,  RTC Watch dog Reset digital core*/
        case 10: return "INTRUSION_RESET";
            break; /**<10, Instrusion tested to reset CPU*/
        case 11: return "TGWDT_CPU_RESET";
            break; /**<11, Time Group reset CPU*/
        case 12: return "SW_CPU_RESET";
            break; /**<12, Software reset CPU*/
        case 13: return "RTCWDT_CPU_RESET";
            break; /**<13, RTC Watch dog Reset CPU*/
        case 14: return "EXT_CPU_RESET";
            break; /**<14, for APP CPU, reseted by PRO CPU*/
        case 15: return "RTCWDT_BROWN_OUT_RESET";
            break; /**<15, Reset when the vdd voltage is not stable*/
        case 16: return "RTCWDT_RTC_RESET";
            break; /**<16, RTC Watch dog reset digital core and rtc module*/
        default: return "NO_MEAN";
    }
}

void Tree_Node::reload_config() {
    m.lock();
    this->config_reload_time = timeSinceEpochMillisec() + (30 * 1000);
    this->config_reload_time_set = true;
    m.unlock();
}

void Tree_Node::load_config() {
    for (int i = 0; i < this->io_actions.size(); i++) {
        delete this->io_actions[i];
    }
    this->io_actions.clear();
    this->io_actions.shrink_to_fit();
    for (int i = 0; i < this->button_actions.size(); i++) {
        delete this->button_actions[i];
    }
    this->button_actions.clear();
    this->button_actions.shrink_to_fit();


    if (this->nodetype == NODE_TYPES::MSH_DISPLAY) {
        this->load_screen_config();
    }

    this->config_loaded = true;
    this->config_reload_time = 0;
};

void Tree_Node::load_screen_config() {
    string jsonfilename = "/var/www/html/ScreenConfigurator/Screen/" + uint32tostring(this->UID) + "_conf.json";
    //inline bool exists_test1 (const std::string& name) {
    std::ifstream testfile(jsonfilename);
    if (testfile.good()) {
        testfile.close();
    } else {
        jsonfilename = "/var/www/html/ScreenConfigurator/Screen/Default_conf.json";
    }


    std::ifstream infile(jsonfilename);
    std::vector<char> buffer;
    if (infile.good()) {
        this->config_file = jsonfilename;

        //get length of file
        infile.seekg(0, infile.end);
        size_t length = infile.tellg();
        infile.seekg(0, infile.beg);

        //read file
        if (length > 0) {
            buffer.resize(length);
            infile.read(&buffer[0], length);
        }
        infile.close();
    } else {
        //code to run default config
        int bp = 0;
        bp++;
    }
    buffer.push_back(0);

    Document d;
    ParseResult ok = d.Parse(buffer.data());
    ;
    if (!ok) {
        fprintf(stderr, "JSON parse error: %s (%u)",
                GetParseError_En(ok.Code()), ok.Offset());
        //exit(EXIT_FAILURE);
        cout << "Buffer : " << endl;
        cout << buffer.data() << endl;
        return;
    }

    int isconverted = d["converted"].GetInt();
    uint32_t version = d["version"].GetInt();
    this->area_id = d["area"].GetInt();
    this->config_version = version;
    this->name = d["nodename"].GetString();

    io_action *io = new io_action();
    io->function = d["io_input"].GetInt();
    this->io_actions.push_back(io);

    button_action *action = new button_action();
    action->function = d["btn1_function"].GetInt();
    action->intensity = d["btn1_intensity"].GetInt();
    action->fade_time = d["btn1_time"].GetInt();
    action->release_time = d["btn1_rel_time"].GetInt();
    action->system_options = d["btn1_system_options"].GetInt();
    action->areasceneid = d["btn1_area_scene"].GetInt();
    action->globalsceneid = d["btn1_global_scene"].GetInt();
    this->button_actions.push_back(action);
    action = new button_action();
    action->function = d["btn2_function"].GetInt();
    action->intensity = d["btn2_intensity"].GetInt();
    action->fade_time = d["btn2_time"].GetInt();
    action->release_time = d["btn2_rel_time"].GetInt();
    action->system_options = d["btn2_system_options"].GetInt();
    action->areasceneid = d["btn2_area_scene"].GetInt();
    action->globalsceneid = d["btn2_global_scene"].GetInt();
    this->button_actions.push_back(action);
    action = new button_action();
    action->function = d["btn3_function"].GetInt();
    action->intensity = d["btn3_intensity"].GetInt();
    action->fade_time = d["btn3_time"].GetInt();
    action->release_time = d["btn3_rel_time"].GetInt();
    action->system_options = d["btn3_system_options"].GetInt();
    action->areasceneid = d["btn3_area_scene"].GetInt();
    action->globalsceneid = d["btn3_global_scene"].GetInt();
    this->button_actions.push_back(action);
    action = new button_action();
    action->function = d["btn4_function"].GetInt();
    action->intensity = d["btn4_intensity"].GetInt();
    action->fade_time = d["btn4_time"].GetInt();
    action->release_time = d["btn4_rel_time"].GetInt();
    action->system_options = d["btn4_system_options"].GetInt();
    action->areasceneid = d["btn4_area_scene"].GetInt();
    action->globalsceneid = d["btn4_global_scene"].GetInt();
    this->button_actions.push_back(action);
    action = new button_action();
    action->function = d["btn5_function"].GetInt();
    action->intensity = d["btn5_intensity"].GetInt();
    action->fade_time = d["btn5_time"].GetInt();
    action->release_time = d["btn5_rel_time"].GetInt();
    action->system_options = d["btn5_system_options"].GetInt();
    action->areasceneid = d["btn5_area_scene"].GetInt();
    action->globalsceneid = d["btn5_global_scene"].GetInt();
    this->button_actions.push_back(action);
    action = new button_action();
    action->function = d["btn6_function"].GetInt();
    action->intensity = d["btn6_intensity"].GetInt();
    action->fade_time = d["btn6_time"].GetInt();
    action->release_time = d["btn6_rel_time"].GetInt();
    action->system_options = d["btn6_system_options"].GetInt();
    action->areasceneid = d["btn6_area_scene"].GetInt();
    action->globalsceneid = d["btn6_global_scene"].GetInt();
    this->button_actions.push_back(action);
    action = new button_action();
    action->function = d["btn7_function"].GetInt();
    action->intensity = d["btn7_intensity"].GetInt();
    action->fade_time = d["btn7_time"].GetInt();
    action->release_time = d["btn7_rel_time"].GetInt();
    action->system_options = d["btn7_system_options"].GetInt();
    action->areasceneid = d["btn7_area_scene"].GetInt();
    action->globalsceneid = d["btn7_global_scene"].GetInt();
    this->button_actions.push_back(action);
    action = new button_action();
    action->function = d["btn8_function"].GetInt();
    action->intensity = d["btn8_intensity"].GetInt();
    action->fade_time = d["btn8_time"].GetInt();
    action->release_time = d["btn8_rel_time"].GetInt();
    action->system_options = d["btn8_system_options"].GetInt();
    action->areasceneid = d["btn8_area_scene"].GetInt();
    action->globalsceneid = d["btn8_global_scene"].GetInt();
    this->button_actions.push_back(action);
    action = new button_action();
    action->function = d["btn9_function"].GetInt();
    action->intensity = d["btn9_intensity"].GetInt();
    action->fade_time = d["btn9_time"].GetInt();
    action->release_time = d["btn9_rel_time"].GetInt();
    action->system_options = d["btn9_system_options"].GetInt();
    action->areasceneid = d["btn9_area_scene"].GetInt();
    action->globalsceneid = d["btn9_global_scene"].GetInt();
    this->button_actions.push_back(action);
    action = new button_action();
    action->function = d["btn10_function"].GetInt();
    action->intensity = d["btn10_intensity"].GetInt();
    action->fade_time = d["btn10_time"].GetInt();
    action->release_time = d["btn10_rel_time"].GetInt();
    action->system_options = d["btn10_system_options"].GetInt();
    action->areasceneid = d["btn10_area_scene"].GetInt();
    action->globalsceneid = d["btn10_global_scene"].GetInt();
    this->button_actions.push_back(action);
    action = new button_action();
    action->function = d["btn11_function"].GetInt();
    action->intensity = d["btn11_intensity"].GetInt();
    action->fade_time = d["btn11_time"].GetInt();
    action->release_time = d["btn11_rel_time"].GetInt();
    action->system_options = d["btn11_system_options"].GetInt();
    action->areasceneid = d["btn11_area_scene"].GetInt();
    action->globalsceneid = d["btn11_global_scene"].GetInt();
    this->button_actions.push_back(action);

    if (isconverted == 1) {
        this->config_file = d["config_filename"].GetString();
        return;
    }
    int bp = 0;
    bp++;
    Tree_Structs::str_display_config config;
    char *header = (char*) "#MyConfig#";
    strcpy(config.config_header, header);
    config.version = d["version"].GetInt();
    config.area_id = d["area"].GetInt();
    config.max_int = d["max_int"].GetInt();
    config.max_touch_int = d["max_touch_int"].GetInt();
    config.min_int = d["min_int"].GetInt();
    config.min_touch_int = d["min_touch_int"].GetInt();
    config.off_touch_mode = d["off_touch_mode"].GetInt();
    config.off_long_press_mode = d["off_long_press_mode"].GetInt();
    //config.io_input = d["io_input"].GetInt();
    string hexclr = d["text_color"].GetString();
    hexclr.erase(hexclr.begin());
    int r, g, b;
    sscanf(hexclr.c_str(), "%02x%02x%02x", &r, &g, &b);
    config.text_r = r;
    config.text_g = g;
    config.text_b = b;
    hexclr = d["text_shadow_color"].GetString();
    hexclr.erase(hexclr.begin());
    sscanf(hexclr.c_str(), "%02x%02x%02x", &r, &g, &b);

    config.shadow_r = r;
    config.shadow_g = g;
    config.shadow_b = b;

    config.def_page = d["defaultscreen"].GetInt();
    config.dualpagemode = d["dualpagemode"].GetInt();
    config.io_input = d["io_input"].GetInt();
    uint32_t config_size = sizeof (Tree_Structs::str_display_config);
    uint32_t file_index = config_size;
    string tmp_file = "/var/www/html/ScreenConfigurator/Screen/" + uint32tostring(this->UID) + "_tmp.conf";
    remove(tmp_file.c_str());
    string filename = d["btn1_bin_background"].GetString();
    config.btn_1.offset = file_index;
    config.btn_1.size = this->add_button(filename);
    file_index = file_index + config.btn_1.size;
    remove(filename.c_str());
    filename = d["btn1_background"].GetString();
    remove(filename.c_str());
    filename = d["btn1_touch_bin_background"].GetString();
    config.btn_1_touch.offset = file_index;
    config.btn_1_touch.size = this->add_button(filename);
    file_index = file_index + config.btn_1_touch.size;
    remove(filename.c_str());
    filename = d["btn1_touch_background"].GetString();
    remove(filename.c_str());
    filename = d["btn1_active_bin_background"].GetString();
    config.btn_1_active.offset = file_index;
    config.btn_1_active.size = this->add_button(filename);
    file_index = file_index + config.btn_1_active.size;
    remove(filename.c_str());
    filename = d["btn1_active_background"].GetString();
    remove(filename.c_str());
    filename = d["btn1_bin_icon"].GetString();
    config.btn_1_icon.offset = file_index;
    config.btn_1_icon.size = this->add_button(filename);
    file_index = file_index + config.btn_1_icon.size;
    //remove(filename.c_str());
    string Header = d["btn1_header"].GetString();
    if (Header.size() >= 30) {
        memcpy(config.btn1_header, Header.c_str(), 30);
        config.btn1_header[31] = 0;
    } else {
        memcpy(config.btn1_header, Header.c_str(), Header.size());
        config.btn1_header[Header.size()] = 0;
    }

    string Footer = d["btn1_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn1_footer, Footer.c_str(), 30);
        config.btn1_footer[31] = 0;
    } else {
        memcpy(config.btn1_footer, Footer.c_str(), Footer.size());
        config.btn1_footer[Footer.size()] = 0;
    }

    config.btn1_function = d["btn1_function"].GetInt();
    //BTN2
    filename = d["btn2_bin_background"].GetString();
    config.btn_2.offset = file_index;
    config.btn_2.size = this->add_button(filename);
    file_index = file_index + config.btn_2.size;
    remove(filename.c_str());
    filename = d["btn2_background"].GetString();
    remove(filename.c_str());
    filename = d["btn2_touch_bin_background"].GetString();
    config.btn_2_touch.offset = file_index;
    config.btn_2_touch.size = this->add_button(filename);
    file_index = file_index + config.btn_2_touch.size;
    remove(filename.c_str());
    filename = d["btn2_touch_background"].GetString();
    remove(filename.c_str());
    filename = d["btn2_active_bin_background"].GetString();
    config.btn_2_active.offset = file_index;
    config.btn_2_active.size = this->add_button(filename);
    file_index = file_index + config.btn_2_active.size;
    remove(filename.c_str());
    filename = d["btn2_active_background"].GetString();
    remove(filename.c_str());
    filename = d["btn2_bin_icon"].GetString();
    config.btn_2_icon.offset = file_index;
    config.btn_2_icon.size = this->add_button(filename);
    file_index = file_index + config.btn_2_icon.size;
    // remove(filename.c_str());
    Header = d["btn2_header"].GetString();
    if (Header.size() >= 31) {
        memcpy(config.btn2_header, Header.c_str(), 31);
        config.btn2_header[31] = 0;
    } else {
        memcpy(config.btn2_header, Header.c_str(), Header.size());
        config.btn2_header[Header.size()] = 0;
    }
    Footer = d["btn2_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn2_footer, Footer.c_str(), 31);
        config.btn2_footer[31] = 0;
    } else {
        memcpy(config.btn2_footer, Footer.c_str(), Footer.size());
        config.btn2_footer[Footer.size()] = 0;
    }
    config.btn2_function = d["btn2_function"].GetInt();
    //BTN3
    filename = d["btn3_bin_background"].GetString();
    config.btn_3.offset = file_index;
    config.btn_3.size = this->add_button(filename);
    file_index = file_index + config.btn_3.size;
    remove(filename.c_str());
    filename = d["btn3_background"].GetString();
    remove(filename.c_str());
    filename = d["btn3_touch_bin_background"].GetString();
    config.btn_3_touch.offset = file_index;
    config.btn_3_touch.size = this->add_button(filename);
    file_index = file_index + config.btn_3_touch.size;
    remove(filename.c_str());
    filename = d["btn3_touch_background"].GetString();
    remove(filename.c_str());
    filename = d["btn3_active_bin_background"].GetString();
    config.btn_3_active.offset = file_index;
    config.btn_3_active.size = this->add_button(filename);
    file_index = file_index + config.btn_3_active.size;
    remove(filename.c_str());
    filename = d["btn3_active_background"].GetString();
    remove(filename.c_str());
    filename = d["btn3_bin_icon"].GetString();
    config.btn_3_icon.offset = file_index;
    config.btn_3_icon.size = this->add_button(filename);
    file_index = file_index + config.btn_3_icon.size;
    //remove(filename.c_str());
    Header = d["btn3_header"].GetString();
    if (Header.size() >= 31) {
        memcpy(config.btn3_header, Header.c_str(), 31);
        config.btn3_header[31] = 0;
    } else {
        memcpy(config.btn3_header, Header.c_str(), Header.size());
        config.btn3_header[Header.size()] = 0;
    }
    Footer = d["btn3_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn3_footer, Footer.c_str(), 31);
        config.btn3_footer[31] = 0;
    } else {
        memcpy(config.btn3_footer, Footer.c_str(), Footer.size());
        config.btn3_footer[Footer.size()] = 0;
    }
    config.btn3_function = d["btn3_function"].GetInt();
    //BTN4
    filename = d["btn4_bin_background"].GetString();
    config.btn_4.offset = file_index;
    config.btn_4.size = this->add_button(filename);
    file_index = file_index + config.btn_4.size;
    remove(filename.c_str());
    filename = d["btn4_background"].GetString();
    remove(filename.c_str());
    filename = d["btn4_touch_bin_background"].GetString();
    config.btn_4_touch.offset = file_index;
    config.btn_4_touch.size = this->add_button(filename);
    file_index = file_index + config.btn_4_touch.size;
    remove(filename.c_str());
    filename = d["btn4_touch_background"].GetString();
    remove(filename.c_str());
    filename = d["btn4_active_bin_background"].GetString();
    config.btn_4_active.offset = file_index;
    config.btn_4_active.size = this->add_button(filename);
    file_index = file_index + config.btn_4_active.size;
    remove(filename.c_str());
    filename = d["btn4_active_background"].GetString();
    remove(filename.c_str());
    filename = d["btn4_bin_icon"].GetString();
    config.btn_4_icon.offset = file_index;
    config.btn_4_icon.size = this->add_button(filename);
    file_index = file_index + config.btn_4_icon.size;
    //remove(filename.c_str());
    Header = d["btn4_header"].GetString();
    if (Header.size() >= 31) {
        memcpy(config.btn4_header, Header.c_str(), 31);
        config.btn4_header[31] = 0;
    } else {
        memcpy(config.btn4_header, Header.c_str(), Header.size());
        config.btn4_header[Header.size()] = 0;
    }
    Footer = d["btn4_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn4_footer, Footer.c_str(), 31);
        config.btn4_footer[31] = 0;
    } else {
        memcpy(config.btn4_footer, Footer.c_str(), Footer.size());
        config.btn4_footer[Footer.size()] = 0;
    }
    config.btn4_function = d["btn4_function"].GetInt();
    //BTN5
    filename = d["btn5_bin_background"].GetString();
    config.btn_5.offset = file_index;
    config.btn_5.size = this->add_button(filename);
    file_index = file_index + config.btn_5.size;
    remove(filename.c_str());
    filename = d["btn5_background"].GetString();
    remove(filename.c_str());
    filename = d["btn5_touch_bin_background"].GetString();
    config.btn_5_touch.offset = file_index;
    config.btn_5_touch.size = this->add_button(filename);
    file_index = file_index + config.btn_5_touch.size;
    remove(filename.c_str());
    filename = d["btn5_touch_background"].GetString();
    remove(filename.c_str());
    filename = d["btn5_active_bin_background"].GetString();
    config.btn_5_active.offset = file_index;
    config.btn_5_active.size = this->add_button(filename);
    file_index = file_index + config.btn_5_active.size;
    remove(filename.c_str());
    filename = d["btn5_active_background"].GetString();
    remove(filename.c_str());
    filename = d["btn5_bin_icon"].GetString();
    config.btn_5_icon.offset = file_index;
    config.btn_5_icon.size = this->add_button(filename);
    file_index = file_index + config.btn_5_icon.size;
    //remove(filename.c_str());
    Header = d["btn5_header"].GetString();
    if (Header.size() >= 31) {
        memcpy(config.btn5_header, Header.c_str(), 31);
        config.btn5_header[31] = 0;
    } else {
        memcpy(config.btn5_header, Header.c_str(), Header.size());
        config.btn5_header[Header.size()] = 0;
    }
    Footer = d["btn5_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn5_footer, Footer.c_str(), 31);
        config.btn5_footer[31] = 0;
    } else {
        memcpy(config.btn5_footer, Footer.c_str(), Footer.size());
        config.btn5_footer[Footer.size()] = 0;
    }
    config.btn5_function = d["btn5_function"].GetInt();

    //BTN6
    filename = d["btn6_bin_background"].GetString();
    config.btn_6.offset = file_index;
    config.btn_6.size = this->add_button(filename);
    file_index = file_index + config.btn_6.size;
    remove(filename.c_str());
    filename = d["btn6_background"].GetString();
    remove(filename.c_str());
    filename = d["btn6_touch_bin_background"].GetString();
    config.btn_6_touch.offset = file_index;
    config.btn_6_touch.size = this->add_button(filename);
    file_index = file_index + config.btn_6_touch.size;
    remove(filename.c_str());
    filename = d["btn6_touch_background"].GetString();
    remove(filename.c_str());
    filename = d["btn6_active_bin_background"].GetString();
    config.btn_6_active.offset = file_index;
    config.btn_6_active.size = this->add_button(filename);
    file_index = file_index + config.btn_6_active.size;
    remove(filename.c_str());
    filename = d["btn6_active_background"].GetString();
    remove(filename.c_str());
    filename = d["btn6_bin_icon"].GetString();
    config.btn_6_icon.offset = file_index;
    config.btn_6_icon.size = this->add_button(filename);
    file_index = file_index + config.btn_6_icon.size;
    //remove(filename.c_str());
    Header = d["btn6_header"].GetString();
    if (Header.size() >= 31) {
        memcpy(config.btn6_header, Header.c_str(), 31);
        config.btn6_header[31] = 0;
    } else {
        memcpy(config.btn6_header, Header.c_str(), Header.size());
        config.btn6_header[Header.size()] = 0;
    }
    Footer = d["btn6_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn6_footer, Footer.c_str(), 31);
        config.btn6_footer[31] = 0;
    } else {
        memcpy(config.btn6_footer, Footer.c_str(), Footer.size());
        config.btn6_footer[Footer.size()] = 0;
    }
    config.btn6_function = d["btn6_function"].GetInt();

    //BTN7
    filename = d["btn7_bin_background"].GetString();
    config.btn_7.offset = file_index;
    config.btn_7.size = this->add_button(filename);
    file_index = file_index + config.btn_7.size;
    remove(filename.c_str());
    filename = d["btn7_background"].GetString();
    remove(filename.c_str());
    filename = d["btn7_touch_bin_background"].GetString();
    config.btn_7_touch.offset = file_index;
    config.btn_7_touch.size = this->add_button(filename);
    file_index = file_index + config.btn_7_touch.size;
    remove(filename.c_str());
    filename = d["btn7_touch_background"].GetString();
    remove(filename.c_str());
    filename = d["btn7_active_bin_background"].GetString();
    config.btn_7_active.offset = file_index;
    config.btn_7_active.size = this->add_button(filename);
    file_index = file_index + config.btn_7_active.size;
    remove(filename.c_str());
    filename = d["btn7_active_background"].GetString();
    remove(filename.c_str());
    filename = d["btn7_bin_icon"].GetString();
    config.btn_7_icon.offset = file_index;
    config.btn_7_icon.size = this->add_button(filename);
    file_index = file_index + config.btn_7_icon.size;
    //remove(filename.c_str());
    Header = d["btn7_header"].GetString();
    if (Header.size() >= 31) {
        memcpy(config.btn7_header, Header.c_str(), 31);
        config.btn7_header[31] = 0;
    } else {
        memcpy(config.btn7_header, Header.c_str(), Header.size());
        config.btn7_footer[Footer.size()] = 0;
    }
    Footer = d["btn7_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn7_footer, Footer.c_str(), 31);
        config.btn7_footer[31] = 0;
    } else {
        memcpy(config.btn7_footer, Footer.c_str(), Footer.size());
        config.btn7_header[Header.size()] = 0;
    }
    config.btn7_function = d["btn7_function"].GetInt();

    //BTN8
    filename = d["btn8_bin_background"].GetString();
    config.btn_8.offset = file_index;
    config.btn_8.size = this->add_button(filename);
    file_index = file_index + config.btn_8.size;
    remove(filename.c_str());
    filename = d["btn8_background"].GetString();
    remove(filename.c_str());
    filename = d["btn8_touch_bin_background"].GetString();
    config.btn_8_touch.offset = file_index;
    config.btn_8_touch.size = this->add_button(filename);
    file_index = file_index + config.btn_8_touch.size;
    remove(filename.c_str());
    filename = d["btn8_touch_background"].GetString();
    remove(filename.c_str());
    filename = d["btn8_active_bin_background"].GetString();
    config.btn_8_active.offset = file_index;
    config.btn_8_active.size = this->add_button(filename);
    file_index = file_index + config.btn_8_active.size;
    remove(filename.c_str());
    filename = d["btn8_active_background"].GetString();
    remove(filename.c_str());
    filename = d["btn8_bin_icon"].GetString();
    config.btn_8_icon.offset = file_index;
    config.btn_8_icon.size = this->add_button(filename);
    file_index = file_index + config.btn_8_icon.size;
    //remove(filename.c_str());
    Header = d["btn8_header"].GetString();
    if (Header.size() >= 31) {
        memcpy(config.btn8_header, Header.c_str(), 31);
        config.btn8_header[31] = 0;
    } else {
        memcpy(config.btn8_header, Header.c_str(), Header.size());
        config.btn8_header[Header.size()] = 0;
    }
    Footer = d["btn8_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn8_footer, Footer.c_str(), 31);
        config.btn8_footer[31] = 0;
    } else {
        memcpy(config.btn8_footer, Footer.c_str(), Footer.size());
        config.btn8_footer[Footer.size()] = 0;
    }
    config.btn8_function = d["btn8_function"].GetInt();

    //BTN9
    filename = d["btn9_bin_background"].GetString();
    config.btn_9.offset = file_index;
    config.btn_9.size = this->add_button(filename);
    file_index = file_index + config.btn_9.size;
    remove(filename.c_str());
    filename = d["btn9_background"].GetString();
    remove(filename.c_str());
    filename = d["btn9_touch_bin_background"].GetString();
    config.btn_9_touch.offset = file_index;
    config.btn_9_touch.size = this->add_button(filename);
    file_index = file_index + config.btn_9_touch.size;
    remove(filename.c_str());
    filename = d["btn9_touch_background"].GetString();
    remove(filename.c_str());
    filename = d["btn9_active_bin_background"].GetString();
    config.btn_9_active.offset = file_index;
    config.btn_9_active.size = this->add_button(filename);
    file_index = file_index + config.btn_9_active.size;
    remove(filename.c_str());
    filename = d["btn9_active_background"].GetString();
    remove(filename.c_str());
    filename = d["btn9_bin_icon"].GetString();
    config.btn_9_icon.offset = file_index;
    config.btn_9_icon.size = this->add_button(filename);
    file_index = file_index + config.btn_9_icon.size;
    //remove(filename.c_str());
    Header = d["btn9_header"].GetString();
    if (Header.size() >= 31) {
        memcpy(config.btn9_header, Header.c_str(), 31);
        config.btn9_header[31] = 0;
    } else {
        memcpy(config.btn9_header, Header.c_str(), Header.size());
        config.btn9_header[Header.size()] = 0;
    }
    Footer = d["btn9_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn9_footer, Footer.c_str(), 31);
        config.btn9_footer[31] = 0;
    } else {
        memcpy(config.btn9_footer, Footer.c_str(), Footer.size());
        config.btn9_footer[Footer.size()] = 0;
    }
    config.btn9_function = d["btn9_function"].GetInt();

    //BTN10
    filename = d["btn10_bin_background"].GetString();
    config.btn_10.offset = file_index;
    config.btn_10.size = this->add_button(filename);
    file_index = file_index + config.btn_10.size;
    remove(filename.c_str());
    filename = d["btn10_background"].GetString();
    remove(filename.c_str());
    filename = d["btn10_touch_bin_background"].GetString();
    config.btn_10_touch.offset = file_index;
    config.btn_10_touch.size = this->add_button(filename);
    file_index = file_index + config.btn_10_touch.size;
    remove(filename.c_str());
    filename = d["btn10_touch_background"].GetString();
    remove(filename.c_str());
    filename = d["btn10_active_bin_background"].GetString();
    config.btn_10_active.offset = file_index;
    config.btn_10_active.size = this->add_button(filename);
    file_index = file_index + config.btn_10_active.size;
    remove(filename.c_str());
    filename = d["btn10_active_background"].GetString();
    remove(filename.c_str());
    filename = d["btn10_bin_icon"].GetString();
    config.btn_10_icon.offset = file_index;
    config.btn_10_icon.size = this->add_button(filename);
    file_index = file_index + config.btn_10_icon.size;
    //remove(filename.c_str());
    Header = d["btn10_header"].GetString();
    if (Header.size() >= 31) {
        memcpy(config.btn10_header, Header.c_str(), 31);
        config.btn10_header[31] = 0;
    } else {
        memcpy(config.btn10_header, Header.c_str(), Header.size());
        config.btn10_header[Header.size()] = 0;
    }
    Footer = d["btn10_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn10_footer, Footer.c_str(), 31);
        config.btn10_footer[31] = 0;
    } else {
        memcpy(config.btn10_footer, Footer.c_str(), Footer.size());
        config.btn10_footer[Footer.size()] = 0;
    }
    config.btn10_function = d["btn10_function"].GetInt();

    //BTN11
    filename = d["btn11_bin_background"].GetString();
    config.btn_11.offset = file_index;
    config.btn_11.size = this->add_button(filename);
    file_index = file_index + config.btn_11.size;
    remove(filename.c_str());
    filename = d["btn11_background"].GetString();
    remove(filename.c_str());
    filename = d["btn11_touch_bin_background"].GetString();
    config.btn_11_touch.offset = file_index;
    config.btn_11_touch.size = this->add_button(filename);
    file_index = file_index + config.btn_11_touch.size;
    remove(filename.c_str());
    filename = d["btn11_touch_background"].GetString();
    remove(filename.c_str());
    filename = d["btn11_active_bin_background"].GetString();
    config.btn_11_active.offset = file_index;
    config.btn_11_active.size = this->add_button(filename);
    file_index = file_index + config.btn_11_active.size;
    remove(filename.c_str());
    filename = d["btn11_active_background"].GetString();
    remove(filename.c_str());
    filename = d["btn11_bin_icon"].GetString();
    config.btn_11_icon.offset = file_index;
    config.btn_11_icon.size = this->add_button(filename);
    file_index = file_index + config.btn_11_icon.size;
    //remove(filename.c_str());
    Header = d["btn11_header"].GetString();
    if (Header.size() >= 31) {
        memcpy(config.btn11_header, Header.c_str(), 31);
        config.btn11_header[31] = 0;
    } else {
        memcpy(config.btn11_header, Header.c_str(), Header.size());
        config.btn11_header[Header.size()] = 0;
    }
    Footer = d["btn11_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn11_footer, Footer.c_str(), 31);
        config.btn11_footer[31] = 0;
    } else {
        memcpy(config.btn11_footer, Footer.c_str(), Footer.size());
        config.btn11_footer[Footer.size()] = 0;
    }
    config.btn11_function = d["btn11_function"].GetInt();



    //system
    filename = d["co2icon"].GetString();
    config.co2_icon.offset = file_index;
    config.co2_icon.size = this->add_button(filename);
    file_index = file_index + config.co2_icon.size;
    remove(filename.c_str());
    filename = d["co2dial"].GetString();
    config.iaq_dial.offset = file_index;
    config.iaq_dial.size = this->add_button(filename);
    file_index = file_index + config.iaq_dial.size;
    remove(filename.c_str());
    filename = d["tempdial"].GetString();
    config.temp_dial.offset = file_index;
    config.temp_dial.size = this->add_button(filename);
    file_index = file_index + config.temp_dial.size;
    remove(filename.c_str());
    filename = d["flameon"].GetString();
    config.flame_on.offset = file_index;
    config.flame_on.size = this->add_button(filename);
    file_index = file_index + config.flame_on.size;
    remove(filename.c_str());
    filename = d["flameoff"].GetString();
    config.flame_off.offset = file_index;
    config.flame_off.size = this->add_button(filename);
    file_index = file_index + config.flame_off.size;
    remove(filename.c_str());
    filename = d["hum_icon"].GetString();
    config.hum_icon.offset = file_index;
    config.hum_icon.size = this->add_button(filename);
    file_index = file_index + config.hum_icon.size;
    remove(filename.c_str());
    filename = d["hum_dial"].GetString();
    config.hum_dial.offset = file_index;
    config.hum_dial.size = this->add_button(filename);
    file_index = file_index + config.hum_dial.size;
    remove(filename.c_str());
    filename = d["slider_icon"].GetString();
    config.slider_icon.offset = file_index;
    config.slider_icon.size = this->add_button(filename);
    file_index = file_index + config.slider_icon.size;
    remove(filename.c_str());
    filename = d["settings_icon"].GetString();
    config.settings_icon.offset = file_index;
    config.settings_icon.size = this->add_button(filename);
    file_index = file_index + config.settings_icon.size;
    remove(filename.c_str());
    filename = d["def_background"].GetString();
    config.def_background.offset = file_index;
    config.def_background.size = this->add_button(filename);
    file_index = file_index + config.def_background.size;
    remove(filename.c_str());
    filename = d["close_icon"].GetString();
    config.close_icon.offset = file_index;
    config.close_icon.size = this->add_button(filename);
    file_index = file_index + config.close_icon.size;
    remove(filename.c_str());
    filename = d["arrow_left"].GetString();
    config.arrow_left.offset = file_index;
    config.arrow_left.size = this->add_button(filename);
    file_index = file_index + config.arrow_left.size;
    remove(filename.c_str());
    filename = d["arrow_right"].GetString();
    config.arrow_right.offset = file_index;
    config.arrow_right.size = this->add_button(filename);
    file_index = file_index + config.arrow_right.size;
    remove(filename.c_str());
    filename = d["warning_icon"].GetString();
    config.warning_icon.offset = file_index;
    config.warning_icon.size = this->add_button(filename);
    file_index = file_index + config.warning_icon.size;
    remove(filename.c_str());





    /*
    config.btn_2.offset = file_index;
    config.btn_2.size = this->add_button(file);
    file_index = file_index + config.btn_2.size;
     file = "/var/www/html/ScreenConfigurator/Screen/Default_btn_2_touch.png.bin"; 
    config.btn_2_touch.offset = file_index;
    config.btn_2_touch.size = this->add_button(file);
    file_index = file_index + config.btn_2_touch.size;
    file = "/var/www/html/ScreenConfigurator/Screen/Default_btn_3.png.bin"; 
    config.btn_3.offset = file_index;
    config.btn_3.size = this->add_button(file);
    file_index = file_index + config.btn_3.size;
     file = "/var/www/html/ScreenConfigurator/Screen/Default_btn_3_touch.png.bin"; 
    config.btn_3_touch.offset = file_index;
    config.btn_3_touch.size = this->add_button(file);
    file_index = file_index + config.btn_3_touch.size;
    file = "/var/www/html/ScreenConfigurator/Screen/Default_btn_4.png.bin"; 
    config.btn_4.offset = file_index;
    config.btn_4.size = this->add_button(file);
    file_index = file_index + config.btn_4.size;
     file = "/var/www/html/ScreenConfigurator/Screen/Default_btn_4_touch.png.bin"; 
    config.btn_4_touch.offset = file_index;
    config.btn_4_touch.size = this->add_button(file);
    file_index = file_index + config.btn_4_touch.size;
    file = "/var/www/html/ScreenConfigurator/Screen/Default_btn_5.png.bin"; 
    config.btn_5.offset = file_index;
    config.btn_5.size = this->add_button(file);
    file_index = file_index + config.btn_5.size;
    file = "/var/www/html/ScreenConfigurator/Screen/Default_btn_5_touch.png.bin"; 
    config.btn_5_touch.offset = file_index;
    config.btn_5_touch.size = this->add_button(file);
    file_index = file_index + config.btn_5_touch.size;
    file = "/var/www/html/ScreenConfigurator/Screen/Wifi_nok_0.png.bin"; 
    config.wifi_nok_0.offset = file_index;
    config.wifi_nok_0.size = this->add_button(file);
    file_index = file_index + config.wifi_nok_0.size;
    file = "/var/www/html/ScreenConfigurator/Screen/Wifi_nok_1.png.bin"; 
    config.wifi_nok_1.offset = file_index;
    config.wifi_nok_1.size = this->add_button(file);
    file_index = file_index + config.wifi_nok_1.size;
    file = "/var/www/html/ScreenConfigurator/Screen/Wifi_nok_2.png.bin"; 
    config.wifi_nok_2.offset = file_index;
    config.wifi_nok_2.size = this->add_button(file);
    file_index = file_index + config.wifi_nok_2.size;
    file = "/var/www/html/ScreenConfigurator/Screen/Wifi_nok_3.png.bin"; 
    config.wifi_nok_3.offset = file_index;
    config.wifi_nok_3.size = this->add_button(file);
    file_index = file_index + config.wifi_nok_3.size;
    file = "/var/www/html/ScreenConfigurator/Screen/Wifi_nok_4.png.bin"; 
    config.wifi_nok_4.offset = file_index;
    config.wifi_nok_4.size = this->add_button(file);
    file_index = file_index + config.wifi_nok_4.size;
    file = "/var/www/html/ScreenConfigurator/Screen/Wifi_ok.png.bin"; 
    config.wifi_ok.offset = file_index;
    config.wifi_ok.size = this->add_button(file);
    file_index = file_index + config.wifi_ok.size;
    file = "/var/www/html/ScreenConfigurator/Screen/FlameIcon_Off.png.bin"; 
    config.flame_off.offset = file_index;
    config.flame_off.size = this->add_button(file);
    file_index = file_index + config.flame_off.size;
     file = "/var/www/html/ScreenConfigurator/Screen/FlameIcon_On.png.bin"; 
    config.flame_on.offset = file_index;
    config.flame_on.size = this->add_button(file);
    file_index = file_index + config.flame_on.size;
     file = "/var/www/html/ScreenConfigurator/Screen/hum_icon.png.bin"; 
    config.hum_icon.offset = file_index;
    config.hum_icon.size = this->add_button(file);
    file_index = file_index + config.hum_icon.size;
        file = "/var/www/html/ScreenConfigurator/Screen/hum2.png.bin"; 
    config.hum_dial.offset = file_index;
    config.hum_dial.size = this->add_button(file);
    file_index = file_index + config.hum_dial.size;
        file = "/var/www/html/ScreenConfigurator/Screen/IAQ.png.bin"; 
    config.iaq_dial.offset = file_index;
    config.iaq_dial.size = this->add_button(file);
    file_index = file_index + config.iaq_dial.size;
      file = "/var/www/html/ScreenConfigurator/Screen/co2.png.bin"; 
    config.co2_icon.offset = file_index;
    config.co2_icon.size = this->add_button(file);
    file_index = file_index + config.co2_icon.size;
     file = "/var/www/html/ScreenConfigurator/Screen/grad_1.png.bin"; 
    config.temp_dial.offset = file_index;
    config.temp_dial.size = this->add_button(file);
    file_index = file_index + config.temp_dial.size;
       file = "/var/www/html/ScreenConfigurator/Screen/SliderIcon.png.bin"; 
    config.slider_icon.offset = file_index;
    config.slider_icon.size = this->add_button(file);
    file_index = file_index + config.slider_icon.size;
     */



    string final_file = "/var/www/html/ScreenConfigurator/Screen/" + uint32tostring(this->UID) + ".conf";
    ofstream finalfile(final_file);
    if (finalfile.good()) {
        char buffer[sizeof (Tree_Structs::str_display_config)];
        memcpy(buffer, &config, sizeof (Tree_Structs::str_display_config));
        finalfile.write(buffer, sizeof (Tree_Structs::str_display_config));
        finalfile.close();
    }
    ifstream tmpfile(tmp_file);
    if (tmpfile.good()) {
        //get length of file
        std::vector<char> buffer;
        tmpfile.seekg(0, tmpfile.end);
        size_t length = tmpfile.tellg();
        tmpfile.seekg(0, tmpfile.beg);
        if (length > 0) {
            buffer.resize(length);
            tmpfile.read(&buffer[0], length);
        }
        tmpfile.close();
        finalfile.open(final_file, std::ios::out | std::ios::app);
        if (finalfile.good()) {
            finalfile.write(buffer.data(), length);
            finalfile.close();
        }
    }
    this->config_file = final_file;
    d["config_filename"].SetString(final_file.c_str(), final_file.size(), d.GetAllocator());
    d["converted"].SetInt(1);
    StringBuffer strbuf;
    strbuf.Clear();

    Writer<StringBuffer> writer(strbuf);
    d.Accept(writer);
    string jsonstring = strbuf.GetString();
    cout << jsonstring << endl;
    ofstream jsonwrite(jsonfilename);
    if (jsonwrite.good()) {

        jsonwrite.write(jsonstring.c_str(), jsonstring.size());
        jsonwrite.close();
    }
    remove(tmp_file.c_str());

};

uint32_t Tree_Node::add_button(string fileloc) {
    uint32_t filesize = 0;
    string tmp_file = "/var/www/html/ScreenConfigurator/Screen/" + uint32tostring(this->UID) + "_tmp.conf";
    std::ifstream infile;
    infile.open(fileloc);
    if (infile.good()) {
        std::vector<char> buffer;

        //get length of file
        infile.seekg(0, infile.end);
        size_t length = infile.tellg();
        infile.seekg(0, infile.beg);

        //read file
        if (length > 0) {
            buffer.resize(length);
            infile.read(&buffer[0], length);
        }
        infile.close();
        filesize = length;
        ofstream tmpfile(tmp_file, std::ios::out | std::ios::app);
        if (tmpfile.good()) {
            tmpfile.write(buffer.data(), length);
            tmpfile.close();
        }
    }
    return filesize;

}

uint32_t Tree_Node::get_msg_id() {

    return this->reftcp->get_msg_id();

};

void Tree_Node::send_ack(uint32_t to, uint32_t msg_id) {
    Tree_Message *m = new Tree_Message(true, to, msg_types::ack, msg_id);
    this->reftcp->send_message(m);
    //m->send_msg(this->reftcp);
    //delete m;
}

void Tree_Node::set_online(bool online) {
    // m.lock();
    this->online = online;
    //  m.unlock();
}

void Tree_Node::upload_file(string filename, uint64_t index) {
    if (index > 0) {
        int bp = 0;
        bp++;
    }
    string file = "/var/www/html/hmi/" + filename;
    std::ifstream infile(file);
    if (infile.good()) {
        std::vector<char> buffer;

        //get length of file
        infile.seekg(0, infile.end);
        size_t length = infile.tellg();
        infile.seekg(0, infile.beg);

        //read file
        if (length > 0) {
            buffer.resize(length);
            infile.read(&buffer[0], length);
        }
        infile.close();
        Tree_Structs::str_File_Response response;

        strcpy(response.FileName, filename.c_str());
        response.FileSize = length;
        response.Index = index;
        uint16_t msg_size = 0;
        for (int i = index; i < length; i++) {
            response.data[msg_size] = buffer[i];
            msg_size++;
            if (msg_size == 512) {
                break;
            }
        }
        response.Data_Length = msg_size;
        response.exist = true;
        char data[sizeof response];
        memcpy(data, &response, sizeof response);
        Tree_Message *m_ret = new Tree_Message(this, this->UID, msg_types::File_Response, this->get_msg_id(), sizeof response, data);
        this->reftcp->send_message(m_ret);
    } else {
        Tree_Structs::str_File_Response response;
        strcpy(response.FileName, filename.c_str());
        response.Index = index;
        response.Data_Length = 0;
        response.exist = false;
        char data[sizeof response];
        memcpy(data, &response, sizeof response);
        Tree_Message *m_ret = new Tree_Message(this, this->UID, msg_types::File_Response, this->get_msg_id(), sizeof response, data);
        this->reftcp->send_message(m_ret);
    }
};

void Tree_Node::announce_config_update() {
    this->config_update_running = timeSinceEpochMillisec();
    std::ifstream infile(this->config_file);
    if (infile.good()) {
        std::vector<char> buffer;

        //get length of file
        infile.seekg(0, infile.end);
        size_t length = infile.tellg();
        infile.seekg(0, infile.beg);

        //read file
        if (length > 0) {
            buffer.resize(length);
            infile.read(&buffer[0], length);
        }
        infile.close();
        Tree_Structs::str_Config_Announce response;
        response.FileSize = length;
        response.config_version = this->config_version;
        char data[sizeof (response)];
        memcpy(data, &response, sizeof (response));
        Tree_Message *m = new Tree_Message(this, this->UID, msg_types::Config_Anounce, this->get_msg_id(), sizeof (response), data);
        this->reftcp->send_message(m);
    }
}

void Tree_Node::upload_config(uint64_t index) {
    this->config_update_running = timeSinceEpochMillisec();
    string file = this->config_file;
    std::ifstream infile(file);
    if (infile.good()) {
        std::vector<char> buffer;

        //get length of file
        infile.seekg(0, infile.end);
        size_t length = infile.tellg();
        infile.seekg(0, infile.beg);

        //read file
        if (length > 0) {
            buffer.resize(length);
            infile.read(&buffer[0], length);
        }
        infile.close();
        Tree_Structs::str_Config_Response response;
        response.FileSize = length;
        response.Index = index;
        uint16_t msg_size = 0;
        for (int i = index; i < length; i++) {
            response.data[msg_size] = buffer[i];
            msg_size++;
            if (msg_size == sizeof (response.data)) {
                break;
            }
        }
        response.Data_Length = msg_size;
        //        response.exist = true;
        char data[sizeof response];
        memcpy(data, &response, sizeof response);
        Tree_Message *m_ret = new Tree_Message(this, this->UID, msg_types::Config_Response, this->get_msg_id(), sizeof response, data);
        this->reftcp->send_message(m_ret);
    } else {
        Tree_Structs::str_File_Response response;
        response.Index = index;
        response.Data_Length = 0;
        response.exist = false;
        char data[sizeof response];
        memcpy(data, &response, sizeof response);
        Tree_Message *m_ret = new Tree_Message(this, this->UID, msg_types::Config_Response, this->get_msg_id(), sizeof response, data);
        this->reftcp->send_message(m_ret);
    }
};

void Tree_Node::reset_sensor_log() {
    this->s_log.uptime = 0;
    this->s_log.static_iaq = 0;
    this->s_log.iaq_accuracy = 0;
    this->s_log.raw_temperature = 0;
    this->s_log.comp_temperature = 0;
    this->s_log.raw_humidity = 0;
    this->s_log.comp_humidity = 0;
    this->s_log.co2 = 0;
    this->s_log.pressure = 0;
    this->s_log.lux = 0;
    this->s_log.motion = 0;
    this->s_log.internal_temperature = 0;
    this->s_log.cpu_0_res_reason = 0;
    this->s_log.cpu_1_res_reason = 0;
}

void Tree_Node::save_sensor_log() {
    time_t nu = time(0);
    if (nu - s_log.last_save > SAVE_INTERVAL) {
        s_log.last_save = nu;
        MYSQL *ms = Mysql_Connect();
        if (ms != NULL) {
            string query = "INSERT INTO Tree_Node_Sensor_Log(NodeID, uptime, static_iaq, iaq_accuracy, raw_temperature, comp_temperature, raw_humidity, comp_humidity, co2, pressure, lux, motion, internal_temp, cpu_0_res_reason, cpu_1_res_reason) VALUES (" + uint32tostring(this->UID) + "," + longtostring(s_log.uptime) + ", " + floattostring(s_log.static_iaq) + "," + inttostring(s_log.iaq_accuracy) + ", " + floattostring(s_log.raw_temperature) + "," + floattostring(s_log.comp_temperature) + ", " + floattostring(s_log.raw_humidity) + "," + floattostring(s_log.comp_humidity) + "," + floattostring(s_log.co2) + "," + floattostring(s_log.pressure) + "," + inttostring(s_log.lux) + "," + inttostring(s_log.motion) + ", " + floattostring(s_log.internal_temperature) + "," + inttostring(s_log.cpu_0_res_reason) + "," + inttostring(s_log.cpu_1_res_reason) + ")";

            if (!mysql_query(ms, query.c_str())) {

            } else {
                string ms_err = string(mysql_error(ms));
                cout << "MYSQL Error " << ms_err << endl;
            }
            Mysql_Disconnect(ms);
        }
        this->reset_sensor_log();
    }

}

void Tree_Node::handle_buttons(int buttonno) {
    cout << "Handle button no " << buttonno << endl;
    if (buttonno == 0) {
        //special button
        Area *a = this->refmaster->areas->get_area(this->area_id);
        if (a != NULL) {
            a->set_brightness(100, 0, 0, "Mesh btn 0");
        }
        return;
        return;
    }
    buttonno = buttonno - 1;
    if (buttonno >= this->button_actions.size()) {
        return;
    }
    button_action *btn_action = this->button_actions[buttonno];
    if (btn_action->function == 0) {
        // system actions
        return;
    }

    if (btn_action->function == 1) {
        //area intensity
        Area *a = this->refmaster->areas->get_area(this->area_id);
        if (a != NULL) {
            a->set_brightness(btn_action->intensity, btn_action->fade_time, btn_action->release_time * 60, "Mesh");
        }
        return;
    }
    if (btn_action->function == 2) {
        Area *a = this->refmaster->areas->get_area(this->area_id);
        if (a != NULL) {
            a->Toggle_Scene(btn_action->areasceneid, btn_action->fade_time * 1000, btn_action->release_time * 60 * 1000);
        }
        return;
    }
    if (btn_action->function == 3) {
        this->refmaster->macros->toggle_macro(btn_action->globalsceneid);
    }

}

Value Tree_Node::GetJsonValue(Document::AllocatorType & allocator) {
    Value obj;
    obj.SetObject();
    m.lock();
    obj.AddMember("ID", this->UID, allocator);
    Value name;
    name.SetString(this->name.c_str(), allocator);
    obj.AddMember("name", name, allocator);
    obj.AddMember("Type", this->nodetype, allocator);
    obj.AddMember("Uptime", this->uptime, allocator);
    obj.AddMember("Layer", this->layer, allocator);
    obj.AddMember("Online", this->online, allocator);
    obj.AddMember("Firmware", this->firmware, allocator);
    obj.AddMember("Temp_Calibrated", this->temp_calibrated, allocator);
    m.unlock();
    return obj;

}

void Tree_Node::Ident() {
    Tree_Message *m = new Tree_Message(this, this->UID, msg_types::Ident, this->get_msg_id(), 0, 0);
    this->reftcp->send_message(m);
}

void Tree_Node::Get_Button_State() {
    uint32_t cur_state = 0;
    int index = this->button_actions.size();
    while (index > 0) {
        index = index - 1;
        if (this->button_actions[index]->function == 2) {
            Area *a = this->refmaster->areas->get_area(this->area_id);
            if (a != NULL) {
                if (a->Get_Scene_Active(this->button_actions[index]->areasceneid)) {
                    cur_state = cur_state + 1;
                }
            }
        } else if (this->button_actions[index]->function == 3) {
            if (this->refmaster->macros->isrunning(this->button_actions[index]->globalsceneid)) {
                cur_state = cur_state + 1;
            }

        }
        cur_state = cur_state << 1;
    }
    int bp = 0;

    if ((cur_state >> 0X01) & 1) {
        bp++;
    }
    if ((cur_state >> 0X02) & 1) {
        bp++;
    }
    if ((cur_state >> 0X04) & 1) {
        bp++;
    }
    if ((cur_state >> 0X08) & 1) {
        bp++;
    }


    if (cur_state != this->buttonstate) {
        this->buttonstate = cur_state;
        Tree_Structs::str_buttonstate state;
        state.state = this->buttonstate;
        char data[sizeof (state)];
        memcpy(data, &state, sizeof (state));
        Tree_Message *m = new Tree_Message(this, this->UID, msg_types::ButtonState, this->get_msg_id(), sizeof (state), data);
        this->reftcp->send_message(m);
    }

}

void Tree_Node::start_temp_calib() {
    m.lock();
    char data[1];
    data[0] = 123;
    Tree_Message *mes = new Tree_Message(this, this->UID, msg_types::StartTempCalib, this->get_msg_id(), sizeof (data), data);
    this->reftcp->send_message(mes);
    m.unlock();
}
