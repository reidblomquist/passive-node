#include "config.h"
#include "system.h"

#include <esp_adc_cal.h>
#include "Sensor.h"
#include "SensorReading.h"
#include "VoltageSensor.h"
#include "SoilSensor.h"
#include "NetworkController.h"
#include "deep_sleep.h"

//////////////////////////////////////////
// Sprout Passive Node Firmware v0.5
//////////////////////////////////////////
// A ESP32 firmware to monitor battery voltage and take sensor readings.
// The unit is powered off a battery with solar charging and spends most
// of the time in deep sleep. Change the settings below based on components. 
// Author: Eric Davisson
// Website: MudPi.app
////////////////////////////////////////////
// Edit the settings in the config.h file //


/////////////////////////////////////////////////////////
// Do not change these unless you know what you are doing!
////////////////////////////////////////////////////


////////////////////////////////////////
// SYSTEM VARIABLES
int last_value = 0;
int sensorCount = 0;

esp_adc_cal_characteristics_t *adc_chars = new esp_adc_cal_characteristics_t;

NetworkController network;

//////////////////////////
// Deep Sleep Data 
//Variable to survive deep sleep
RTC_NOINIT_ATTR int bootCount = 0;
RTC_NOINIT_ATTR int readingsCount = 0;
RTC_NOINIT_ATTR int readingsIndex = 0;
RTC_DATA_ATTR int failedReadingsCount = 0;

typedef struct {
    int type; // Sensor Reading Type
    byte source; // Node Id
    int value;
    int boot;
} reading_t;

RTC_DATA_ATTR reading_t saved_readings[MAX_SAVED_READINGS];

/////////////////////////////////////
// Sensors
Sensor * sensors[TOTAL_SENSORS];

SensorReading last_reading;
SensorReading readings[FAILED_JOBS_LIMIT];


////////////////////////
// Data Controls
void print_reading(reading_t reading) {
  Serial.print("Name: Sprout-");
  Serial.println(reading.source);
  Serial.print("Type: ");
  Serial.println(reading.type);
  Serial.print("Value: ");
  Serial.println(reading.value);
  Serial.print("Boot: ");
  Serial.println(reading.boot);
}



////////////////////////
// Main Program Loop
//////////////////
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("");
  Serial.println("----------");
  Serial.println("Sprout Booting...");
  Serial.print("Sprout ID: ");
  Serial.println(DEVICE_ID);
  
  analogReadResolution(11);

  // Calibration function
  esp_adc_cal_value_t val_type =
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_11, REF_VOLTAGE, adc_chars);
  
  ++bootCount;
  Serial.println("Boot cycle: " + String(bootCount));
  Serial.println("Reading index: " + String(readingsIndex));
  Serial.println("Reading count: " + String(readingsCount));
  Serial.println("");
  
  //print_wakeup_reason();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  //ESP_RST_UNKNOWN
  //ESP_RST_POWERON
  //ESP_RST_EXT
  //ESP_RST_SW
  //ESP_RST_PANIC
  //ESP_RST_INT_WDT
  //ESP_RST_TASK_WDT
  //ESP_RST_WDT
  //ESP_RST_DEEPSLEEP
  //ESP_RST_BROWNOUT
  //ESP_RST_SDIO
  
  esp_reset_reason_t reason = esp_reset_reason();
  if ((reason != ESP_RST_DEEPSLEEP) && (reason != ESP_RST_SW)) {
    bootCount = 1;
    readingsCount = 0;
    readingsIndex = 0;
  }

  // Initialize the sensors
  for(byte i = 0; i < TOTAL_SENSORS_VOLTAGE; i++) {
    sensors[sensorCount] = new VoltageSensor(voltage_sensor_pins[i], adc_chars);
    sensorCount++;
  }
  
  for(byte i = 0; i < TOTAL_SENSORS_SOIL; i++) {
    sensors[sensorCount] = new SoilSensor(soil_sensor_pins[i]);
    sensorCount++;
  }

  

  network = NetworkController(ssid, password, database_endpoint);
  if(network.setup()) {
    Serial.println("Network Controller Online");
  }
  
  delay(2000);
}

// Loop only runs once due to sleep
void loop() {

  bool sent_successfully = false;
  
  // Sensor Read Loop
  for(byte i = 0; i < TOTAL_SENSORS; i++) {
    sensors[i]->display();
    
    /////////////////////////////////////////////
    //Deep Sleep Sensor Reading Logs
    reading_t new_reading;
    
    new_reading.value = sensors[i]->read();
    new_reading.type = sensors[i]->type;
    new_reading.source = DEVICE_ID;
    new_reading.boot = bootCount;
    
    saved_readings[readingsIndex] = new_reading;
    //print_reading(new_reading);
    readingsCount++;
    readingsIndex++;
    if (readingsIndex >= MAX_SAVED_READINGS - 1) {
      readingsIndex = 0; // Loop to the start of array and overwrite oldest data first
    }
    if (readingsCount >= MAX_SAVED_READINGS - 1) {
      readingsCount = MAX_SAVED_READINGS - 1; // Loop to the start of array and overwrite oldest data first
    }
    ////////////////////////////////////////////////
    
    last_reading = sensors[i]->reading();
    last_reading._boot = bootCount;
    last_reading.display();
    
    Serial.println("");
    
    network.add_reading(last_reading);
    
    delay(500);
  }
  
  if(network.connect()) {
    if(network.send_data(bootCount)) {
      Serial.println("Data sent successfully");
      sent_successfully = true;
    }
  }

  if(!sent_successfully) {
    Serial.println("Data Failed to Send to Database!");
    readings[failedReadingsCount] = last_reading;
    failedReadingsCount++;
    if (failedReadingsCount >= FAILED_JOBS_LIMIT) {
      failedReadingsCount = FAILED_JOBS_LIMIT;
    }
  }

  Serial.println("----------");
  enter_deep_sleep();
}
