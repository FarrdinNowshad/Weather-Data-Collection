#define BLYNK_TEMPLATE_ID "TMPL6VhrmwPah"
#define BLYNK_TEMPLATE_NAME "Home Weather Station"
#define BLYNK_AUTH_TOKEN "z8ryDvUEEIteFCTiUNFDk8n3j3-NYsw_"

#define BLYNK_PRINT Serial

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Your_SSID";  // Replace with your WiFi SSID
char pass[] = "Your_Password";  // Replace with your WiFi password

DHT dht(5, DHT11);  // DHT sensor on pin 5
Adafruit_BMP280 bmp;

int sensorValue = 0;

BlynkTimer timer;

void setup() {
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(100L, sendSensor);

  dht.begin();

  pinMode(A0, INPUT);  // MQ2 sensor on A0

  while (!Serial)
    delay(100);

  Serial.println(F("BMP280 test"));
  Wire.begin();
  unsigned status = bmp.begin(0x76);  // BMP280 I2C address

  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or try a different address!"));
    Serial.print("SensorID was: 0x");
    Serial.println(bmp.sensorID(), 16);
    Serial.print("ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("ID of 0x60 represents a BME 280.\n");
    Serial.print("ID of 0x61 represents a BME 680.\n");
    while (1)
      delay(10);
  }

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
}

void sendSensor() {
  sensorValue = analogRead(A0);  // MQ2 sensor

  float temp = bmp.readTemperature();
  float pressure = bmp.readPressure();
  float altitude = bmp.readAltitude(1017);
  float humidity = dht.readHumidity();

  if (isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print(F("Temperature = "));
  Serial.print(temp);
  Serial.println(" *C");

  Serial.print(F("Humidity = "));
  Serial.print(humidity);
  Serial.println("%");

  Serial.print(F("Pressure = "));
  Serial.print(pressure);
  Serial.println(" Pa");

  Serial.print(F("Approx altitude = "));
  Serial.print(altitude);
  Serial.println(" m");

  Serial.print(F("Air Quality = "));
  Serial.print(sensorValue);

  Serial.println();

  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V3, humidity);
  Blynk.virtualWrite(V1, pressure);
  Blynk.virtualWrite(V2, altitude);
  Blynk.virtualWrite(V4, sensorValue);
  Blynk.virtualWrite(V5, "Data sent successfully\n");
}

void loop() {
  Blynk.run();
  timer.run();
}
