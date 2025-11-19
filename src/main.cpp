#include <Arduino.h>
#include "DHTesp.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int DHT1_PIN = 15;    // Container 1 sensor
const int DHT2_PIN = 18;    // Container 2 sensor
const int FAN_1 = 4;        // Supply fan (pushes dry air back)
const int FAN_2 = 5;        // Exhaust fan (pulls moist air out)

// Relay pins for solenoid valves
const int RELAY_EXHAUST_1 = 13;  // Container 1 exhaust valve (normally closed)
const int RELAY_SUPPLY_1 = 12;   // Container 1 supply valve (normally closed)
const int RELAY_EXHAUST_2 = 27;  // Container 2 exhaust valve (normally closed)
const int RELAY_SUPPLY_2 = 26;   // Container 2 supply valve (normally closed)
const int RELAY_PELTIER = 25;    // Peltier thermoelectric cooler

// Humidity control thresholds
const float UPPER_HUMIDITY = 90.0;  // Turn fans ON when humidity exceeds this
const float LOWER_HUMIDITY = 80.0;  // Turn fans OFF when humidity drops below this

// Dehumidification delay: time between exhaust fan ON and supply fan ON
const unsigned long DEHUMIDIFY_DELAY_MS = 30000;  // 30 seconds for dehumidifying process

DHTesp dht1;  // Container 1 sensor
DHTesp dht2;  // Container 2 sensor
bool fansRunning = false;
bool supplyFanOn = false;
unsigned long exhaustStartTime = 0;

// Container-specific valve states
bool container1ExhaustValve = false;  // Container 1 exhaust valve state
bool container1SupplyValve = false;   // Container 1 supply valve state
unsigned long container1ExhaustStart = 0;  // Track when exhaust started for Container 1

bool container2ExhaustValve = false;  // Container 2 exhaust valve state
bool container2SupplyValve = false;   // Container 2 supply valve state
unsigned long container2ExhaustStart = 0;  // Track when exhaust started for Container 2

