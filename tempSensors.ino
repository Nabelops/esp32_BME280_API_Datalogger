#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_LC709203F.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define uS_TO_S_FACTOR 1000000ULL
#define PIN_I2C_POWER 7

Adafruit_BME280 bme;
Adafruit_LC709203F lc;
StaticJsonDocument<200> doc;
HTTPClient http;

unsigned long delayTime;

const char* ssid = "Example_SSID";
const char* password = "examplePassword";

void setup() {
  Serial.begin(115200);
  delay(10000);
  pinMode(PIN_I2C_POWER, INPUT);
  delay(1);
  bool polarity = digitalRead(PIN_I2C_POWER);
  Serial.println(polarity);
  if (!polarity) {
    pinMode(PIN_I2C_POWER, OUTPUT);
    digitalWrite(PIN_I2C_POWER, !polarity);
  }

  unsigned long start = millis();
  if (!bme.begin(0x77, &Wire)) {
    while (1) {
      restart(start);
      delay(10);
    };
  }

  // bme.setTemperatureCompensation(-2);
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_X1, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF   );

  if (!lc.begin()) {
    while (1) {
      restart(start);
      delay(10);
    };
  }
  lc.setThermistorB(3950);
  lc.setPackSize(LC709203F_APA_3000MAH);
  lc.setAlarmVoltage(3.8);
}

void loop() {
  if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_UNDEFINED) {
    bme.takeForcedMeasurement(); 
    mapData();
    connectToWiFi();
    postData();
    WiFi.disconnect();
  }
  digitalWrite(PIN_I2C_POWER, 0);
  esp_sleep_enable_timer_wakeup(600 * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void mapData() {
  doc["temp"] = bme.readTemperature() * 1.8 + 32;
  doc["pressure"] = bme.readPressure() / 100.0F;
  doc["humidity"] = bme.readHumidity();
  doc["battery"] = lc.cellPercent();
  doc["mac"] = WiFi.macAddress();
}

void postData() {
  http.begin("https://someAPIendpoint.cloudfunctions.net/tempSensor");  
  http.addHeader("Content-Type", "application/json"); 
  String requestBody;
  serializeJson(doc, requestBody);
  int httpResponseCode = http.POST(requestBody);
  Serial.print("sent");
}

void restart(unsigned long start) {
  if ((millis() - start) > (30 * 1000)) {
    ESP.restart();
  }
}

void connectToWiFi()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) 
  {
    restart(start);
    Serial.print(".");
    delay(500);
  }
}

















