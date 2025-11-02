#include <Arduino.h>
#include "DHTesp.h"

const int DHT_PIN = 15;  // Change this to match your wiring
const int FAN = 4;

// Humidity control thresholds
const float UPPER_HUMIDITY = 75.0;  // Turn fan ON when humidity exceeds this
const float LOWER_HUMIDITY = 65.0;  // Turn fan OFF when humidity drops below this

DHTesp dht;
bool fanRunning = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize DHT sensor
  dht.setup(DHT_PIN, DHTesp::DHT11);

  pinMode(FAN, OUTPUT);
  digitalWrite(FAN, LOW);  // Start with fan OFF
  
  Serial.println("Automatic Humidity Control");
  Serial.println("-------------------------");
  Serial.print("Target range: ");
  Serial.print(LOWER_HUMIDITY);
  Serial.print(" - ");
  Serial.print(UPPER_HUMIDITY);
  Serial.println(" %");
  Serial.println();
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
    Serial.print(" %");
    
    // Control logic with hysteresis
    if (humidity > UPPER_HUMIDITY && !fanRunning) {
      // Humidity too high - turn fan ON
      digitalWrite(FAN, HIGH);
      fanRunning = true;
      Serial.print("  ->  FAN ON (reducing humidity)");
    } 
    else if (humidity < LOWER_HUMIDITY && fanRunning) {
      // Humidity back in range - turn fan OFF
      digitalWrite(FAN, LOW);
      fanRunning = false;
      Serial.print("  ->  FAN OFF (target reached)");
    }
    else {
      // Maintain current state
      Serial.print("  ->  FAN ");
      Serial.print(fanRunning ? "ON" : "OFF");
    }
    
    Serial.println();
  }
  
  delay(2000);  // Wait 2 seconds before next reading
}