void updateDisplay(float h1, float h2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Title
  display.setCursor(0, 0);
  display.println("Current Status");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  
  // Container 1
  display.setCursor(0, 16);
  display.setTextSize(1);
  display.println("Container 1:");
  display.setCursor(0, 26);
  display.setTextSize(1);
  if (isnan(h1)) {
    display.println("RH: ERROR");
  } else {
    display.print("RH: ");
    display.print(h1, 1);
    display.println(" %");
  }
  
  // Container 2
  display.setCursor(0, 42);
  display.setTextSize(1);
  display.println("Container 2:");
  display.setCursor(0, 52);
  display.setTextSize(1);
  if (isnan(h2)) {
    display.println("RH: ERROR");
  } else {
    display.print("RH: ");
    display.print(h2, 1);
    display.println(" %");
  }
  
  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  // Startup animation
  display.clearDisplay();
  
  // Draw animated loading bar
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 10);
  display.println("DEHUMID");
  display.setCursor(29, 30);
  display.println("SYSTEM");
  display.display();
  delay(1000);
  
  // Animated progress bar
  for(int i = 0; i < 128; i += 4) {
    display.fillRect(0, 55, i, 8, SSD1306_WHITE);
    display.display();
    delay(20);
  }
  delay(500);
  
  // Welcome screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(25, 20);
  display.println("Initializing");
  display.setCursor(35, 35);
  display.println("Sensors...");
  display.display();
  delay(1500);
  
  // Initialize DHT sensors
  dht1.setup(DHT1_PIN, DHTesp::DHT11);
  dht2.setup(DHT2_PIN, DHTesp::DHT11);

  // Initialize fan pins (Active LOW relays: HIGH = OFF, LOW = ON)
  pinMode(FAN_1, OUTPUT);
  pinMode(FAN_2, OUTPUT);
  digitalWrite(FAN_1, HIGH);  // Supply fan OFF
  digitalWrite(FAN_2, HIGH);  // Exhaust fan OFF
  
  // Initialize relay pins for solenoid valves and Peltier
  pinMode(RELAY_EXHAUST_1, OUTPUT);
  pinMode(RELAY_SUPPLY_1, OUTPUT);
  pinMode(RELAY_EXHAUST_2, OUTPUT);
  pinMode(RELAY_SUPPLY_2, OUTPUT);
  pinMode(RELAY_PELTIER, OUTPUT);
  digitalWrite(RELAY_EXHAUST_1, LOW);  // Container 1 exhaust valve CLOSED
  digitalWrite(RELAY_SUPPLY_1, LOW);   // Container 1 supply valve CLOSED
  digitalWrite(RELAY_EXHAUST_2, LOW);  // Container 2 exhaust valve CLOSED
  digitalWrite(RELAY_SUPPLY_2, LOW);   // Container 2 supply valve CLOSED
  digitalWrite(RELAY_PELTIER, LOW);    // Peltier OFF
  
  Serial.println("Dual Container Humidity Control");
  Serial.println("--------------------------------");
  Serial.println("Container 1: DHT sensor on D15");
  Serial.println("Container 2: DHT sensor on D18");
  Serial.println("FAN 1 (D4): Supply Fan");
  Serial.println("FAN 2 (D5): Exhaust Fan");
  Serial.println("Relay D13: Container 1 Exhaust Valve");
  Serial.println("Relay D12: Container 1 Supply Valve");
  Serial.println("Relay D27: Container 2 Exhaust Valve");
  Serial.println("Relay D26: Container 2 Supply Valve");
  Serial.println("Relay D25: Peltier Cooler");
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
  
  // Update OLED display
  updateDisplay(humidity1, humidity2);
  
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

  // Control Container 1 valves based on Container 1 humidity only
  if (!isnan(humidity1)) {
    if (humidity1 > UPPER_HUMIDITY && !container1ExhaustValve) {
      // Container 1 humidity too high - START dehumidifying
      // OPEN exhaust valve, CLOSE supply valve
      digitalWrite(RELAY_EXHAUST_1, HIGH);
      digitalWrite(RELAY_SUPPLY_1, LOW);
      container1ExhaustValve = true;
      container1SupplyValve = false;
      container1ExhaustStart = millis();
      Serial.print("  [C1: Exhaust OPEN, Supply CLOSED]");
    }
    else if (humidity1 < LOWER_HUMIDITY && container1ExhaustValve) {
      // Container 1 humidity OK - STOP dehumidifying
      // CLOSE both valves
      digitalWrite(RELAY_EXHAUST_1, LOW);
      digitalWrite(RELAY_SUPPLY_1, LOW);
      container1ExhaustValve = false;
      container1SupplyValve = false;
      Serial.print("  [C1: Both valves CLOSED]");
    }
    else if (container1ExhaustValve) {
      // Currently dehumidifying - check if it's time to open supply valve
      unsigned long elapsed = millis() - container1ExhaustStart;
      
      if (!container1SupplyValve && elapsed >= DEHUMIDIFY_DELAY_MS) {
        // 30 seconds passed - OPEN supply valve (keep exhaust open too)
        digitalWrite(RELAY_SUPPLY_1, HIGH);
        container1SupplyValve = true;
        Serial.print("  [C1: Both valves OPEN]");
      }
      else if (!container1SupplyValve) {
        // Still waiting for delay
        Serial.print("  [C1: Exhaust OPEN, Supply opening in ");
        Serial.print((DEHUMIDIFY_DELAY_MS - elapsed) / 1000);
        Serial.print("s]");
      }
      else {
        // Both valves open
        Serial.print("  [C1: Both valves OPEN]");
      }
    }
    else {
      // Humidity in range, valves closed
      Serial.print("  [C1: Both valves CLOSED]");
    }
  }

  // Control Container 2 valves based on Container 2 humidity only
  if (!isnan(humidity2)) {
    if (humidity2 > UPPER_HUMIDITY && !container2ExhaustValve) {
      // Container 2 humidity too high - START dehumidifying
      // OPEN exhaust valve, CLOSE supply valve
      digitalWrite(RELAY_EXHAUST_2, HIGH);
      digitalWrite(RELAY_SUPPLY_2, LOW);
      container2ExhaustValve = true;
      container2SupplyValve = false;
      container2ExhaustStart = millis();
      Serial.print("  [C2: Exhaust OPEN, Supply CLOSED]");
    }
    else if (humidity2 < LOWER_HUMIDITY && container2ExhaustValve) {
      // Container 2 humidity OK - STOP dehumidifying
      // CLOSE both valves
      digitalWrite(RELAY_EXHAUST_2, LOW);
      digitalWrite(RELAY_SUPPLY_2, LOW);
      container2ExhaustValve = false;
      container2SupplyValve = false;
      Serial.print("  [C2: Both valves CLOSED]");
    }
    else if (container2ExhaustValve) {
      // Currently dehumidifying - check if it's time to open supply valve
      unsigned long elapsed = millis() - container2ExhaustStart;
      
      if (!container2SupplyValve && elapsed >= DEHUMIDIFY_DELAY_MS) {
        // 30 seconds passed - OPEN supply valve (keep exhaust open too)
        digitalWrite(RELAY_SUPPLY_2, HIGH);
        container2SupplyValve = true;
        Serial.print("  [C2: Both valves OPEN]");
      }
      else if (!container2SupplyValve) {
        // Still waiting for delay
        Serial.print("  [C2: Exhaust OPEN, Supply opening in ");
        Serial.print((DEHUMIDIFY_DELAY_MS - elapsed) / 1000);
        Serial.print("s]");
      }
      else {
        // Both valves open
        Serial.print("  [C2: Both valves OPEN]");
      }
    }
    else {
      // Humidity in range, valves closed
      Serial.print("  [C2: Both valves CLOSED]");
    }
  }

  // Control Peltier cooler - ON when any container is dehumidifying
  bool anyDehumidifying = container1ExhaustValve || container2ExhaustValve;
  if (anyDehumidifying) {
    digitalWrite(RELAY_PELTIER, HIGH);  // Peltier ON
    Serial.print("  [Peltier: ON]");
  } else {
    digitalWrite(RELAY_PELTIER, LOW);   // Peltier OFF
    Serial.print("  [Peltier: OFF]");
  }

  // Control logic with hysteresis and delayed supply fan
  // Use the highest humidity reading to decide fan control
  if (maxHumidity > UPPER_HUMIDITY && !fansRunning) {
    // Humidity too high in at least one container - start dehumidifying
    fansRunning = true;
    supplyFanOn = false;
    exhaustStartTime = millis();
    digitalWrite(FAN_2, LOW);   // Turn ON exhaust fan immediately (Active LOW)
    digitalWrite(FAN_1, HIGH);  // Supply fan stays OFF initially (Active LOW)
    Serial.print("  ->  EXHAUST FAN ON (dehumidifying)");
  } 
  else if (maxHumidity < LOWER_HUMIDITY && fansRunning) {
    // Humidity reached target in both containers - turn OFF both fans
    digitalWrite(FAN_2, HIGH);  // Turn OFF exhaust fan (Active LOW)
    digitalWrite(FAN_1, HIGH);  // Turn OFF supply fan (Active LOW)
    fansRunning = false;
    supplyFanOn = false;
    Serial.print("  ->  BOTH FANS OFF (target reached)");
  }
  else if (fansRunning) {
    // Fans are running - check if it's time to turn on supply fan
    unsigned long elapsed = millis() - exhaustStartTime;
    
    if (!supplyFanOn && elapsed >= DEHUMIDIFY_DELAY_MS) {
      // 30 seconds passed, turn ON supply fan
      digitalWrite(FAN_1, LOW);  // Turn ON supply fan (Active LOW)
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