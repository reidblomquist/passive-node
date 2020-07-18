#include "Arduino.h"
#include <esp_adc_cal.h>
#include "system.h"
#include "VoltageSensor.h"

VoltageSensor::VoltageSensor()
{
  this->atten = ADC_ATTEN_DB_0;
  this->battery_min = BATTERY_MIN;
  this->battery_max = BATTERY_MAX;
  this->type = READING_TYPE_VOLTAGE;
  this->source = DEVICE_ID;
}
VoltageSensor::VoltageSensor(byte pin)
{
  this->pin = pin;
  this->analog = true;
  this->atten = ADC_ATTEN_DB_0;
  this->battery_min = BATTERY_MIN;
  this->battery_max = BATTERY_MAX;
  this->type = READING_TYPE_VOLTAGE;
  this->source = DEVICE_ID;
}
VoltageSensor::VoltageSensor(byte pin, esp_adc_cal_characteristics_t *adc_chars)
{
  this->pin = pin;
  this->analog = true;
  this->atten = ADC_ATTEN_DB_0;
  this->adc_chars = adc_chars;
  this->battery_min = BATTERY_MIN;
  this->battery_max = BATTERY_MAX;
  this->type = READING_TYPE_VOLTAGE;
  this->source = DEVICE_ID;
}
void VoltageSensor::display() {
  Serial.println("#####");
  Serial.println("Voltage Sensor");
  Sensor::display();
}

int VoltageSensor::read() {
  //Read voltage a few times as the first reading after sleep result is often garbage
  for (byte i = 0; i < 2; i++) {
      adc1_channel_t channelNum;

      //Set number of cycles per sample 1 - 255
      analogSetCycles(16);
    
      switch (this->pin) {
        case (36):
          channelNum = ADC1_CHANNEL_0;
          break;
    
        case (39):
          channelNum = ADC1_CHANNEL_3;
          break;
      }
    
      adc1_config_channel_atten(channelNum, this->atten);
      this->value = esp_adc_cal_raw_to_voltage(analogRead(pin), adc_chars);
      this->value = this->value / (R2 / (R1 + R2)); //reverse voltage divider
      delay(2000);
  }
  VoltageSensor::parse();
  return this->value;
  
}

float VoltageSensor::parse() {
  float percent = 0.0;
  percent = ((this->value - this->battery_min) / (this->battery_max - this->battery_min)) * 100;
  percent = roundf(percent * 100) / 100;
  if (percent > 100) {
    percent = 100.0f;
  }
  this->value_parsed = percent;
  return percent;
}
