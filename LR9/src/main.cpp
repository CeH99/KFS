#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// --- WIFI CONFIG ---
const char* ssid = "IPZE";
const char* password = "IPZE WIFI Password";

// --- PINS ---
const int PIN_GRID_V = 26;  // ADC0: Simulates Grid Voltage
const int PIN_TRAFFIC = 27; // ADC1: Simulates Traffic Load
const int PIN_LDR = 28;     // ADC2: Light Sensor
const int PIN_LAMP = 15;    // PWM: LED Output

// --- CONSTANTS ---
const int LIGHT_THRESHOLD = 100;
const float MAX_SIM_V = 250.0;
const float MAX_SIM_A = 10.0;

WebServer server(80);

// --- STATE VARIABLES ---
int rawLight = 0;
float voltage = 0;
float traffic = 0;
int brightness = 0; // 0-255
String sysStatus = "Booting";

// --- WEB HANDLER ---
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<meta http-equiv='refresh' content='1'>";
  html += "<style>body{font-family:sans-serif;text-align:center;background:#1a1a1a;color:#fff;margin-top:20px;}";
  html += ".card{background:#333;padding:20px;max-width:350px;margin:auto;border-radius:10px;}";
  html += "h1{color:#f39c12;} .val{color:#3498db;font-weight:bold;font-size:1.2em;}";
  html += ".stat{font-size:1.1em;margin-top:10px;padding:5px;border-radius:4px;}";
  html += "</style></head><body>";

  html += "<div class='card'>";
  html += "<h1>🚦 Smart Street Light</h1>";
  html += "<p>Light Sensor (LDR): <span class='val'>" + String(rawLight) + "</span></p>";
  html += "<p>Grid Voltage: <span class='val'>" + String(voltage, 1) + " V</span></p>";
  html += "<p>Traffic Load: <span class='val'>" + String(traffic, 2) + " A</span></p>";
  html += "<hr>";
  
  // Lamp Status Display
  int pct = map(brightness, 0, 255, 0, 100);
  String color = (brightness > 0) ? "#f1c40f" : "#7f8c8d";
  html += "<p>Lamp Output: <b style='color:" + color + "'>" + String(pct) + "%</b></p>";
  html += "<div class='stat' style='background:" + String(brightness > 0 ? "#27ae60" : "#c0392b") + "'>";
  html += "Status: " + sysStatus + "</div>";
  
  html += "</div></body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LAMP, OUTPUT);
  analogReadResolution(12); // 12-bit ADC (0-4095)
  analogWriteRange(255);    // PWM range

  // WiFi Init
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();

  // 1. Read Sensors
  rawLight = analogRead(PIN_LDR);
  int rawV = analogRead(PIN_GRID_V);
  int rawT = analogRead(PIN_TRAFFIC);

  // 2. Convert Data
  voltage = (rawV / 4095.0) * MAX_SIM_V;
  traffic = (rawT / 4095.0) * MAX_SIM_A;

  // 3. Control Logic
  if (voltage < 180.0 || voltage > 245.0) {
    // Safety Cutoff
    brightness = 0;
    sysStatus = "GRID ERROR (Voltage)";
  } 
  else if (rawLight >= LIGHT_THRESHOLD) {
    // Day Mode
    brightness = 0;
    sysStatus = "Daytime (Standby)";
  } 
  else {
    // Night Mode (Adaptive)
    sysStatus = "Night Mode (Adaptive)";
    // Map Traffic (0-10A) to Brightness (50-255)
    brightness = map(rawT, 0, 4095, 50, 255);
  }

  // 4. Output
  analogWrite(PIN_LAMP, brightness);
  delay(100);
}