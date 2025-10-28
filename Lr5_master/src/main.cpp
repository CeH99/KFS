#include <WiFi.h>
#include <Wire.h>

const char* ssid = "PicoControl";
const char* password = "12345678";

WiFiServer server(80);
const int slaveAddr = 0x08;

void setup() {
  Serial.begin(115200);
  Serial.begin(9600);
  Wire.begin(); // I2C Master

  Serial.println("Starting Access Point...");
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("❌ Failed to start AP!");
    return;
  }

  delay(2000); // дать чипу инициализироваться

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Access Point Started!\nIP address: ");
  Serial.println(myIP);

  server.begin();
  Serial.println("Server started!");
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String req = client.readStringUntil('\r');
  client.flush();

  if (req.indexOf("/right") != -1) {
    Wire.beginTransmission(slaveAddr);
    Wire.write('R');
    Wire.endTransmission();
    Serial.println("Sent: R");
  }
  else if (req.indexOf("/left") != -1) {
    Wire.beginTransmission(slaveAddr);
    Wire.write('L');
    Wire.endTransmission();
    Serial.println("Sent: L");
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html><head><title>Servo Control</title></head><body>");
  client.println("<h1>Servo Controller</h1>");
  client.println("<button onclick=\"location.href='/left'\">⬅️ Left</button>");
  client.println("<button onclick=\"location.href='/right'\">➡️ Right</button>");
  client.println("</body></html>");
}
