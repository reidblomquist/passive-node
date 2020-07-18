//////////////////////////////////////////////////////
// SYSTEM CONFIGURATION FILE
// Change the configurations below depending on your specific node setup.

// Run this command to see the REF_VOLTAGE: "espefuse.py --port COM3 adc_info" 
#define REF_VOLTAGE 1128
#define CONNECTION_TRY_LIMIT 30 // Number of cycles to try WiFi connection 
#define MAX_SAVED_READINGS    50 // How many max previous readings to backup
#define FAILED_JOBS_LIMIT 20 // number of failed jobs to store

//////////////////////////////////////////
// General Settings
#define DEVICE_ID      6


#define BATTERY_MAX   4100 //maximum voltage of bcmdattery 4.2v
#define BATTERY_MIN   2800 //minimum voltage of battery before shutdown
// Battery Voltage Divider Resistor Values
const float R1 = 100000; // resistance of R1 (100K)
const float R2 = 22000;  // resistance of R2 (22K)



/////////////////////////////////////////
// SYSTEM SETTINGS
// Do not change these unless you know what you are doing!

// Reading Type Constants to Save Memory
#define READING_TYPE_VOLTAGE      0
#define READING_TYPE_SOIL         1
#define READING_TYPE_TEMPERATURE  2
#define READING_TYPE_HUMIDITY     3
#define READING_TYPE_RAIN         4
#define READING_TYPE_PRESSURE     5
#define READING_TYPE_FLOAT        6
#define READING_TYPE_PH           7
#define READING_TYPE_EC           8

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
