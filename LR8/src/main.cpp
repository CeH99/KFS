#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

#include "S:\PlatformIO\LR7\lib\LightSensor.h"

// --- WiFi & Firebase Credentials ---
const char* WIFI_SSID = "molniya1";
const char* WIFI_PASS = "140578875041";
const String FIREBASE_HOST = "lr-8-20a0d-default-rtdb.firebaseio.com";
const String FIREBASE_AUTH = "UI91aiNZ8OwCeWGGqGaFsX37YWt1ZDlvkd3gw8Go";
const String FIREBASE_PATH = "/pico_data";

// --- Global Variables ---
unsigned long lastSendTime = 0;
LightSensor lightSensor(A1);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() - lastSendTime > 10000) {
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Sending data to Firebase...");

      // Creating json
      StaticJsonDocument<200> jsonDoc;
      jsonDoc["board_name"] = "Pico W";
      jsonDoc["sensor_reading"] = lightSensor.getValue();
      jsonDoc["time"] = millis();

      String jsonPayload;
      serializeJson(jsonDoc, jsonPayload);

      WiFiClientSecure client;
      HTTPClient http;

      //disabling certificate verification
      client.setInsecure(); 

      String url = "https://" + FIREBASE_HOST + FIREBASE_PATH + ".json?auth=" + FIREBASE_AUTH;
      Serial.print("Request URL: ");
      Serial.println(url);

      // Initiating PUT request
      if (http.begin(client, url)) { 
        http.addHeader("Content-Type", "application/json");

        int httpResponseCode = http.POST(jsonPayload);

        if (httpResponseCode > 0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String response = http.getString();
          Serial.println(response);
        } else {
          Serial.print("Error on sending PUT: ");
          Serial.println(httpResponseCode);
        }
        
        http.end();
      
      } else {
        Serial.println("HTTP.begin(client, url) failed!");
      }

      lastSendTime = millis();

    } else {
      Serial.println("WiFi Disconnected. Reconnecting...");
      WiFi.begin(WIFI_SSID, WIFI_PASS);
    }
  }
}