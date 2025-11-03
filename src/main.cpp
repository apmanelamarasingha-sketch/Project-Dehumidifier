#include <Arduino.h>
#include "DHTesp.h"

const int DHT1_PIN = 15;    // Container 1 sensor
const int DHT2_PIN = 18;    // Container 2 sensor
const int FAN_1 = 4;        // Supply fan (pushes dry air back)
const int FAN_2 = 5;        // Exhaust fan (pulls moist air out)

// Relay pins for solenoid valves
const int RELAY_EXHAUST_1 = 13;  // Container 1 exhaust valve (normally closed)

// Humidity control thresholds
const float UPPER_HUMIDITY = 85.0;  // Turn fans ON when humidity exceeds this
const float LOWER_HUMIDITY = 75.0;  // Turn fans OFF when humidity drops below this

// Dehumidification delay: time between exhaust fan ON and supply fan ON
const unsigned long DEHUMIDIFY_DELAY_MS = 30000;  // 30 seconds for dehumidifying process

DHTesp dht1;  // Container 1 sensor
DHTesp dht2;  // Container 2 sensor
bool fansRunning = false;
bool supplyFanOn = false;
unsigned long exhaustStartTime = 0;

// Container-specific valve states
bool container1Valve = false;  // Container 1 exhaust valve state

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize DHT sensors
  dht1.setup(DHT1_PIN, DHTesp::DHT11);
  dht2.setup(DHT2_PIN, DHTesp::DHT11);

  // Initialize fan pins
  pinMode(FAN_1, OUTPUT);
  pinMode(FAN_2, OUTPUT);
  digitalWrite(FAN_1, LOW);  // Supply fan OFF
  digitalWrite(FAN_2, LOW);  // Exhaust fan OFF
  
  // Initialize relay pins for solenoid valves
  pinMode(RELAY_EXHAUST_1, OUTPUT);
  digitalWrite(RELAY_EXHAUST_1, LOW);  // Container 1 exhaust valve CLOSED
  
  Serial.println("Dual Container Humidity Control");
  Serial.println("--------------------------------");
  Serial.println("Container 1: DHT sensor on D15");
  Serial.println("Container 2: DHT sensor on D18");
  Serial.println("FAN 1 (D4): Supply Fan");
  Serial.println("FAN 2 (D5): Exhaust Fan");
  Serial.println("Relay D12: Container 1 Exhaust Valve");
  Serial.print("Target range: ");
  Serial.print(LOWER_HUMIDITY);
  Serial.print(" - ");
  Serial.print(UPPER_HUMIDITY);
  Serial.println(" %");
  Serial.println();
}

void loop() {
  // Read humidity from both sensors
  delay(dht1.getMinimumSamplingPeriod());
  float humidity1 = dht1.getHumidity();
  delay(dht2.getMinimumSamplingPeriod());
  float humidity2 = dht2.getHumidity();
  
  // Display readings
  Serial.print("Container 1: ");
  if (isnan(humidity1)) {
    Serial.print("ERROR");
  } else {
    Serial.print(humidity1);
    Serial.print("%");
  }
  
  Serial.print("  |  Container 2: ");
  if (isnan(humidity2)) {
    Serial.print("ERROR");
  } else {
    Serial.print(humidity2);
    Serial.print("%");
  }

  // Determine if any container needs dehumidifying
  // Use the higher humidity value if both are valid
  float maxHumidity = 0;
  bool anyValid = false;
  
  if (!isnan(humidity1)) {
    maxHumidity = humidity1;
    anyValid = true;
  }
  if (!isnan(humidity2)) {
    if (!anyValid || humidity2 > maxHumidity) {
      maxHumidity = humidity2;
    }
    anyValid = true;
  }
  
  if (!anyValid) {
    Serial.println("  ->  ERROR: Both sensors failed!");
    delay(2000);
    return;
  }

  // Control Container 1 exhaust valve based on Container 1 humidity only
  if (!isnan(humidity1)) {
    if (humidity1 > UPPER_HUMIDITY && !container1Valve) {
      // Container 1 humidity too high - OPEN exhaust valve
      digitalWrite(RELAY_EXHAUST_1, HIGH);
      container1Valve = true;
      Serial.print("  [C1 Valve: OPEN]");
    }
    else if (humidity1 < LOWER_HUMIDITY && container1Valve) {
      // Container 1 humidity OK - CLOSE exhaust valve
      digitalWrite(RELAY_EXHAUST_1, LOW);
      container1Valve = false;
      Serial.print("  [C1 Valve: CLOSED]");
    }
    else {
      // Maintain current state
      Serial.print(container1Valve ? "  [C1 Valve: OPEN]" : "  [C1 Valve: CLOSED]");
    }
  }

  // Control logic with hysteresis and delayed supply fan
  // Use the highest humidity reading to decide fan control
  if (maxHumidity > UPPER_HUMIDITY && !fansRunning) {
    // Humidity too high in at least one container - start dehumidifying
    fansRunning = true;
    supplyFanOn = false;
    exhaustStartTime = millis();
    digitalWrite(FAN_2, HIGH);  // Turn ON exhaust fan immediately
    digitalWrite(FAN_1, LOW);   // Supply fan stays OFF initially
    Serial.print("  ->  EXHAUST FAN ON (dehumidifying)");
  } 
  else if (maxHumidity < LOWER_HUMIDITY && fansRunning) {
    // Humidity reached target in both containers - turn OFF both fans
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