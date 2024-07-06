/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   Tree_OTA_list_item.h
 * Author: Fedde
 *
 * Created on 2 april 2023, 21:26
 */

#ifndef TREE_OTA_LIST_ITEM_H
#define TREE_OTA_LIST_ITEM_H

#include <string.h>

class Tree_OTA_list_item{
public:
    uint32_t nodeid;
    uint16_t nodetype;
    uint16_t firmware;
    std::string filename;    
};



#endif /* TREE_OTA_LIST_ITEM_H */

