#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <TimeLib.h>

#include "S:\PlatformIO\LR7\lib\LightSensor.h"
#include "S:\PlatformIO\LR7\lib\SoundSensor.h"

#define DEBUG

const char* ssid = "IPZE";
const char* password = "IPZE WIFI Password";

const String FIREBASE_HOST = "lr-8-20a0d-default-rtdb.firebaseio.com";
const String FIREBASE_AUTH = "UI91aiNZ8OwCeWGGqGaFsX37YWt1ZDlvkd3gw8Go";
const String FIREBASE_PATH = "/pico_data";

#define WIFI_RETRY_INTERVAL 5000
unsigned long lastWiFiCheck = 0;
unsigned long lastSendTime = 0;

WebServer server(80);
LightSensor lightSensor(27);
SoundSensor soundSensor(26);

// ======================= Wi-Fi Handling ====================
void ensureWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    unsigned long now = millis();
    if (now - lastWiFiCheck >= WIFI_RETRY_INTERVAL) {
#ifdef DEBUG
      Serial.println("Reconnecting to WiFi...");
#endif
      WiFi.disconnect();
      WiFi.begin(ssid, password);
      lastWiFiCheck = now;
    }
  }
}

// ======================= Обробник Web-запиту =========================
void handleData() {

  String json = "{";
  json += "\"light\": " + String(lightSensor.getValue());
  json += ", \"sound\": " + String(soundSensor.getValue());
  json += "}";

  server.send(200, "application/json", json);
#ifdef DEBUG
  Serial.printf("Data sent: %s\n", json.c_str());
#endif
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not Found");
}

void handleFireBase();

// ======================== Setup ============================
void setup() {
#ifdef DEBUG
  Serial.begin(115200); 
  delay(500);
  Serial.println("Pico W ANALOG Sensor Server starting...");
#endif
  // hour minute second day month year
  setTime(10, 35, 0, 21, 11, 2025);
  lightSensor.begin();
  soundSensor.begin();
#ifdef DEBUG
  Serial.println("Analog sensors ready.");
#endif

  WiFi.begin(ssid, password);
#ifdef DEBUG
  Serial.print("Connecting to WiFi...");
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
#ifdef DEBUG
    Serial.print(".");
#endif
  }
#ifdef DEBUG
  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
#endif

  server.on("/data", handleData); 
  server.onNotFound(handleNotFound);
  server.begin();
#ifdef DEBUG
  Serial.println("Web server ready");
#endif
}

// ============================== Loop =====================
void loop() {
  server.handleClient();
  handleFireBase();
  ensureWiFi();
}

void handleFireBase() {
  if (millis() - lastSendTime > 10000) {
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Sending data to Firebase...");

      // Creating json
      StaticJsonDocument<200> jsonDoc;
      jsonDoc["Sound_sensor"] = soundSensor.getValue();
      jsonDoc["Light_sensor"] = lightSensor.getValue();
      jsonDoc["time"] = String(hour()) + ":" + String(minute()) + ":" + String(second());

      String jsonPayload;
      serializeJson(jsonDoc, jsonPayload);

      WiFiClientSecure client;
      HTTPClient http;

      //disabling certificate verification ???
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
    }
  }
}