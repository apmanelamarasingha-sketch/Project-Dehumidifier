#include <Arduino.h>
#include "DHTesp.h"

const int DHT_PIN = 15;     // DHT sensor pin
const int FAN_1 = 4;        // Supply fan (pushes dry air back)
const int FAN_2 = 5;        // Exhaust fan (pulls moist air out)

// Humidity control thresholds
const float UPPER_HUMIDITY = 75.0;  // Turn fans ON when humidity exceeds this
const float LOWER_HUMIDITY = 65.0;  // Turn fans OFF when humidity drops below this

// Dehumidification delay: time between exhaust fan ON and supply fan ON
const unsigned long DEHUMIDIFY_DELAY_MS = 30000;  // 30 seconds for dehumidifying process

DHTesp dht;
bool fansRunning = false;
bool supplyFanOn = false;
unsigned long exhaustStartTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize DHT sensor
  dht.setup(DHT_PIN, DHTesp::DHT11);

  // Initialize fan pins
  pinMode(FAN_1, OUTPUT);
  pinMode(FAN_2, OUTPUT);
  digitalWrite(FAN_1, LOW);  // Supply fan OFF
  digitalWrite(FAN_2, LOW);  // Exhaust fan OFF
  
  Serial.println("Dual Fan Humidity Control");
  Serial.println("-------------------------");
  Serial.println("FAN 1 (D4): Supply Fan");
  Serial.println("FAN 2 (D5): Exhaust Fan");
  Serial.print("Target range: ");
  Serial.print(LOWER_HUMIDITY);
  Serial.print(" - ");
  Serial.print(UPPER_HUMIDITY);
  Serial.println(" %");
  Serial.println();
}

void loop() {
  // Read humidity
  delay(dht.getMinimumSamplingPeriod());
  float humidity = dht.getHumidity();
  
  if (isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  Serial.print("Relative Humidity: ");
  Serial.print(humidity);
  Serial.print(" %");

  // Control logic with hysteresis and delayed supply fan
  if (humidity > UPPER_HUMIDITY && !fansRunning) {
    // Humidity too high - start dehumidifying
    fansRunning = true;
    supplyFanOn = false;
    exhaustStartTime = millis();
    digitalWrite(FAN_2, HIGH);  // Turn ON exhaust fan immediately
    digitalWrite(FAN_1, LOW);   // Supply fan stays OFF initially
    Serial.print("  ->  EXHAUST FAN ON (dehumidifying)");
  } 
  else if (humidity < LOWER_HUMIDITY && fansRunning) {
    // Humidity reached target - turn OFF both fans
    digitalWrite(FAN_2, LOW);   // Turn OFF exhaust fan
    digitalWrite(FAN_1, LOW);   // Turn OFF supply fan
    fansRunning = false;
    supplyFanOn = false;
    Serial.print("  ->  BOTH FANS OFF (target reached)");
  }
  else if (fansRunning) {
    // Fans are running - check if it's time to turn on supply fan
    unsigned long elapsed = millis() - exhaustStartTime;
    
    if (!supplyFanOn && elapsed >= DEHUMIDIFY_DELAY_MS) {
      // 30 seconds passed, turn ON supply fan
      digitalWrite(FAN_1, HIGH);
      supplyFanOn = true;
      Serial.print("  ->  BOTH FANS ON (exhaust + supply)");
    }
    else if (!supplyFanOn) {
      // Still waiting for delay
      Serial.print("  ->  EXHAUST FAN ON (supply in ");
      Serial.print((DEHUMIDIFY_DELAY_MS - elapsed) / 1000);
      Serial.print("s)");
    }
    else {
      // Both fans running
      Serial.print("  ->  BOTH FANS ON (reducing humidity)");
    }
  }
  else {
    // Fans OFF, humidity in acceptable range
    Serial.print("  ->  BOTH FANS OFF (humidity in range)");
  }
  
  Serial.println();
  delay(2000);  // Wait before next reading
}