/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 *
 
 */

#include "tcp.h"

_tcp::_tcp(Master *refmaster) {
    this->exit = false;
    this->refmaster = refmaster;
}

_tcp::~_tcp() {
    delete this->acceptor;
    int bp = 0;
    bp++;
};

void _tcp::quit() {
    this->acceptor->~TCPAcceptor();
    this->exit = true;
}

void _tcp::init() {

};

string _tcp::handle_incoming(string incoming){
    Document document;
    string ret_msg = "";
    ParseResult ok = document.Parse(incoming.c_str());
    if (!ok) {
        string error = GetParseError_En(ok.Code());
        WriteError("TCP ParseError", error);
    } else {
        ret_msg = "{\"error\":\"FALSE REQUEST\"}";
        if (document.IsObject()) {
            // cout << "TCP NEEDS SOME PROTECTION" << endl;
            if (document.HasMember("item")) {
                string item = document["item"].GetString();
                if (item == "Bridges") {
                    ret_msg = this->handle_bridge(document);
                } else if (item == "Lights") {
                    ret_msg = this->handle_lights(document);
                } else if (item == "Sensors") {
                    ret_msg = this->handle_sensors(document);
                } else if (item == "Buttons") {
                    ret_msg = this->handle_buttons(document);
                } else if (item == "Areas") {
                    ret_msg = this->handle_areas(document);
                } else if (item == "Background") {
                    ret_msg = this->handle_background(document);
                } else if (item == "power") {
                    ret_msg = this->refmaster->powermeter->get_json();
                }else if(item == "Mesh"){
                    ret_msg = this->handle_mesh(document);
                }else if(item == "Macros"){
                    ret_msg = this->handle_macro(document);
                }
            }
        } else {
            ret_msg = "NO VALID DATA STRUCT";
            cout << "recieved Garbage" << endl;
        }
    }
    return ret_msg;
}

void *_tcp::run() {

    stream = NULL;
    acceptor = NULL;
    acceptor = new TCPAcceptor(7012);
    cout << "Server started at port 7012" << endl;
    Document document;
    string ret_msg;
    size_t len;
    char line[1024];

    if (acceptor->start() == 0) {
        while (this->exit == false) {
            stream = acceptor->accept();

            if (stream != NULL) {

                try {


                    while ((len = stream->receive(line, sizeof (line))) > 0) {
                        line[len] = 0;
                        //string str = string(line);
                        // cout << "TCP REC:" << line << endl;
                        ret_msg = this->handle_incoming(line);
                        char *returnmsg = (char *) ret_msg.c_str();
                        stream->_send(returnmsg, strlen(returnmsg));
                        // document.Clear();
                    }

                } catch (exception& e) {
                    cout << "TCP ERROR + " + string(e.what()) << endl;
                    WriteError("TCP Loop", e.what());

                }
                delete stream;


            }
        }
    }
    cout << "SERVER STOPPED" << endl;
    return NULL;

    //  perror("Could not start the server");

    //  return NULL;
};

string _tcp::handle_bridge(rapidjson::Value& object) {
    // cout << "Handle Bridge" << endl;
    string action = object["action"].GetString();
    // cout << "action" << endl;
    if (action == "Discover") {
        this->refmaster->hue_bridges->Discover();
        return this->refmaster->hue_bridges->get_json();
    } else if (action == "Get Bridges") {
        string tmp = this->refmaster->hue_bridges->get_json();
        return tmp;
    } else if (action == "Get Bridge") {
        string id = object["id"].GetString();
        Hue_Bridge *br = this->refmaster->hue_bridges->get_bridge(id);
        if (br != NULL) {
            return br->GetJson();
        }
    } else if (action == "Link Bridge") {
        string id = object["id"].GetString();
        Hue_Bridge *br = this->refmaster->hue_bridges->get_bridge(id);
        if (br != NULL) {
            return br->link_bridge();
        }
    }


    return "{\"error\":\"FALSE REQUEST\"}";
};

