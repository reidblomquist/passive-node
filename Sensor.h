#ifndef _SENSOR_H
#define _SENSOR_H

#include "SensorReading.h"

class Sensor
{
  public:
    Sensor();
    Sensor(byte pin);
    virtual int read();
    virtual void display();
    SensorReading reading();
    byte pin;
    int type;
    float value;
    int source;
    float value_parsed;
    bool analog;
    SensorReading last_reading;
  private:
    int _id;
};

#endif
