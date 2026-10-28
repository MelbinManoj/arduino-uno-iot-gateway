// Arduino Uno - reads DHT11 (temperature/humidity) and HC-SR04 (distance)
// and sends "temperature,humidity,distance" to the NodeMCU over SoftwareSerial.

#include <SoftwareSerial.h>
#include <DHT.h>

#define DHTPIN 3
#define DHTTYPE DHT11
#define TRIGPIN 4
#define ECHOPIN 5

SoftwareSerial nodeSerial(7, 8); // RX, TX
DHT dht(DHTPIN, DHTTYPE);

float readDistanceCM() {
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);

  long duration = pulseIn(ECHOPIN, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * 0.0343 / 2.0;
}

void setup() {
  Serial.begin(9600);
  nodeSerial.begin(9600);

  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float d = readDistanceCM();

  if (!isnan(t) && !isnan(h)) {
    String payload = String(t, 1) + "," + String(h, 1) + "," + String(d, 1);
    nodeSerial.println(payload);
    Serial.println(payload);
  } else {
    nodeSerial.println("nan,nan,nan");
    Serial.println("nan,nan,nan");
  }

  delay(2000);
}
