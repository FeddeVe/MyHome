/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Scene_Values.cpp
 * Author: Fedde
 * 
 * Created on 23 februari 2022, 8:36
 */

#include "Scene_Values.h"

Scene_Values::Scene_Values(int msid, string lightid, int sceneid) {
    this->sceneid = sceneid;
    this->msid = msid;
    this->lightid = lightid;
    this->raw_brightness = 0;
    this->brightness = 0;
    this->ct = 333;
    this->usect = true;
    this->x = 0;
    this->y = 0;
    this->raw_x = 0;
    this->raw_y = 0;
    this->red = 0;
    this->green = 0;
    this->blue = 0;
    this->inuse = false;
    this->ct_x = 0;
    this->ct_y = 0; 
}

Scene_Values::Scene_Values(string lightid, int sceneid) {
    this->sceneid = sceneid;
    this->msid = -1;
    this->lightid = lightid;
    this->raw_brightness = 0;
    this->brightness = 0;
    this->ct = 333;
    this->usect = true;
    this->x = 0;
    this->y = 0;
    this->raw_x = 0;
    this->raw_y = 0;
    this->red = 0;
    this->green = 0;
    this->blue = 0;
    this->inuse = false;
    this->ct_x = 0;
    this->ct_y = 0; 
}

Scene_Values::Scene_Values(const Scene_Values& orig) {
}

Scene_Values::~Scene_Values() {
}

void Scene_Values::set_brightness(int brightness) {
    this->raw_brightness = brightness;
    this->brightness = brightness * 100;
};

void Scene_Values::set_kelvin(int kelvin, bool use_ct) {
    this->kelvin = kelvin;
    int mired = 500;
    if (kelvin > 0) {
        mired = 1000000 / kelvin;
    }
    this->ct = mired;
    this->usect = use_ct;
    if (this->usect) {
        MYSQL *ms = Mysql_Connect();
        if (ms != NULL) {
            string query = "SELECT x1, y1 FROM cttoxy WHERE ct=" + inttostring(kelvin);
            if (!mysql_query(ms, query.c_str())) {
                MYSQL_RES *result = mysql_store_result(ms);
                if (result != NULL) {
                    //Get the number of columns
                    int num_rows = mysql_num_rows(result);
                    int num_fields = mysql_num_fields(result);

                    MYSQL_ROW row; //An array of strings
                    while ((row = mysql_fetch_row(result))) {
                        if (num_fields >= 1) {
                            this->raw_ct_x = atof(row[0]);
                            this->raw_ct_y = atof(row[1]);
                            this->ct_x = this->raw_ct_x * 1000;
                            this->ct_y = this->raw_ct_y * 1000;

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
}

void Scene_Values::set_rgb(int r, int g, int b) {
    this->red = r;
    this->green = g;
    this->blue = b;
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

    this->raw_x = x;
    this->raw_y = y;
    this->x = this->raw_x * 1000;
    this->y = this->raw_y * 1000;

};

void Scene_Values::set_in_use(bool inuse) {
    this->inuse = inuse;
}

void Scene_Values::Save_Values() {
    MYSQL *ms = Mysql_Connect();
    if (ms != NULL) {
        if (this->msid != -1) {
            string query = "UPDATE Scene_Values set Brightness=" + inttostring(this->raw_brightness) + ", CT=" + inttostring(this->kelvin) + ", UseCT=" + booltostring(this->usect) + ", Red=" + inttostring(this->red) + ", Green=" + inttostring(this->green) + ", Blue=" + inttostring(this->blue) + ", InUse=" + booltostring(this->inuse) + " WHERE id=" + inttostring(this->msid);
            if (!mysql_query(ms, query.c_str())) {
            } else {
                string ms_err = string(mysql_error(ms));
                cout << "MYSQL Error " << ms_err << endl;
            }
           
        } else {
            string query = "INSERT INTO Scene_Values (SceneID, lightid, Brightness, CT, UseCT, Red, Green, Blue, InUse) VALUES (" + inttostring(this->sceneid) + ",'" + this->lightid + "', " + inttostring(this->raw_brightness) + "," + inttostring(this->kelvin) + "," + inttostring(this->usect) + "," + inttostring(this->red) + "," + inttostring(this->green) + "," + inttostring(this->blue) + ", " + booltostring(this->inuse) + ")";
            if (!mysql_query(ms, query.c_str())) {
                query = "SELECT LAST_INSERT_ID()";
                if (!mysql_query(ms, query.c_str())) {
                    MYSQL_RES *result = mysql_store_result(ms);
                    if (result != NULL) {
                        //Get the number of columns
                        int num_rows = mysql_num_rows(result);
                        int num_fields = mysql_num_fields(result);

                        MYSQL_ROW row; //An array of strings
                        while ((row = mysql_fetch_row(result))) {
                            if (num_fields >= 1) {
                                this->msid = atoi(row[0]);
                            }
                        }
                    }
                    mysql_free_result(result);
                }
            } else {
                string ms_err = string(mysql_error(ms));
                cout << "MYSQL Error " << ms_err << endl;
            }
        }
         Mysql_Disconnect(ms);
    }

};