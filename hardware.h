/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   hardware.h
 * Author: Fedde
 *
 * Created on 9 juli 2018, 11:22
 */

#ifndef HARDWARE_H
#define HARDWARE_H
#include<iostream>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/sysinfo.h>
#include <string>
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <ctime>
#include <vector>
#include <mutex>

#include <wiringPi.h>
#include <wiringPiSPI.h> 
#include <unistd.h>
#include "Master.h"
#include "powermeter.h"
#include "hardware_nrf_msg.h"
#include "Thread.h"



#define led0 8
#define led1 9
#define led2 7
#define trapkastdeur 3


#define R_REGISTER   0
#define W_REGISTER   32
#define R_RX_PAYLOAD  97
#define W_TX_PAYLOAD   160
#define FLUSH_TX   225
#define FLUSH_RX   226
#define REUSE_TX_PL  227

//REGISTER
#define CONFIG   0
#define EN_AA   1
#define EN_RX_ADDR   2 
#define SETUP_AW   3 
#define SETUP_RETR   4 
#define RF_CH   5 
#define RF_SETUP   6 
#define STATUS   7 
#define OBSERVE_TX   8 
#define CD   9 
#define RX_ADDR_P0   10 
#define RX_ADDR_P1   11 
#define RX_ADDR_P2   12 
#define RX_ADDR_P3   13 
#define RX_ADDR_P4   14 
#define RX_ADDR_P5   15 
#define TX_ADDR   16 
#define RX_PW_P0   17 
#define RX_PW_P1   18 
#define RX_PW_P2   19 
#define RX_PW_P3   20 
#define RX_PW_P4   21 
#define RX_PW_P5   22 
#define FIFO_STATUS   23 

//DEFAULT

#define FIRST_CONFIG 0b00001110
#define DEF_CONFIG_RX   0b00001111 
#define DEF_CONFIG_TX   0b00001110 
#define DEF_EN_AA   0b00000011
#define DEF_EN_RX_ADDR   0b00000001
#define DEF_SETUP_AW   0b00000011 
#define DEF_SETUP_RETR   0b010001000
#define DEF_SETUP_RETR_BROADCAST 0b00000000
#define DEF_RF_CH   0b00000110
#define DEF_RF_SETUP   0b00000111
#define DEF_CLR_STATUS   0b01110000 
#define DEF_RX_PW_P0   32
#define DEF_RX_PW_P1 32
#define DEF_RX_ADDR_P1 251 
#define INIT_SETUP_RETR 0b00000000


#define CE_Pin 25
#define IRQ_Pin 24

//NRF MSGTYPES

#define SPI_WELCOME_MSG 21
#define SPI_BATTERY_REQUEST 22
#define SPI_BATTERY_RESPONSE 23
#define SPI_MSG_FROM_MASTER_WRITE 11
#define SPI_MSG_FROM_SLAVE_WRITE 12
#define SPI_MSG_FROM_MASTER_READ 13
#define SPI_MSG_FROM_SLAVE_READ 14

class Master;
class _powermeter;


using namespace std;

class _hardware : public Thread {
private:
   
    mutex m;
    Master *refmaster;
    _powermeter *refpowermeter;
    bool exit;
    ifstream datei;
    char system_led;
    char led_counter;
    char l0;
    char l1;
    char l2;
    int nrf_broadcast;
    int olddeur;

    struct nrf_status {
        char reserved;
        char RX_DR;
        char TX_DS;
        char MAX_RT;
        char RX_P_NO;
        char TX_FULL;
    } s_nrf_status;

    struct nrf_fifo_status {
        char reserverd;
        char TX_REUSE;
        char TX_FULL;
        char TX_EMPTY;
        char reserverd2;
        char RX_FULL;
        char RX_EMPTY;
    } s_nrf_fifo_status;

    char MY_ADDR[5];

    void do_leds();
    void setup_nrf();
    void nrf_get_status();
    void nrf_get_fifo_status();
    void nrf_clear_status();
    void nrf_read_status(char data);

    void nrf_flush_tx();
    void nrf_flush_rx();
    void nrf_set_rx();
    void nrf_set_tx(char *addr); 
    char nrf_has_client_space();

    void nrf_msg_recieve();
    
    vector<hardware_nrf_msg*>msg_list;
    
    void split_adrres(char address[], char* outStr);
    void compile_addres(char address[], char* outStr);
    char get_first_addr(char address[]);
    
   


public:

    _hardware(Master *m, _powermeter *refpowermeter);
    ~_hardware();
   
    void do_work();
    void nrf_send_SPI_message(unsigned char *msg, unsigned char *addr);
    void stop();
    string get_cpu_percentage();
    string get_cpu_mhz();
    string get_free_space();
    string get_cpu_temp();
    long get_max_mem();
    double get_cur_mem();
    void quit();
    int get_int64_value_from_ascii_string(string source_string, int char_index, int64_t *result);
      void *run();
      
      void add_nrf_msg(hardware_nrf_msg *msg);


};



#endif /* HARDWARE_H */

