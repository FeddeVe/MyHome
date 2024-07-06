/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "hardware.h"

_hardware::_hardware(Master* m, _powermeter *refpowermeter) {
    this->refmaster = m;
    this->refpowermeter = refpowermeter;
    wiringPiSetup();
    pinMode(led0, OUTPUT);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(trapkastdeur, INPUT);
    pinMode(CE_Pin, OUTPUT);
    pinMode(IRQ_Pin, INPUT);
    wiringPiSPISetup(0, 6000000);
    this->l0 = 0;
    this->l1 = 0;
    this->l2 = 0;
    MY_ADDR[0] = 0;
    MY_ADDR[1] = 0;
    MY_ADDR[2] = 0;
    MY_ADDR[3] = 0;
    MY_ADDR[4] = 0;

    this->olddeur = 99;

    this->setup_nrf();
    this->nrf_flush_rx();
    this->nrf_set_rx();


};

_hardware::~_hardware() {
    for(int i = 0; i < this->msg_list.size(); i++){
        delete this->msg_list[i];
    }
    
};

void _hardware::setup_nrf() {
    unsigned char SPI_BUFFER[10];

    SPI_BUFFER[0] = W_REGISTER + CONFIG;
    SPI_BUFFER[1] = FIRST_CONFIG;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);
    usleep(1300);
    SPI_BUFFER[0] = W_REGISTER + EN_AA;
    SPI_BUFFER[1] = DEF_EN_AA;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);
    SPI_BUFFER[0] = W_REGISTER + EN_RX_ADDR;
    SPI_BUFFER[1] = DEF_EN_RX_ADDR;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);
    SPI_BUFFER[0] = W_REGISTER + SETUP_AW;
    SPI_BUFFER[1] = DEF_SETUP_AW;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);
    SPI_BUFFER[0] = W_REGISTER + SETUP_RETR;
    SPI_BUFFER[1] = INIT_SETUP_RETR;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);
    SPI_BUFFER[0] = W_REGISTER + RF_CH;
    SPI_BUFFER[1] = DEF_RF_CH;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);
    SPI_BUFFER[0] = W_REGISTER + RF_SETUP;
    SPI_BUFFER[1] = DEF_RF_SETUP;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);
    SPI_BUFFER[0] = W_REGISTER + RX_PW_P0;
    SPI_BUFFER[1] = DEF_RX_PW_P0;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);
    SPI_BUFFER[0] = W_REGISTER + RX_PW_P1;
    SPI_BUFFER[1] = DEF_RX_PW_P1;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);
    SPI_BUFFER[0] = W_REGISTER + RX_ADDR_P1;
    SPI_BUFFER[1] = DEF_RX_ADDR_P1;
    SPI_BUFFER[2] = DEF_RX_ADDR_P1;
    SPI_BUFFER[3] = DEF_RX_ADDR_P1;
    SPI_BUFFER[4] = DEF_RX_ADDR_P1;
    SPI_BUFFER[5] = DEF_RX_ADDR_P1;
    wiringPiSPIDataRW(0, SPI_BUFFER, 6);
    SPI_BUFFER[0] = W_REGISTER + RX_ADDR_P0;
    SPI_BUFFER[1] = this->MY_ADDR[0];
    SPI_BUFFER[2] = this->MY_ADDR[1];
    SPI_BUFFER[3] = this->MY_ADDR[2];
    SPI_BUFFER[4] = this->MY_ADDR[3];
    SPI_BUFFER[5] = this->MY_ADDR[4];
    wiringPiSPIDataRW(0, SPI_BUFFER, 6);
    this->nrf_read_status(SPI_BUFFER[0]);
    this->nrf_clear_status();


};

void _hardware::nrf_clear_status() {
    unsigned char SPI_BUFFER[10];
    SPI_BUFFER[0] = W_REGISTER + STATUS;
    SPI_BUFFER[1] = DEF_CLR_STATUS;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);
    this->nrf_read_status(SPI_BUFFER[0]);

};

void _hardware::nrf_get_status() {
    unsigned char SPI_BUFFER[10];
    SPI_BUFFER[0] = 255;
    wiringPiSPIDataRW(0, SPI_BUFFER, 1);
    this->nrf_read_status(SPI_BUFFER[0]);
};

