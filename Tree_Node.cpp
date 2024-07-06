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

Tree_Node::Tree_Node(Tree_Server *reftcp, Tree_Message *m) {
    this->reftcp = reftcp;
    this->msg_id = 0;
    this->online = true;
    this->UID = m->header->from;
    this->firmware = m->header->firmware;
    this->nodetype = m->header->node_type;
    this->config_loaded = false;
    this->last_message_send = time(0);

    for (uint32_t i = 0; i < 100; i++) {
        this->arr_avg_turn_time[i] = UINT16_MAX;
    };
    this->arr_avg_turn_time_index = 0;
    this->avg_turn_time = UINT16_MAX;
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
        if (diff > 60) {
            this->ota_running = false;
        }
    }

    m.unlock();
    uint64_t diff = time(0) - this->last_message_send;
    if (diff > this->reftcp->mesh_info_interval) {
        this->last_message_send = time(0);
        Tree_Message *mes = new Tree_Message(this, this->UID, msg_types::Hello, this->get_msg_id(), 0, "");
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
    if (this->config_loaded == false) {
        this->load_config();
    }
    // check if fully recieved

    switch (m->header->type) {
        case msg_types::OTA_Request:
        {
            Tree_Message *new_req = this->reftcp->OTA->handle_OTA_Request(this, m);
            if (new_req != NULL) {
                this->reftcp->send_message(new_req);
                this->send_ack(m->header->from, m->header->msg_id);
                this->m.unlock();
                return true;
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


    this->send_ack(m->header->from, m->header->msg_id);
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

void Tree_Node::load_config() {
    if (this->nodetype == NODE_TYPES::MSH_DISPLAY) {
        this->load_screen_config();
    }
    this->config_loaded = true;
};

void Tree_Node::load_screen_config() {
    string jsonfilename = "/var/www/html/ScreenConfigurator/Screen/Default_conf.json";
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

    Document d;
    ParseResult ok = d.Parse(buffer.data());;
if (!ok) {
    fprintf(stderr, "JSON parse error: %s (%u)",
            GetParseError_En(ok.Code()), ok.Offset());
    //exit(EXIT_FAILURE);
    return;
}
    
    int isconverted = d["converted"].GetInt();
    uint32_t version = d["version"].GetInt();
    this->config_version = version;
    
    if (isconverted == 1) {
        this->config_file = d["config_filename"].GetString();
        return;
    }
    int bp = 0;
    bp++;
    Tree_Structs::str_display_config config;
    char *header = "#MyConfig#";
    strcpy(config.config_header, header);
    config.version = d["version"].GetInt();
    //// CHANGE below ///////////
    config.area_id = 0;
    config.temp_offset_fast = 1.0f;
    config.temp_offset_slow = 1.0f;
    ////////////////////////////////////////////////
    config.max_int = d["max_int"].GetInt();
    config.max_touch_int = d["max_touch_int"].GetInt();
    config.min_int = d["min_int"].GetInt();
    config.min_touch_int = d["min_touch_int"].GetInt();
    config.off_touch_mode = d["off_touch_mode"].GetInt();
    config.off_long_press_mode = d["off_long_press_mode"].GetInt();
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
    uint32_t config_size = sizeof (Tree_Structs::str_display_config);
    uint32_t file_index = config_size;
    string tmp_file = "/var/www/html/ScreenConfigurator/Screen/tmp.conf";
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
    if (Header.size() >= 31) {
        memcpy(config.btn1_header, Header.c_str(), 31);
        config.btn1_header[31] = '/0';
    } else {
        memcpy(config.btn1_header, Header.c_str(), Header.size());
    }
    string Footer = d["btn1_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn1_footer, Footer.c_str(), 31);
        config.btn1_footer[31] = '/0';
    } else {
        memcpy(config.btn1_footer, Footer.c_str(), Footer.size());
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
        config.btn2_header[31] = '/0';
    } else {
        memcpy(config.btn2_header, Header.c_str(), Header.size());
    }
    Footer = d["btn2_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn2_footer, Footer.c_str(), 31);
        config.btn2_footer[31] = '/0';
    } else {
        memcpy(config.btn2_footer, Footer.c_str(), Footer.size());
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
        config.btn3_header[31] = '/0';
    } else {
        memcpy(config.btn3_header, Header.c_str(), Header.size());
    }
    Footer = d["btn3_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn3_footer, Footer.c_str(), 31);
        config.btn3_footer[31] = '/0';
    } else {
        memcpy(config.btn3_footer, Footer.c_str(), Footer.size());
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
        config.btn4_header[31] = '/0';
    } else {
        memcpy(config.btn4_header, Header.c_str(), Header.size());
    }
    Footer = d["btn4_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn4_footer, Footer.c_str(), 31);
        config.btn4_footer[31] = '/0';
    } else {
        memcpy(config.btn4_footer, Footer.c_str(), Footer.size());
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
        config.btn5_header[31] = '/0';
    } else {
        memcpy(config.btn5_header, Header.c_str(), Header.size());
    }
    Footer = d["btn5_footer"].GetString();
    if (Footer.size() >= 31) {
        memcpy(config.btn5_footer, Footer.c_str(), 31);
        config.btn5_footer[31] = '/0';
    } else {
        memcpy(config.btn5_footer, Footer.c_str(), Footer.size());
    }
    config.btn5_function = d["btn5_function"].GetInt();
    
    
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
    filename = d["wifi_nok_0"].GetString();
    config.wifi_nok_0.offset = file_index;
    config.wifi_nok_0.size = this->add_button(filename);
    file_index = file_index + config.wifi_nok_0.size;
    remove(filename.c_str());
     filename = d["wifi_nok_1"].GetString();
    config.wifi_nok_1.offset = file_index;
    config.wifi_nok_1.size = this->add_button(filename);
    file_index = file_index + config.wifi_nok_1.size;
    remove(filename.c_str());
     filename = d["wifi_nok_2"].GetString();
    config.wifi_nok_2.offset = file_index;
    config.wifi_nok_2.size = this->add_button(filename);
    file_index = file_index + config.wifi_nok_2.size;
    remove(filename.c_str());
     filename = d["wifi_nok_3"].GetString();
    config.wifi_nok_3.offset = file_index;
    config.wifi_nok_3.size = this->add_button(filename);
    file_index = file_index + config.wifi_nok_3.size;
    remove(filename.c_str());
     filename = d["wifi_nok_4"].GetString();
    config.wifi_nok_4.offset = file_index;
    config.wifi_nok_4.size = this->add_button(filename);
    file_index = file_index + config.wifi_nok_4.size;
    remove(filename.c_str());
     filename = d["wifi_ok"].GetString();
    config.wifi_ok.offset = file_index;
    config.wifi_ok.size = this->add_button(filename);
    file_index = file_index + config.wifi_ok.size;
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



    string final_file = "/var/www/html/ScreenConfigurator/Screen/Default.conf";
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
    string jsonstring= strbuf.GetString();
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
    string tmp_file = "/var/www/html/ScreenConfigurator/Screen/tmp.conf";
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
    m.lock();
    this->online = online;
    m.unlock();
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
            if (msg_size == sizeof(response.data)) {
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