string _tcp::handle_lights(Value &v) {
    // cout << "Handle Lights" << endl;
    string action = v["action"].GetString();
    // cout << "action" << endl;
    if (action == "Get Lights") {
        string filename = "/var/www/html/lights.json";
        if (remove(filename.c_str()) != 0) {
            int bp = 0;
            bp++;
        }
        ofstream outfile;
        outfile.open(filename);
        // cout << "Writing to the lights file" << endl;
        outfile << this->refmaster->lights->GetJson() << endl;
        outfile.close();
        return "OK";
    }
    if (action == "Set Preview") {
        string id = v["id"].GetString();
        Light *l = this->refmaster->lights->getlight(id);
        if (l != NULL) {
             l->set_preview(v["value"]);
            return "OK";
        }
    }
    if (action == "Identify") {
        string id = v["ID"].GetString();
        Light *l = this->refmaster->lights->getlight(id);
        if (l != NULL) {
            l->Identify();
            return "OK";
        }
    }

    return "NOK";

};

string _tcp::handle_sensors(Value &v) {

    string action = v["action"].GetString();

    if (action == "Get Sensors") {
        string filename = "/var/www/html/sensors.json";
        if (remove(filename.c_str()) != 0) {
            int bp = 0;
            bp++;
        }
        ofstream outfile;
        outfile.open(filename);
        // cout << "Writing to the sensor file" << endl;
        outfile << this->refmaster->sensors->GetJson() << endl;
        outfile.close();
        return "OK";
    }
    if (action == "Identify") {
        string id = v["ID"].GetString();
        Sensor *s = this->refmaster->sensors->get_sensor(id);
        if (s != NULL) {
            s->Identify();
            return "OK";
        }
    }
    return "NOK";
};

string _tcp::handle_buttons(Value &v) {

    string action = v["action"].GetString();
    if (action == "Get Buttons") {
        string filename = "/var/www/html/buttons.json";
        if (remove(filename.c_str()) != 0) {
            int bp = 0;
            bp++;
        }
        ofstream outfile;
        outfile.open(filename);
        // cout << "Writing to the sensor file" << endl;

        outfile << this->refmaster->buttons->GetJson() << endl;
        outfile.close();
        return "OK";
    }
    if (action == "Get Button") {
        string id = v["ID"].GetString();
        Button *b = this->refmaster->buttons->get_button(id);
        if (b != NULL) {
            Document d;
            d.SetObject();
            d.AddMember("Button", b->GetJsonValue(d.GetAllocator()), d.GetAllocator());
            string ret = GetJsonString(d);
            return ret;
        } else {
            return "NOK";
        }
    }
    if (action == "Save Button") {
        string id = v["ID"].GetString();
        string name = v["Name"].GetString();
        Button *b = this->refmaster->buttons->get_button(id);
        if (b != NULL) {
            b->save_button(name, v);
            return "OK";
        } else {
            return "NOK";
        }
    }

    if (action == "Start Detect") {
        this->refmaster->buttons->start_detect();
        return "OK";
    };
    if (action == "Request Detect") {
        return this->refmaster->buttons->GetJsonDetect();
    }
    if (action == "Cancel Detect") {
        this->refmaster->buttons->cancel_detect();
        return "OK";
    }
};