void _hardware::nrf_read_status(char data) {
    s_nrf_status.MAX_RT = 0;
    s_nrf_status.RX_DR = 0;
    s_nrf_status.RX_P_NO = 0;
    s_nrf_status.TX_DS = 0;
    s_nrf_status.TX_FULL = 0;
    s_nrf_status.reserved = 0;

    if (data >= 128) {
        s_nrf_status.reserved = 1;
        data = data - 128;
    }

    if (data >= 64) {
        s_nrf_status.RX_DR = 1;
        data = data - 64;
    }

    if (data >= 32) {
        s_nrf_status.TX_DS = 1;
        data = data - 32;
    }

    if (data >= 16) {
        s_nrf_status.MAX_RT = 1;
        data = data - 16;
    }

    if (data >= 8) {
        s_nrf_status.RX_P_NO = s_nrf_status.RX_P_NO + 4;
        data = data - 8;
    }

    if (data >= 4) {
        s_nrf_status.RX_P_NO = s_nrf_status.RX_P_NO + 2;
        data = data - 4;
    }

    if (data >= 2) {
        s_nrf_status.RX_P_NO = s_nrf_status.RX_P_NO + 1;
        data = data - 2;
    }

    if (data >= 1) {
        s_nrf_status.TX_FULL = 1;
    }
};

void _hardware::nrf_flush_tx() {
    unsigned char SPI[10];
    SPI[0] = FLUSH_TX;
    wiringPiSPIDataRW(0, SPI, 1);


}

void _hardware::nrf_flush_rx() {
    unsigned char SPI_BUFFER[10];
    SPI_BUFFER[0] = FLUSH_RX;
    wiringPiSPIDataRW(0, SPI_BUFFER, 1);
    this->nrf_read_status(SPI_BUFFER[0]);

};

void _hardware::nrf_set_rx() {
    digitalWrite(CE_Pin, 0);
    this->nrf_clear_status();
    this->nrf_flush_rx();
    unsigned char SPI_BUFFER[10];
    SPI_BUFFER[0] = W_REGISTER + CONFIG;
    SPI_BUFFER[1] = DEF_CONFIG_RX;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);

    SPI_BUFFER[0] = W_REGISTER + RX_ADDR_P0;
    SPI_BUFFER[1] = MY_ADDR[0];
    SPI_BUFFER[2] = MY_ADDR[1];
    SPI_BUFFER[3] = MY_ADDR[2];
    SPI_BUFFER[4] = MY_ADDR[3];
    SPI_BUFFER[5] = MY_ADDR[4];
    wiringPiSPIDataRW(0, SPI_BUFFER, 6);

    SPI_BUFFER[0] = W_REGISTER + EN_AA;
    SPI_BUFFER[1] = DEF_EN_AA;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);

    SPI_BUFFER[0] = W_REGISTER + EN_RX_ADDR;
    SPI_BUFFER[1] = DEF_EN_RX_ADDR;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);




    digitalWrite(CE_Pin, 1);



};

void _hardware::nrf_get_fifo_status() {
    unsigned char SPI_BUFFER[10];
    SPI_BUFFER[0] = R_REGISTER + FIFO_STATUS;
    SPI_BUFFER[1] = 0;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);
    this->nrf_read_status(SPI_BUFFER[0]);
    unsigned char data = SPI_BUFFER[1];
    s_nrf_fifo_status.RX_EMPTY = 0;
    s_nrf_fifo_status.RX_FULL = 0;
    s_nrf_fifo_status.TX_EMPTY = 0;
    s_nrf_fifo_status.TX_FULL = 0;
    s_nrf_fifo_status.TX_REUSE = 0;
    s_nrf_fifo_status.reserverd = 0;
    s_nrf_fifo_status.reserverd2 = 0;
    if (data >= 128) {
        s_nrf_fifo_status.reserverd = 1;
        data = data - 128;
    }

    if (data >= 64) {
        s_nrf_fifo_status.TX_REUSE = 1;
        data = data - 64;
    }

    if (data >= 32) {
        s_nrf_fifo_status.TX_FULL = 1;
        data = data - 32;
    }

    if (data >= 16) {
        s_nrf_fifo_status.TX_EMPTY = 1;
        data = data - 16;
    }
    if (data >= 8) {
        s_nrf_fifo_status.reserverd2 = 1;
        data = data - 8;
    }
    if (data > 4) {
        data = data - 4;
    }
    if (data >= 2) {
        s_nrf_fifo_status.RX_FULL = 1;
        data = data - 2;
    }
    if (data >= 1) {
        s_nrf_fifo_status.RX_EMPTY = 1;
    }
};

