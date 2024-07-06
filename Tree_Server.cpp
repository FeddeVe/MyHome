/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 *
 
 */

#include <ios>
#include <ostream>

#include "Tree_Server.h"

Tree_Server::Tree_Server() { 
    this->index = 0;
    this->exit = false;
    this->host_ready = true;
    this->last_message_send = this->get_timestamp_ms();
    this->last_mesh_info_send = this->get_timestamp_ms();
    this->mesh_info_interval = 5000;
    this->msg_min = 0;
    this->msg_id = 0;
    this->OTA = new Tree_OTA();
    this->connected = false;
    this->sleep = 100000; //50ms
}

Tree_Server::~Tree_Server() {
    delete this->acceptor;
    int bp = 0;
    bp++;
};

void Tree_Server::quit() {
    this->acceptor->~TCPAcceptor();
    this->exit = true;
}

void Tree_Server::init() {

};

void *Tree_Server::run() {

    stream = NULL;
    acceptor = NULL;
    acceptor = new TCPAcceptor(5434);
    cout << "Server started at port 7011" << endl;
    sockaddr cliaddr;
    memset(&cliaddr, 0, sizeof (cliaddr));
    string ret_msg;
    size_t len;
    char buffer[2048];
    vector<char>work_buffer;
    size_t maxlen = 5;
    uint32_t index = 0;
    uint32_t size = 0;
    uint32_t work_buffer_index = 0;
    bool message_reading = false;
    if (acceptor->start() == 0) {

        while (this->exit == false) {
            stream = acceptor->accept();
            cout << "Client Connected" << endl;
            if (stream != NULL) {
                try {
                    // while ((len = stream->receive(line, sizeof (line), 15)) > 0) {
                    while ((len = stream->receive(buffer, sizeof (buffer), 15)) > 0) {
                        if (len < 0) {
                            break;
                        }
                        if (len >= sizeof (buffer)) {
                            break;
                        }
                        // Data received
                        index = 0;
                        while (index < len) {
                            work_buffer.push_back(buffer[index]);
                            if (work_buffer.size() > 3) {
                                if ((work_buffer[0] == '#') && (work_buffer[3] == '#')) {
                                    size = work_buffer[1];
                                    size = size << 8;
                                    size = size + work_buffer[2];
                                    if ((size > sizeof (HDR)) || (size < sizeof (HDR) - MAX_MSG_SIZE)) {
                                        cout << "Wrong size recieved - " << size << endl; 
                                        work_buffer.clear();
                                        work_buffer.shrink_to_fit();
                                    } else {
                                        if (work_buffer.size() == size) {
                                            // log_d("size matches -- YOEPIE");
                                            // do something
                                            Tree_Message *m = new Tree_Message(work_buffer);
                                            this->handle_msg(m);
                                            delete m;
                                            // resetting buffer
                                            work_buffer.clear();
                                            work_buffer.shrink_to_fit();
                                        }
                                    }
                                } else {
                                    work_buffer.clear();
                                    work_buffer.shrink_to_fit();
                                }
                            }
                            index++;

                        }
                        
                    } // while connected 
            } catch (exception& e) {
                cout << "TCP ERROR + " + string(e.what()) << endl;
                //WriteError("TCP Loop", e.what());
            }
            delete stream;
        }
        this->connected = false;
        // this->sleep = 50000;
        cout << "Client Disconnected" << endl;
    }
}
cout << "SERVER STOPPED" << endl;
 
};

uint32_t Tree_Server::get_msg_id() {
    if (this->msg_id > UINT32_MAX) {
        this->msg_id = 0;
    }
    this->msg_id++;
    return this->msg_id;
};

