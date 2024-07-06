/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/main.cc to edit this template
 */

/* 
 * File:   main.cpp
 * Author: Fedde
 *
 * Created on 9 februari 2022, 11:47
 */

#include <cstdlib> 
#include <iostream>

#include <unistd.h>
#include "Master.h"
 
using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    cout << "Starting MyHome" << endl;
    Master *m = new Master();
    m->start(); 
    cout << "Type exit any time to quit" << endl;
    string sinput = "";
    cin >> sinput;
    while (sinput != "exit") { 
        sleep(1);
        cin >> sinput;
    } 
    m->Exit();
    m->join();
    delete m;
    cout << "END" << endl;
    return 0;     
}

