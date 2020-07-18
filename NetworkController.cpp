#include "Arduino.h"
#include "NetworkController.h"

#include "system.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

NetworkController::NetworkController()
{
  this->readingCount = 0;
}
NetworkController::NetworkController(char* ssid, char* password)
{
  this->ssid = ssid;
  this->password = password;
  this->readingCount = 0;
}
NetworkController::NetworkController(char* ssid, char* password, char* endpoint)
{
  this->ssid = ssid;
  this->password = password;
  this->endpoint = endpoint;
  this->readingCount = 0;
}
bool NetworkController::connect() {
  int count = 0;
  int ip_count = 0;
  // it wil set the static IP address to 192, 168, 1, 184
  IPAddress null_IP(0, 0, 0, 0);
  IPAddress ip = null_IP;
  
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password); 
 // waitForConnectResult() != WL_CONNECTED;
  while (WiFi.waitForConnectResult() != WL_CONNECTED && ip == null_IP && count < CONNECTION_TRY_LIMIT) { //Check for the connection
    delay(1000);
    switch (WiFi.status()) {
      case WL_CONNECTED:
        Serial.println("Connection Success");
        break;
      case WL_IDLE_STATUS:
        Serial.println("Idle Waiting Connection");
        break;
      case WL_NO_SSID_AVAIL:
        Serial.println("No SSID Available");
        break;
      case WL_SCAN_COMPLETED:
        Serial.println("Scan Completed");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("Connetion Failed");
        break;
      case WL_CONNECTION_LOST:
        Serial.println("Connetion Lost");
        break;
      case WL_DISCONNECTED:
        Serial.println("Connetion Disconnected");
        break;
    }
    ++count;
    
    ip = WiFi.localIP();
    if(count == 1) {
      Serial.print("Waiting for Connection...");
    }
    else {
//      Serial.print(".");
    }

    if(count % 10 == 0) {
      Serial.print("Reseting WiFi...");
      WiFi.disconnect();
      delay(2000);
      WiFi.begin(ssid, password); 
      delay(1000);
      ip = WiFi.localIP();
    }
  }
  
  
 // Check if WiFi was successful 
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to the WiFi network");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  }
  else {
    Serial.println("Failed to Connect to WiFi!");
    return false;
  }
}

bool NetworkController::setup() {
  WiFi.disconnect(); //Flush WiFi
  return true;
}

bool NetworkController::send_data(int bootCount) {
  Serial.println("Sending readings to server...");
  // Block until we are able to connect to the WiFi access point
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;   
     
    http.begin(this->endpoint);  
    http.addHeader("Content-Type", "application/json");      
    http.addHeader("Accept", "application/json");         

    //PACKAGE DATA
    
    String requestBody;
    serializeJson(NetworkController::prepare_json(bootCount), requestBody);

    int send_attempts = 0;

    // Try to post the data up to three times
    int httpResponseCode = 0;
    while(send_attempts < 3 && httpResponseCode != 200) {
      httpResponseCode = http.POST(requestBody);
      send_attempts++;
   
      if(httpResponseCode>0) {
        String response = http.getString();                       
        Serial.print("Response: ");
        Serial.println(httpResponseCode);   
        Serial.println(response);
      }
      else {
        Serial.printf("Error occurred while sending HTTP POST: %s\n", http.errorToString(httpResponseCode).c_str());
        delay(3000);
      }
    
    }
    // Add failed request data to array
    if(send_attempts > 3 && httpResponseCode != 200) {
      Serial.println("Failed to Send to Database!");
      http.end();
      return false;
    }
    
    http.end();
    this->readingCount = 0;
    return true;
     
  }
}

bool NetworkController::add_reading(SensorReading new_reading) {
  this->readings[this->readingCount] = new_reading;
  this->readingCount++;
  if (this->readingCount >= 19) {
    this->readingCount = 19;
  }
  return true;
}

DynamicJsonDocument NetworkController::prepare_json(int bootCount) {
  size_t capacity = JSON_ARRAY_SIZE(this->readingCount) + JSON_OBJECT_SIZE(3) + this->readingCount*JSON_OBJECT_SIZE(4);
  DynamicJsonDocument doc(capacity);

  // StaticJsonDocument<500> doc;
  // Add values in the document
  
  doc["source"] = DEVICE_ID;
  doc["boots"] = bootCount;
  
  // Add an array of readings.
  JsonArray data = doc.createNestedArray("value");

  for(byte i = 0; i < this->readingCount; i++) {
    JsonObject data_0 = data.createNestedObject();
    data_0["value"] = readings[i]._value;
    data_0["parsed"] = readings[i]._value_parsed;
    data_0["boots"] = readings[i]._boot;
    switch(readings[i]._type) {
      case READING_TYPE_VOLTAGE:
        data_0["sensor"] = "voltage";
        break;
      case READING_TYPE_SOIL:
        data_0["sensor"] = "soil";
        break;
      case READING_TYPE_TEMPERATURE:
        data_0["sensor"] = "temperature";
        break;
      case READING_TYPE_HUMIDITY:
        data_0["sensor"] = "humidity";
        break;
      case READING_TYPE_RAIN:
        data_0["sensor"] = "rain";
        break;
      case READING_TYPE_PRESSURE:
        data_0["sensor"] = "pressure";
        break;
      case READING_TYPE_FLOAT:
        data_0["sensor"] = "float";
        break;
      case READING_TYPE_PH:
        data_0["sensor"] = "ph";
        break;
      case READING_TYPE_EC:
        data_0["sensor"] = "ec";
        break;
      default:
        data_0["sensor"] = "unknown";
        break;
    }
  }

  return doc;
  
}
