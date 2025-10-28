#include <Arduino.h>
#include <WebServer.h>
#include <Wire.h>
#include <WiFi.h>

#define DEBUG

const char* ssid = "IPZE";
const char* password = "IPZE WIFI Password";

WebServer server(80);

#define I2C_SDA 4
#define I2C_SCL 5
#define ARDUINO_ADDR 0x08
#define WIFI_RETRY_INTERVAL 5000

unsigned long lastWiFiCheck = 0;

// ====== HTML Page ======
String htmlPage() {
  String page = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  page += "<title>Servo Control</title></head>";
  page += "<body style='text-align:center;font-family:sans-serif'>";
  page += "<h1>Pico W → Arduino</h1>";
  page += "<p>Press a button to send a command via I2C</p>";
  page += "<form action='/left'><button style='padding:20px 40px;font-size:22px;background:blue;color:white;border:none;border-radius:12px;'>⬅ Left</button></form><br>";
  page += "<form action='/right'><button style='padding:20px 40px;font-size:22px;background:green;color:white;border:none;border-radius:12px;'>Right ➡</button></form>";
  page += "</body></html>";
  return page;
}

// ======================= I2C Command =========================
bool sendCommand(char cmd) {
  Wire.beginTransmission(ARDUINO_ADDR);
  Wire.write(cmd);
  byte err = Wire.endTransmission();
#ifdef DEBUG
  Serial.printf("Sent %c, I2C result = %d\n", cmd, err); //! 0 = OK
#endif
  return (err == 0);
}

// ============================Web Handlers =======================
void handleRoot() {
  server.send(200, "text/html", htmlPage());
}

void handleLeft() {
  sendCommand('L');
  server.send(200, "text/html", htmlPage());
}

void handleRight() {
  sendCommand('R');
  server.send(200, "text/html", htmlPage());
}

// ======================== Wi-Fi Handling ====================
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

// ====== ===================Setup ============================
void setup() {
#ifdef DEBUG
  Serial.begin(9600);
  delay(500);
  Serial.println("Pico W I2C Master starting...");
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

  server.on("/", handleRoot);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.begin();

  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  Wire.begin();
#ifdef DEBUG
  Serial.println("I2C ready");
#endif
}

// ============================== Loop =====================
void loop() {
  server.handleClient();
  ensureWiFi();
}
