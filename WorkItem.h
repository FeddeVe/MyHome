/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   WorkItem.h
 * Author: Fedde
 *
 * Created on 17 februari 2016, 21:41
 */

#ifndef WORKITEM_H
#define WORKITEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "Thread.h"
#include "wqueue.h"
 
class WorkItem
{
    string m_message;
    int    m_number;
 
  public:
    WorkItem(const char* message, int number) 
          : m_message(message), m_number(number) {}
    ~WorkItem() {}
 
    const char* getMessage() { return m_message.c_str(); }
    int getNumber() { return m_number; }
};

#endif /* WORKITEM_H */