string _tcp::handle_areas(Value &v) {

    string action = v["action"].GetString();
    if (action == "Get Areas") {
        string filename = "/var/www/html/areas.json";
        if (remove(filename.c_str()) != 0) {
            int bp = 0;
            bp++;
        }
        ofstream outfile;
        outfile.open(filename);
        // cout << "Writing to the sensor file" << endl;

        outfile << this->refmaster->areas->GetJson() << endl;
        outfile.close();
        return "OK";
    }
    if (action == "New Area") {
        string name = v["Name"].GetString();
        this->refmaster->areas->add_area(name);
        return "OK";
    }
    if (action == "Get Area") {
        int msid = v["ID"].GetInt();
        Area *a = this->refmaster->areas->get_area(msid);
        if (a != NULL) {
            Document d;
            d.SetObject();
            d.AddMember("Area", a->GetJsonValue(d.GetAllocator()), d.GetAllocator());
            string ret = GetJsonString(d);
            return ret;
        } else {
            return "NOK";
        }
    }
    if (action == "Update Area") {
        int msid = v["ID"].GetInt();
        Area *a = this->refmaster->areas->get_area(msid);
        if (a != NULL) {
            Value &data = v["Data"];
            return a->save_settings(data);
        } else {
            return "NOK";
        }
    }
    if (action == "Get Lights") {
        int msid = v["ID"].GetInt();
        Area *a = this->refmaster->areas->get_area(msid);
        if (a != NULL) {
            return a->GetJsonLights();
        } else {
            return "NOK";
        }
    }
    if (action == "Toggle Light") {
        int msid = v["ID"].GetInt();
        string lightID = v["LightID"].GetString();
        Area *a = this->refmaster->areas->get_area(msid);
        if (a != NULL) {
            return a->toggle_light(lightID);
        } else {
            return "NOK";
        }
    }
    if (action == "Get Sensors") {
        int msid = v["ID"].GetInt();
        Area *a = this->refmaster->areas->get_area(msid);
        if (a != NULL) {
            string tmp = a->GetJsonSensors();
            return tmp;
        } else {
            return "NOK";
        }
    }
    if (action == "Toggle Sensor") {
        int msid = v["ID"].GetInt();
        string SensorID = v["SensorID"].GetString();
        Area *a = this->refmaster->areas->get_area(msid);
        if (a != NULL) {
            return a->toggle_sensor(SensorID);
        } else {
            return "NOK";
        }
    }
    if (action == "Set Surround") {
        int msid = v["ID"].GetInt();
        int surid = v["SurID"].GetInt();
        int level = v["Level"].GetInt();
        Area *a = this->refmaster->areas->get_area(msid);
        if (a != NULL) {
            return a->set_surrounding(surid, level);
        } else {
            return "NOK";
        }
    }
    if (action == "Set Int") {
        int msid = v["ID"].GetInt();
        int brightness = v["Brightness"].GetInt();
        int fade = v["Fade"].GetInt();
        Area *a = this->refmaster->areas->get_area(msid);
        if (a != NULL) {
            a->set_brightness(brightness, fade,0, "From TCP");
            return "OK";
        } else {
            return "NOK";
        }
    }
    
    if(action == "UpdateTemps"){
        int msid = v["ID"].GetInt();
        Area *a = this->refmaster->areas->get_area(msid);
        if (a != NULL) {
            a->LoadSetpoints();
            return "OK";
        } else {
            return "NOK";
        } 
    } 
    
     if (action == "Get Scene Values") {
        int areaid = v["ID"].GetInt();
        int Sceneid = v["SceneID"].GetInt();
        Area *a = this->refmaster->areas->get_area(areaid);
        if(a != NULL){
            return a->Get_Scene_Values(Sceneid);
        } 
    }
    
     if (action == "Save Scene") {
        int areaid = v["ID"].GetInt();
        int sceneid = v["SceneID"].GetInt();
        string name = v["name"].GetString();
        Value &value = v["Value"];
        Area *a = this->refmaster->areas->get_area(areaid);
        if(a != NULL){
           return inttostring(a->Save_Scene_Values(sceneid, name, value));
        }
       // this->refmaster->background->Save_Scene_Values(slotid, sceneid, value);
        return "NOK";
    }
    
    if(action == "Set Global Heating"){
        bool value = v["state"].GetBool();
        this->refmaster->heating_enabled = value;
        string filename = "/var/www/html/areas.json";
        if (remove(filename.c_str()) != 0) {
            int bp = 0;
            bp++;
        }
        ofstream outfile;
        outfile.open(filename);
        // cout << "Writing to the sensor file" << endl;

        outfile << this->refmaster->areas->GetJson() << endl;
        outfile.close();
        return "OK";
    }
    
    if(action == "Set Manual Heating"){
        int AreaID = v["AreaID"].GetInt();
        float value = v["val"].GetFloat();
        Area *a = this->refmaster->areas->get_area(AreaID);
        if(a != NULL){
            a->Set_Temp_Manual(value);
        }
         string filename = "/var/www/html/areas.json";
        if (remove(filename.c_str()) != 0) {
            int bp = 0;
            bp++;
        }
        ofstream outfile;
        outfile.open(filename);
        // cout << "Writing to the sensor file" << endl;

        outfile << this->refmaster->areas->GetJson() << endl;
        outfile.close();
        return "OK";
    }
    
     if(action == "Set Auto Heating"){
        int AreaID = v["AreaID"].GetInt();  
        Area *a = this->refmaster->areas->get_area(AreaID);
        if(a != NULL){
            a->Set_Temp_Auto();
        }
         string filename = "/var/www/html/areas.json";
        if (remove(filename.c_str()) != 0) {
            int bp = 0;
            bp++;
        }
        ofstream outfile;
        outfile.open(filename);
        // cout << "Writing to the sensor file" << endl;

        outfile << this->refmaster->areas->GetJson() << endl;
        outfile.close();
        return "OK";
    }
    
    if(action == "Get Scenes"){
        int AreaID = v["AreaID"].GetInt();  
        Area *a = this->refmaster->areas->get_area(AreaID);
        if(a != NULL){
            return a->Get_Scenes();
        }
    }
    
    if(action == "Toggle Scene"){
         int AreaID = v["AreaID"].GetInt();  
         int SceneID = v["SceneID"].GetInt();
         Area *a = this->refmaster->areas->get_area(AreaID);
         if(a != NULL){
             a->Toggle_Scene(SceneID);
             return "OK";
            //return a->Get_Scenes();
        }
    }
    
    return "NOK";
};

