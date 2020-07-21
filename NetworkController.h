#ifndef _NETWORKCONTROLLER_H
#define _NETWORKCONTROLLER_H

#include "SensorReading.h"
#include <ArduinoJson.h>

class NetworkController
{
  public:
    NetworkController();
    NetworkController(char* ssid, char* password);
    NetworkController(char* ssid, char* password, char* endpoint);
    bool connect();
    bool disconnect();
    bool setup();
    bool send_data(int bootCount);
    DynamicJsonDocument prepare_json(int bootCount);
    bool add_reading(SensorReading reading);
    char* ssid;
    char* password;
    char* endpoint;
    SensorReading readings[20];
    int readingCount;
    bool connected;
  private:
    int _id;
};

#endif
