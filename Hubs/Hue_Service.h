/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Hue_Service.h
 * Author: Fedde
 *
 * Created on 13 februari 2022, 9:20
 */

#ifndef HUE_SERVICE_H
#define HUE_SERVICE_H

#include "../Global.h" 

 enum SERVICE_TYPE{
      bridge_home,
      room,
      zone,
      light,
      button,
      temperature,
      light_level,
      motion,
      entertainment,
      grouped_light,
      device_power,
      zigbee_bridge_connectivity,
      zigbee_connectivity,
      zgp_connectivity,
      bridge,
      homekit,
      scene,
      entertainment_configuration,
      public_image,
      auth_v1,
      behavior_script,
      behavior_instance,
      geofence,
      geofence_client,
      geolocation   
    };

class Hue_Service {
public:
   
    Hue_Service(int service_type, string service_id);
    Hue_Service(const Hue_Service& orig);
    virtual ~Hue_Service();
    
    int service_type;
    string service_id;
    time_t last_update;
    
    
private:
   

};

#endif /* HUE_SERVICE_H */