string _tcp::handle_background(Value &v) {

    string action = v["action"].GetString();

    if (action == "Get Background") {
        string filename = "/var/www/html/background.json";
        if (remove(filename.c_str()) != 0) {
            int bp = 0;
            bp++;
        }
        ofstream outfile;
        outfile.open(filename);
        // cout << "Writing to the sensor file" << endl;
        outfile << this->refmaster->background->GetJson() << endl;
        outfile.close();
        return "OK";
    }
    if (action == "Create Scene") {
        int slotid = v["SlotID"].GetInt();
        string Name = v["Name"].GetString();
        this->refmaster->background->add_slot_scene(slotid, Name);
        string filename = "/var/www/html/background.json";
        if (remove(filename.c_str()) != 0) {
            int bp = 0;
            bp++;
        }
        ofstream outfile;
        outfile.open(filename);
        // cout << "Writing to the sensor file" << endl;
        outfile << this->refmaster->background->GetJson() << endl;
        outfile.close();
        return "OK";
    }
    if (action == "Get Scene Values") {
        int slotid = v["SlotID"].GetInt();
        int Sceneid = v["SceneID"].GetInt();
        string ret = this->refmaster->background->Get_Slot_Scene_Values(slotid, Sceneid);
        return ret;
    }
    if (action == "Save Scene") {
        int slotid = v["SlotID"].GetInt();
        int sceneid = v["SceneID"].GetInt();
        Value &value = v["Value"];
        this->refmaster->background->Save_Scene_Values(slotid, sceneid, value);
        return "OK";
    }

    return "NOK";
};

string _tcp::handle_mesh(rapidjson::Value& object) {
    // cout << "Handle Bridge" << endl;
    string action = object["action"].GetString();
    // cout << "action" << endl;
    if (action == "Get Nodes") {
        return this->refmaster->tree_mesh->GetJson(); 
    }
    
    if(action == "Ident"){
        uint32_t nodeid = object["NodeID"].GetUint();
        this->refmaster->tree_mesh->Ident(nodeid);
        return "ok";
    }
    
    if(action == "New Config"){
        uint32_t nodeid = object["NodeID"].GetUint();
        Tree_Node *n = this->refmaster->tree_mesh->get_Node(nodeid);
        if(n!=NULL){
            n->reload_config();
            return "ok";
        }
    }
    
     if(action == "New Firmware"){         
         this->refmaster->tree_mesh->OTA->load_list();
         return "ok";
    }
    
    if(action == "StartTempCalib"){
        uint32_t nodeid = object["NodeID"].GetUint();
        Tree_Node *n = this->refmaster->tree_mesh->get_Node(nodeid);
        if(n!=NULL){
            n->start_temp_calib();
            return "ok";
        }
    }
    
    return "NOK";
}

string _tcp::handle_macro(rapidjson::Value& object) {
    // cout << "Handle Bridge" << endl;
    string action = object["action"].GetString();
    if(action == "Update Macro"){
        int id = object["ID"].GetInt();
        this->refmaster->macros->reload_macro(id);
        return "OK";        
    }
    if(action == "Delete Macro"){
        int id = object["ID"].GetInt();
        this->refmaster->macros->delete_macro(id);
        return "OK";  
    }
    if(action == "Run Macro"){
        int id = object["ID"].GetInt();
        this->refmaster->macros->start_macro(id);
        return "OK";
    }
    return "NOK";
}

 