void _hardware::nrf_msg_recieve() {
    /*
    this->nrf_get_fifo_status();
    while (this->s_nrf_fifo_status.RX_EMPTY == 0) {
        unsigned char SPI_BUFFER[33];
        SPI_BUFFER[0] = R_RX_PAYLOAD;
        wiringPiSPIDataRW(0, SPI_BUFFER, 33);
        char item_id = SPI_BUFFER[1];
        if (item_id == 123) {
            // is power
            this->refpowermeter->set_nrf_value(SPI_BUFFER);
            int bp = 0;
            bp++;
        } else if (item_id == 1) {
            char addr = SPI_BUFFER[2];
            m.unlock();
//            this->refmaster->roomcontrollers->msg_from_nrf(SPI_BUFFER);
            m.lock();
            /*
             hardware_nrf_msg *msg = new hardware_nrf_msg();
             msg->addr[0] = 1;
             msg->addr[1] = addr;
             msg->addr[2] = 0;
             msg->addr[3] = 0;
             msg->addr[4] = 0;
             msg->msg[0] = this->refmaster->valves->get_valve_request(addr);
             msg->msg[1] = 0;
             msg->msg[2] = SPI_BUFFER[3] ^ SPI_BUFFER[4]; // XOR BATTERY
             msg->msg[3] = SPI_BUFFER[5] ^ SPI_BUFFER[6] ^ SPI_BUFFER[7]; // XOR TEMPERATURE
             msg->msg[4] = SPI_BUFFER[8] ^ SPI_BUFFER[9] ^ SPI_BUFFER[10]; // XOR Pressure
             msg->msg[5] = SPI_BUFFER[11] ^ SPI_BUFFER[12]; // XOR Humidity
             msg->msg[6] =  SPI_BUFFER[13] ^ SPI_BUFFER[14]; // XOR States;
             this->msg_list.push_back(msg);
           
            //    this->nrf_send_SPI_message(ret_data, ret_addr);


        }

        this->nrf_get_fifo_status();

    }
    this->nrf_flush_rx();
     */

};

void _hardware::do_work() {
    m.lock();
    try{
    this->do_leds();



    int deur = digitalRead(trapkastdeur);
    if (deur != olddeur) {
        olddeur = deur;
        Area *a = this->refmaster->areas->get_area(13);
        if (a != NULL) {
            if (deur == 1) {
                cout << "DEUR = 1" << endl;
                a->set_brightness(100, 0, 0, "deur");                

            } else {
               cout << "DEUR = 0" << endl;
               a->set_brightness(0, 0, 0, "deur");    
            }
        }
    }
    }catch(exception &e){
     WriteError("Hardware DoWork", e.what());   
    }



    m.unlock();
}

void _hardware::nrf_send_SPI_message(unsigned char *msg, unsigned char *addr) {



    this->nrf_flush_tx();

    this->nrf_clear_status();
    digitalWrite(CE_Pin, 0);
    unsigned char SPI_BUFFER[33];
    SPI_BUFFER[0] = W_REGISTER + CONFIG;
    SPI_BUFFER[1] = DEF_CONFIG_TX;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);
    usleep(130);

    SPI_BUFFER[0] = W_REGISTER + RX_ADDR_P0;
    SPI_BUFFER[1] = addr[0];
    SPI_BUFFER[2] = addr[1];
    SPI_BUFFER[3] = addr[2];
    SPI_BUFFER[4] = addr[3];
    SPI_BUFFER[5] = addr[4];
    wiringPiSPIDataRW(0, SPI_BUFFER, 6);

    SPI_BUFFER[0] = W_REGISTER + TX_ADDR;
    SPI_BUFFER[1] = addr[0];
    SPI_BUFFER[2] = addr[1];
    SPI_BUFFER[3] = addr[2];
    SPI_BUFFER[4] = addr[3];
    SPI_BUFFER[5] = addr[4];
    wiringPiSPIDataRW(0, SPI_BUFFER, 6);

    SPI_BUFFER[0] = W_REGISTER + SETUP_RETR;
    SPI_BUFFER[1] = DEF_SETUP_RETR;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);

    SPI_BUFFER[0] = W_REGISTER + EN_AA;
    SPI_BUFFER[1] = DEF_EN_AA;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);

    SPI_BUFFER[0] = W_REGISTER + EN_RX_ADDR;
    SPI_BUFFER[1] = DEF_EN_RX_ADDR;
    wiringPiSPIDataRW(0, SPI_BUFFER, 2);

    SPI_BUFFER[0] = W_TX_PAYLOAD;
    for (int i = 0; i < 32; i++) {
        SPI_BUFFER[1 + i] = msg[i];
    }

    wiringPiSPIDataRW(0, SPI_BUFFER, 33);

    unsigned char count = 0;
    digitalWrite(CE_Pin, 1);
    while (digitalRead(IRQ_Pin) == 1) {
        usleep(10000);
        count++;
        if (count > 65000) {
            break;
        }
    }
    digitalWrite(CE_Pin, 0);
    this->nrf_get_status();


    if (s_nrf_status.MAX_RT) {
        int bp = 0;
        bp++;
        /*CODE FOR REROUTING
        retry++;
        if (retry == 4) {
            NRF_STATUS = 0;
            retry= 0;
        }
    } else {
        retry = 0;
        if (NRF_STATUS == 1) {
            clr_status();
            NRF_STATUS = 2; //  natuurlijk geen 1, dit moet 2 worden :)

        }
      
    PIR_value = 0;
         */

    }
    this->nrf_set_rx();



};

