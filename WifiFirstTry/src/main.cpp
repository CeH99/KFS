#include <Arduino.h>
#include <WebServer.h>

const char* ssid = "molniya1";       // имя Wi-Fi
const char* password = "140578875041"; // пароль Wi-Fi

WebServer server(80);

bool ledState = false;
const int ledPin = 5; // встроенный светодиод Pico W

// Генерация HTML
String htmlPage() {
  String state = ledState ? "ON" : "OFF";
  String color = ledState ? "red" : "green";

  String page = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  page += "<title>Pico W LED</title></head><body style='text-align:center;'>";
  page += "<h1>Raspberry Pi Pico W</h1>";
  page += "<h2>LED is " + state + "</h2>";
  page += "<form action='/toggle'><button style='padding:20px;font-size:24px;background:" 
          + color 
          + ";color:white;border:none;border-radius:12px;'>Toggle LED</button></form>";
  page += "</body></html>";
  return page;
}

// Главная
void handleRoot() {
  server.send(200, "text/html", htmlPage());
}

// Переключение
void handleToggle() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState ? HIGH : LOW);
  server.send(200, "text/html", htmlPage());
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("🚀 Запуск...");

  WiFi.begin(ssid, password);

  Serial.print("⏳ Подключение к Wi-Fi: ");
  Serial.println(ssid);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) { // ждём макс 10 секунд
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Wi-Fi подключен!");
    Serial.print("📡 IP адрес: ");
    Serial.println(WiFi.localIP());

    // 👉 Регистрируем обработчики
    server.on("/", handleRoot);
    server.on("/toggle", handleToggle);

    // 👉 Запускаем веб-сервер
    server.begin();
    Serial.println("🌍 Веб-сервер запущен!");
  } else {
    Serial.println("\n❌ Не удалось подключиться к Wi-Fi!");
  }

  pinMode(ledPin, OUTPUT); // не забудь инициализировать пин светодиода
  digitalWrite(ledPin, LOW);
}



void loop() {
  server.handleClient();
}
