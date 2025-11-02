#include <Arduino.h>
#include "DHTesp.h"

const int DHT_PIN = 15;  // Change this to match your wiring
const int FAN = 4;
bool reucing;
DHTesp dht;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize DHT sensor
  dht.setup(DHT_PIN, DHTesp::DHT11);

  pinMode(FAN, OUTPUT);
  
  Serial.println("DHT11 Humidity Sensor Test");
  Serial.println("-------------------------");
}

void loop() {
  // Wait a few seconds between measurements
  delay(dht.getMinimumSamplingPeriod());

  // Reading temperature and humidity
  float humidity = dht.getHumidity();
  
  if (isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Relative Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }
  
  delay(2000);  // Wait 2 seconds before next reading
}