void _hardware::do_leds() {
    if (this->l0 == 1) {
        this->l0 = 0;
        this->l1 = 1;
    } else if (this->l1 == 1) {
        this->l1 = 0;
        this->l2 = 1;
    } else if (this->l2 == 1) {
        this->l2 = 0;
        this->l0 = 1;
    } else {
        this->l0 = 1;
    }

    digitalWrite(led0, this->l0);
    digitalWrite(led1, this->l1);
    digitalWrite(led2, this->l2);


}

string _hardware::get_cpu_percentage() {
    int FileHandler;
    char FileBuffer[1024];
    float load;
    float l1;
    float l2;

    try {
        FileHandler = open("/proc/loadavg", O_RDONLY);
        if (FileHandler < 0) {
            return "-1";
        }
        read(FileHandler, FileBuffer, sizeof (FileBuffer) - 1);
        //	sscanf(FileBuffer, "%f", &load);
        sscanf(FileBuffer, "%f" "%f" "%f", &load, &l1, &l2);
        close(FileHandler);
    } catch (exception& e) {
        //refmaster->display_error(sectie + "Reading CPU Load", e.what());
    }

    double l = (load * 100);
    string ret = inttostring(l);

    return ret + "%";
};

long _hardware::get_max_mem() {
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    long res = pages * page_size;
    return res / 131072;

};

double _hardware::get_cur_mem() {
    using std::ios_base;
    using std::ifstream;
    using std::string;

    double vm_usage = 0.0;
    double resident_set = 0.0;

    // 'file' stat seems to give the most reliable results
    //
    ifstream stat_stream("/proc/self/stat", ios_base::in);

    // dummy vars for leading entries in stat that we don't care about
    //
    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;

    // the two fields we want
    //
    unsigned long vsize;
    long rss;

    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
            >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
            >> utime >> stime >> cutime >> cstime >> priority >> nice
            >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

    stat_stream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0;
    resident_set = rss * page_size_kb;

    return vm_usage;

};

void *_hardware::run() {
    /*
    this->exit = false;
    while(this->exit == false){
    m.lock();
    
    char IRQ = digitalRead(IRQ_Pin);
    if (IRQ == 0) {
        this->nrf_get_status();
        if (this->s_nrf_status.RX_DR == 1) {
            this->nrf_msg_recieve();
            this->nrf_clear_status();
        } else {
            this->nrf_clear_status();
        }
    }

    if(this->msg_list.size() > 0) {
        this->nrf_send_SPI_message(this->msg_list[0]->msg, this->msg_list[0]->addr);
        delete this->msg_list[0];
        this->msg_list.erase(this->msg_list.begin());
    }
    m.unlock();
    
    
    usleep(10000);
    
  
    }
    
     */
    return 0;

};

void _hardware::add_nrf_msg(hardware_nrf_msg* msg){
  
  m.lock();
  this->msg_list.push_back(msg);

  m.unlock();  
 
    
};

void _hardware::quit(){
  m.lock();
  this->exit = true;
m.unlock();  
};