# Data Logging System - Dataset Creation

## ✅ System Overview

### ESP32 Firmware (main.cpp)
**Dual Output System:**

1. **High-Resolution CSV Data** (every 100ms)
   - 10 samples per second
   - Format: `Time,H1,T1,H2,T2,Fans,Peltier,C1_Exhaust,C1_Supply,C2_Exhaust,C2_Supply`
   - Streamed continuously via Serial for dataset creation

2. **Human-Readable Monitoring** (every 2 seconds)
   - Clear status display on Serial Monitor
   - Shows: Humidity, Temperature, Fan status, Peltier status, Valve states
   - Easy to monitor system operation

### Python Logger (data_logger.py)
**Smart Data Capture:**
- Automatically separates CSV data from monitoring messages
- Saves only CSV data to file for dataset
- Displays monitoring messages on console
- Shows progress every 50 records (5 seconds)

## 📊 Data Resolution

**100ms logging = 10 samples/second**
- 600 samples/minute
- 36,000 samples/hour
- 864,000 samples/24 hours

**File Size Estimate:**
- ~80 bytes per line
- ~28 KB per hour
- ~672 KB per 24 hours

## 🚀 How to Use

### 1. Install Python Dependencies
```powershell
pip install pyserial pandas openpyxl
```

### 2. Upload Code to ESP32
```powershell
# In VS Code with PlatformIO
Ctrl+Alt+U
```

### 3. Start Dataset Collection
```powershell
python data_logger.py
```

### 4. Create Excel Dashboard with Graphs
```powershell
python create_dashboard.py
```

This creates `dehumidifier_dashboard.xlsx` with automatic graphs!

### What You'll See:
**In Console (Monitoring):**
```
========================================
Container 1: 85.5% RH, 28.1°C  |  Container 2: 82.3% RH, 27.9°C
Fans: ON  |  Peltier: ON
C1 Valves: Exhaust=OPEN, Supply=CLOSED
C2 Valves: Exhaust=OPEN, Supply=CLOSED

✓ Logged 50 records... (latest: 2025-11-26 10:00:05.123)
```

**In CSV File (Dataset - High-Resolution):**
```csv
Timestamp,Time(ms),Humidity1(%),Temp1(C),Humidity2(%),Temp2(C),FansRunning,PeltierOn,C1_ExhaustValve,C1_SupplyValve,C2_ExhaustValve,C2_SupplyValve
2025-11-26 10:00:00.100,10000,85.50,28.10,82.30,27.90,1,1,1,0,1,0
2025-11-26 10:00:00.200,10100,85.48,28.10,82.28,27.90,1,1,1,0,1,0
2025-11-26 10:00:00.300,10200,85.46,28.11,82.26,27.90,1,1,1,0,1,0
```

**Dataset Location:**
`Analyze Data\dehumidifier_data.csv`

## ⚠️ Important Notes

### System Requirements
- ✅ Python with pyserial library installed
- ✅ ESP32 connected via USB during data collection
- ✅ Python logger must run continuously to capture data

### Serial Buffer Considerations
**If you experience data loss:**
- Increase LOG_INTERVAL in main.cpp from 100ms to 200ms or 500ms
- Close any other programs using the COM port

### PC Connection Required
- ESP32 has no built-in storage
- Dataset streams over USB to PC in real-time
- Keep Python script running during entire data collection session

## 🔧 Adjusting Log Resolution

Edit `main.cpp` line 49:
```cpp
const unsigned long LOG_INTERVAL = 100;  // Change this value (in ms)
```

**Recommended values:**
- `50` = 20 samples/sec (very high resolution, may cause serial overflow)
- `100` = 10 samples/sec (current, good balance)
- `200` = 5 samples/sec (safe, still captures details)
- `500` = 2 samples/sec (low resolution)
- `1000` = 1 sample/sec (minimal)

## 📊 Automatic Excel Dashboard

After collecting data, run:
```powershell
python create_dashboard.py
```

This automatically creates `dehumidifier_dashboard.xlsx` with:

### Included Graphs:
1. **Humidity Trends** - Both containers over time
2. **Temperature Impact** - Temperature changes during operation
3. **System Activity** - Fans and Peltier ON/OFF status
4. **Dehumidification Rate** - Calculated efficiency metrics
5. **Duty Cycle Analysis** - System running time percentage

### Sheet Structure:
- **Raw Data**: All imported CSV data
- **Calculations**: Automatic metrics (duty cycle, temp drop, rates)
- **Humidity Trends**: Line chart showing humidity changes
- **Temperature Impact**: Temperature graphs with summary
- **System Activity**: Fans/Peltier activity with duty cycle summary

Simply open `dehumidifier_dashboard.xlsx` and all graphs are ready!

## 📊 Manual Analysis (Alternative)

You can also analyze the CSV manually:

### Python (pandas)
```python
import pandas as pd
import matplotlib.pyplot as plt

# Load dataset
df = pd.read_csv('Analyze Data/dehumidifier_data.csv')

# Plot humidity over time
plt.plot(df['Time(ms)'], df['Humidity1(%)'])
plt.xlabel('Time (ms)')
plt.ylabel('Humidity (%)')
plt.show()
```

### MATLAB
```matlab
data = readtable('Analyze Data/dehumidifier_data.csv');
plot(data.Time_ms_, data.Humidity1__)
```

## ✅ System Benefits

**Advantages:**
- ✅ No additional hardware needed
- ✅ High-resolution data (100ms intervals)
- ✅ Real-time monitoring during collection
- ✅ Human-readable status messages
- ✅ Dataset ready for analysis

**Limitations:**
- ⚠️ PC must stay connected during data collection
- ⚠️ Keep Python logger running continuously

---

**Configuration:** 100ms high-resolution dataset logging + 2-second human-readable monitoring
