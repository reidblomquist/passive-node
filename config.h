//////////////////////////////////////////////////////
// CONFIGURATION FILE
// Change the configurations below depending on your specific node setup.

// Soil Moisture Settings
// Pins to read moisture on
#define TOTAL_SENSORS_SOIL  2
byte soil_sensor_pins[TOTAL_SENSORS_SOIL] = {32, 33};

// Battery Voltage Sensor Settings
// Pins to read battery voltage on
#define TOTAL_SENSORS_VOLTAGE  1
byte voltage_sensor_pins[TOTAL_SENSORS_VOLTAGE] = {36};

// Wifi Settings
char* ssid = "Infected Network Detected";
char* password =  "DeaDlyDavisson";
char* database_endpoint = "http://192.168.2.217/ajax/sprout.php";

#define TIME_TO_SLEEP  60        /* sleep (in seconds) */

#define TOTAL_SENSORS TOTAL_SENSORS_SOIL + TOTAL_SENSORS_VOLTAGE
