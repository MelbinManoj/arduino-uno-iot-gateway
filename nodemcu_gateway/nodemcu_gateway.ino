// NodeMCU ESP8266 - receives sensor data from the Uno over SoftwareSerial
// and serves it on a Wi-Fi web page ("/") and a plain-text API ("/data").

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>

const char* ssid = "YOUR_WIFI_NAME";         // 2.4 GHz network
const char* password = "YOUR_WIFI_PASSWORD";

ESP8266WebServer server(80);
SoftwareSerial unoSerial(13, 15);   // RX, TX  => D7, D8

float temperature = NAN;
float humidity = NAN;
float distanceCm = NAN;
String lastLine = "No data yet";

String htmlPage() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<meta http-equiv='refresh' content='2'>";
  html += "<title>UNO Sensor Gateway</title>";
  html += "<style>";
  html += "body{font-family:Arial,sans-serif;background:#f4f6f8;margin:0;padding:20px;text-align:center;}";
  html += ".card{max-width:420px;margin:20px auto;background:#fff;padding:20px;border-radius:14px;box-shadow:0 3px 12px rgba(0,0,0,0.12);}";
  html += "h1{font-size:24px;margin-bottom:10px;}";
  html += "p{font-size:20px;margin:12px 0;}";
  html += ".small{font-size:14px;color:#666;}";
  html += "</style></head><body>";
  html += "<div class='card'>";
  html += "<h1>UNO Sensor Gateway</h1>";
  html += "<p>Temperature: " + String(temperature, 1) + " C</p>";
  html += "<p>Humidity: " + String(humidity, 1) + " %</p>";
  html += "<p>Distance: " + String(distanceCm, 1) + " cm</p>";
  html += "<p class='small'>Raw line: " + lastLine + "</p>";
  html += "<p class='small'>Data API: /data</p>";
  html += "</div></body></html>";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", htmlPage());
}

void handleData() {
  String out = "temperature=" + String(temperature, 1)
             + ",humidity=" + String(humidity, 1)
             + ",distance=" + String(distanceCm, 1);
  server.send(200, "text/plain", out);
}

void setup() {
  Serial.begin(9600);
  unoSerial.begin(9600);
  delay(1000);

  Serial.println();
  Serial.println("Booting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 30) {
    delay(500);
    Serial.print(".");
    tries++;
  }

  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi failed");
  }

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  if (unoSerial.available()) {
    String line = unoSerial.readStringUntil('\n');
    line.trim();

    if (line.length() > 0) {
      lastLine = line;
      Serial.println("RX: " + line);

      int first = line.indexOf(',');
      int second = line.indexOf(',', first + 1);

      if (first > 0 && second > first) {
        temperature = line.substring(0, first).toFloat();
        humidity = line.substring(first + 1, second).toFloat();
        distanceCm = line.substring(second + 1).toFloat();
      }
    }
  }
}