bool Tree_Server::handle_msg(Tree_Message *m) {
    try {
        this->msg_min_buffer.push_back(this->get_timestamp_ms());
        if (m->err) {
            return true;
        }
    } catch (exception& e) {
        cout << "Buffer msg min error" << endl;
    }

    if (m->header->type == msg_types::socket_ready) {
        // this->ready_mutex.lock();
        // this->host_ready = true;
        // this->ready_mutex.unlock();
        //cout << "socket_ready_recieved" << endl;

        return false;
    }

    if (m->header->type == msg_types::ping) {
        cout << "Ping Recieved" << endl;
        this->connected = true;
        Tree_Node *node = NULL;
        for (int i = 0; i < this->nodes.size(); i++) {
            if (this->nodes[i]->UID == m->header->from) {
                node = this->nodes[i];
                break;
            }
        }
        if (node == NULL) {
            node = new Tree_Node(this, m);
            this->nodes.push_back(node);
        }
        return true;
    }

    if (m->header->type == msg_types::ack) {
        cout << "ACK RECIEVED FOR " << m->header->msg_id << endl;
        //  xSemaphoreTake(this->buffer_mutex, portMAX_DELAY);
        this->buffer_mutex.lock();
        for (int i = 0; i < new_msg_buffer.size(); i++) {
            if (this->new_msg_buffer[i]->UID == m->header->msg_id) {
                //  if (this->mes_buffer[i]->header->total_size <= m->header->index)
                //  {
                // ACK RECIEVED, DELETING AND GO ON

                //this->set_avg_turn_time(this->msg_buffer[i]);
                cout << "ACK RECIEVED, DELETING BUFFER POSs" << endl;
                delete this->new_msg_buffer[i];
                this->new_msg_buffer.erase(this->new_msg_buffer.begin() + i);
                break;
                // xSemaphoreGive(this->buffer_mutex);
                /// return true;
                //   }
                //    else
                /*
                    {
                        log_d("ACK RECIEVED FOR HIER");
                        this->set_avg_turn_time(this->mes_buffer[i]);
                        this->mes_buffer[i]->send_msg(this, m->header->index);
                        delete m;
                        m = NULL;
                        return true;
                    }
                 */
            }
        }
        // xSemaphoreGive(this->buffer_mutex);
        this->buffer_mutex.unlock();
        return false;
    }

    try {
        Tree_Node *node = NULL;
        for (int i = 0; i < this->nodes.size(); i++) {
            if (this->nodes[i]->UID == m->header->from) {
                node = this->nodes[i];
                break;
            }
        }
        if (node == NULL) {
            node = new Tree_Node(this, m);
            this->nodes.push_back(node);
        }

        int uptime_hours = 0;
        int uptime_minutes = 0;
        uint64_t uptime = m->header->uptime;
        uptime = uptime / 1000;
        uptime = uptime / 60;
        while (uptime >= 60) {
            uptime = uptime - 60;
            uptime_hours++;
        }
        uptime_minutes = uptime;

        cout << "Header info - From:" << (uint32_t) m->header->from;
        cout << " - To:" << (uint32_t) m->header->to;
        cout << " - Type:" << (uint16_t) m->header->type;
        cout << " - Uptime: " << (uint64_t) m->header->uptime;
        cout << " - Uptime: " << uptime_hours << ":" << uptime_minutes;
        cout << " - Firmware: " << (uint16_t) m->header->firmware;
        if (m->header->type == msg_types::Pir_Sensor) {
            if (m->header->data[0] == 1) {
                cout << "JOPIE HET WERKT";
            }
            cout << " - PIR VALUE " << (uint8_t) m->header->data[0];
        }
        if (m->header->type == 23) {
            uint16_t raw = (m->header->data[1] * 256) + m->header->data[0];
            uint16_t comp = (m->header->data[3] * 256) + m->header->data[2];
            cout << " - LUX Report - Raw: " << (uint16_t) raw << " - Compensated = " << (uint16_t) comp;

        }
        cout << " - " << m->header->data << endl;

        node->handle_msg(m);
    } catch (exception& e) {
        cout << "Error in node vector : " << e.what() << endl;

    }


    return true; 

};

void Tree_Server::mesh_worker() {
    
    this->buffer_tasks();
    // if(this->sleep > 5000){
    //     this->sleep = this->sleep - 1;
    // }
    // }
    //cout << "Sleep = " << sleep << endl;
    // usleep(this->sleep / 2);
    // if(this->connected){
    usleep(this->sleep);

}

void Tree_Server::buffer_tasks() {
    this->buffer_mutex.lock();
    uint64_t resend_time = 0;
    uint64_t r_time = 0;

    resend_time = 500;
    //  log_d("resend time = %ld , millis= %ld", resend_time, millis());
    r_time = this->get_timestamp_ms() - resend_time;
    //  log_d("r_time = %ld", r_time);
    for (int i = 0; i < this->new_msg_buffer.size(); i++) {
        if (this->new_msg_buffer[i]->time_stamp < r_time) {
            //  log_d("MESID = %d, resend_cnt = %d", mes_buffer[i]->header->UID, mes_buffer[i]->resend_cnt);
            if (this->new_msg_buffer[i]->resend_cnt > MAX_RESEND) {
                // deleting message, node offline
                // this->set_avg_turn_time(this->msg_buffer[i]);
                cout << "Message buffer deleted, MAX RESEND REACHED - BufferNO: " << endl;
                Tree_Node *n = NULL;
                for (int i = 0; i < this->nodes.size(); i++) {
                    if (this->nodes[i]->UID == this->new_msg_buffer[i]->to) {
                        this->nodes[i]->set_online(false);
                        break;
                    }
                }
                delete this->new_msg_buffer[i];
                this->new_msg_buffer.erase(this->new_msg_buffer.begin() + i);
                break;
            } else {
                cout << "Resending message for the " << (uint32_t) this->new_msg_buffer[i]->resend_cnt << " time" << endl;
                this->new_msg_buffer[i]->resend_msg(this);
                break;
            }
        }
    }
    // xSemaphoreGive(this->buffer_mutex);
    this->buffer_mutex.unlock();
};

void Tree_Server::network_send(char *data, uint16_t len) {
    this->network_send_mutex.lock();   
    this->msg_min_buffer.push_back(this->get_timestamp_ms());
    if (this->stream != NULL) {
        this->stream->_send(data, len);
        this->last_message_send = this->get_timestamp_ms();
    } 
   // usleep(1000);
    this->network_send_mutex.unlock();


}

