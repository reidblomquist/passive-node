#include "config.h"
#include "system.h"

#include <esp_adc_cal.h>
#include <esp32-hal-cpu.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "Sensor.h"
#include "SensorReading.h"
#include "VoltageSensor.h"
#include "SoilSensor.h"
#include "NetworkController.h"
#include "deep_sleep.h"

//////////////////////////////////////////
// Sprout Passive Node Firmware v0.6
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
RTC_NOINIT_ATTR int totalReadingsCount = 0;
RTC_NOINIT_ATTR int failedReadingsIndex = 0;
RTC_NOINIT_ATTR int failedReadingsCount = 0;

typedef struct {
    int type; // Sensor Reading Type
    byte source; // Node Id
    int value;
    int boot;
    float value_parsed;
} reading_t;

RTC_DATA_ATTR reading_t failed_readings[FAILED_READINGS_LIMIT];

/////////////////////////////////////
// Sensors
int readingsCount = 0;
Sensor * sensors[TOTAL_SENSORS];

SensorReading last_reading;
SensorReading readings[MAX_SAVED_READINGS];


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
  
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.print("Reset Reason: ");
  Serial.println(reason);
  if ((reason != ESP_RST_DEEPSLEEP) && (reason != ESP_RST_SW)) {
    bootCount = 0;
    totalReadingsCount = 0;
    failedReadingsIndex = 0;
    failedReadingsCount = 0;
  }

  Serial.println("");
  Serial.println("----------");
  Serial.println("Sprout Booting...");
  Serial.print("Sprout ID: ");
  Serial.println(DEVICE_ID);

  setCpuFrequencyMhz(80);
  analogReadResolution(11);

  // Calibration function
  esp_adc_cal_value_t val_type =
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_11, REF_VOLTAGE, adc_chars);
  
  ++bootCount;
  Serial.println("Boot cycle: " + String(bootCount));
  Serial.println("Failed Reading current index: " + String(failedReadingsIndex));
  Serial.println("Total Failed Reading count: " + String(failedReadingsCount));
  Serial.println("Total Readings count: " + String(totalReadingsCount));
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

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  
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

  
  // Check for failed readings that need resending
  if(failedReadingsCount > 0) {
    int bootCycle = 0;
    for(byte i; i < failedReadingsCount; i++) {
      
      byte currentIndex = failedReadingsIndex + i;
      if (currentIndex > (FAILED_READINGS_LIMIT - 1)) {
        currentIndex = i - failedReadingsIndex;
      }

      if (bootCycle == 0) {
        bootCycle = failed_readings[currentIndex].boot;
      }

      // Detect new batch of failed readings
      if (bootCycle != failed_readings[currentIndex].boot) {
        
      }
      
      last_reading = SensorReading(failed_readings[currentIndex].type, failed_readings[currentIndex].value, failed_readings[currentIndex].source, failed_readings[currentIndex].value_parsed, failed_readings[currentIndex].boot);
      
      network.add_reading(last_reading);
    }

    if(network.connect()) {
      if(network.send_data(bootCount)) {
        Serial.println("Failed Data resent successfully");
        failedReadingsCount = 0;
        failedReadingsIndex = 0;
      }
    }
    delay(2000);
  }

  
  // Sensor Read Loop
  for(byte i = 0; i < TOTAL_SENSORS; i++) {
    // sensors[i]->display();
    Serial.print("Sensor Reading: ");
    Serial.println(sensors[i]->read());
    
    last_reading = sensors[i]->reading();
    last_reading._boot = bootCount;
    // last_reading.display();
    
    readings[readingsCount] = last_reading;
    readingsCount++;
    totalReadingsCount++;
    if (readingsCount >= MAX_SAVED_READINGS) {
      readingsCount = MAX_SAVED_READINGS;
    }

    network.add_reading(last_reading);
    
    delay(500);
  }

  delay(500); // wait for power to stablize
  
  if(network.connect()) {
    if(network.send_data(bootCount)) {
      Serial.println("Data sent successfully");
      sent_successfully = true;
    }
  }

  if(!sent_successfully) {
    Serial.println("Data Failed to Send to Database!");

    /////////////////////////////////////////////
    //Deep Sleep Sensor Failed Reading Logs
    
    for(byte i = 0; i <= readingsCount; i++) {
      reading_t failed_reading;
      
      failed_reading.value = readings[i]._value;
      failed_reading.value_parsed = readings[i]._value_parsed;
      failed_reading.type = readings[i]._type;
      failed_reading.boot = readings[i]._boot;
      failed_reading.source = DEVICE_ID;
      
      failed_readings[failedReadingsIndex] = failed_reading;
      
      //print_reading(failed_reading);
      
      failedReadingsCount++;
      failedReadingsIndex++;
      
      if (failedReadingsIndex >= FAILED_READINGS_LIMIT - 1) {
        failedReadingsIndex = 0; // Loop to the start of array and overwrite oldest data first
      }
      if (failedReadingsCount >= FAILED_READINGS_LIMIT - 1) {
        failedReadingsCount = FAILED_READINGS_LIMIT - 1;
      }
    }
    ////////////////////////////////////////////////
    
  }
  network.disconnect();
  Serial.println("----------");
  enter_deep_sleep();
}