void Tree_Server::send_ping() {
    cout << "Send Ping" << endl;
    Tree_Message *m = new Tree_Message(NULL, 0, msg_types::ping, 0, 0, "hallopop");
    this->send_message(m);     
}; 

uint64_t Tree_Server::get_timestamp_ms() {
    
    milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    return ms.count();
}

void Tree_Server::run_task() {
    m.lock();
    this->OTA->loop_work();
    for (int i = 0; i < this->nodes.size(); i++) {
        this->nodes[i]->loop_worker();
    }
    uint64_t diff = this->get_timestamp_ms() - this->last_message_send;
    if (diff > 2500) {
        this->send_ping();
    }
    diff = this->get_timestamp_ms() - this->last_mesh_info_send;
    if (diff > this->mesh_info_interval) {
        this->send_mesh_info();
        this->last_mesh_info_send = this->get_timestamp_ms();
    }

    uint64_t remove = this->get_timestamp_ms() - (60 * 1000);
    for (uint32_t i = 0; i < this->msg_min_buffer.size(); i++) {
        if (this->msg_min_buffer[i] < remove) {
            this->msg_min_buffer.erase(this->msg_min_buffer.begin() + i);
            i = i - 1;
        }
    }
    this->msg_min = this->msg_min_buffer.size();
    uint32_t msg_sec = this->msg_min / 60;
    if (msg_sec > 100) {
        if (this->mesh_info_interval < UINT16_MAX - 100) {
            this->mesh_info_interval = this->mesh_info_interval + 100;
        }
    } else {
        if (this->mesh_info_interval > 5000) {
            this->mesh_info_interval = this->mesh_info_interval - 100;
        }
    }
    cout << "MSG PER MINUTE " << this->msg_min << " - MSG PER SEC " << msg_sec << " - INTERVAL " << this->mesh_info_interval << endl;

    time_t nu = time(0);
    std::tm* now = std::localtime(&nu);
    if(now->tm_min != this->datetime.minute){
        this->datetime.timestamp_s = nu;
        this->datetime.Month = now->tm_mon;
        this->datetime.WeekDay = now->tm_wday;
        this->datetime.day = now->tm_mday;
        this->datetime.hour = now->tm_hour;
        this->datetime.minute = now->tm_min;
        this->datetime.year = now->tm_year;
        string Weekdays[7] = {"Sunday","Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
        string Months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "Oktober", "November", "December"};
        string Day = Weekdays[now->tm_wday];
        string Clock = "--:--";
        string DateString = "-- "+ Months[now->tm_mon] + "----";
        memcpy(this->datetime.ClockString, Clock.c_str(), sizeof(Clock)+1);
        memcpy(this->datetime.MonthString, DateString.c_str(), sizeof(DateString)+1);
        memcpy(this->datetime.WeekDayString, Day.c_str(), sizeof(Day)+1);
        
        char data[sizeof(this->datetime)];
        memcpy(data, &this->datetime, sizeof(this->datetime));
        Tree_Message *m = new Tree_Message(NULL, 0xFFFFFFFF, msg_types::Time_Report, 0, sizeof(this->datetime), data);
        this->send_message(m); 
    }
    


    if (this->nodes.size() > 0) {

        Tree_Node *node = this->nodes[this->index];
        if (node != NULL) {
            //    cout << "Sending test to: "<< node->node_id[0] << ":" << node->node_id[1] <<":"<<node->node_id[2] <<":" << node->node_id[3] <<":" << node->node_id[4] <<":" << node->node_id[5] << endl;
            node->send_test_msg();
        } else {
            Tree_Message *m = new Tree_Message(NULL, 0xFFFFFFFF, msg_types::test, 0, 17, "Broadcast to all!");
            this->send_message(m);
            index = -1;
        }
        this->index++;
    }
    m.unlock();
};

void Tree_Server::send_mesh_info() {
    char info[8];
    info[0] = 0;
    info[1] = MAX_LAYERS;
    uint16_t tmp = this->mesh_info_interval;
    info[3] = tmp & 0xFF;
    tmp = tmp >> 8;
    info[2] = tmp & 0xFF;

    Tree_Message *m = new Tree_Message(info, 8, 0xFFFFFFFF, msg_types::mesh_info, 0, 0, "-");
    this->send_message(m);   
};

void Tree_Server::send_message(Tree_Message * m) {
    m->send_msg(this);
    if ((m->msg_type == msg_types::ack) || (m->msg_type == msg_types::ping) || (m->to == 0xFFFFFFFF)) {
        delete m;
    } else {
        this->buffer_mutex.lock();
        bool inserted = false;
        for (int i = 0; i < this->new_msg_buffer.size(); i++) {
            if ((this->new_msg_buffer[i]->msg_type == m->msg_type) && (this->new_msg_buffer[i]->to == m->to)) {
                cout << "REPLACING BUFFER MESSAGE" << endl;
                m->resend_cnt = this->new_msg_buffer[i]->resend_cnt;
                delete this->new_msg_buffer[i];
                this->new_msg_buffer[i] = m;
                inserted = true;
            }
        }
        if (inserted == false) {
            this->new_msg_buffer.push_back(m);
        }
        this->buffer_mutex.unlock();
    }
}
 


