# ESP32 Dehumidifier Data Logger

## Overview
This system logs data from the ESP32 dehumidifier controller to CSV files every 100ms for analysis.

## Data Collected
The following data is captured every 100ms:
- **Container 1**: Humidity (%), Temperature (°C)
- **Container 2**: Humidity (%), Temperature (°C)
- **Exhaust Fan Status**: 0=OFF, 1=ON
- **Supply Fan Status**: 0=OFF, 1=ON
- **Container 1 Exhaust Valve**: 0=CLOSED, 1=OPEN
- **Container 1 Supply Valve**: 0=CLOSED, 1=OPEN
- **Container 2 Exhaust Valve**: 0=CLOSED, 1=OPEN
- **Container 2 Supply Valve**: 0=CLOSED, 1=OPEN
- **Peltier Cooler Status**: 0=OFF, 1=ON

## Quick Start

### 1. Upload ESP32 Code
First, upload the modified `main.cpp` to your ESP32 using PlatformIO:
```
platformio run --target upload
```

### 2. Install Python Requirements
Make sure you have Python installed, then install required packages:
```
pip install pyserial
```

### 3. Start Data Logging
Simply double-click `start_logger.bat` or run:
```
python data_logger.py
```

The script will:
- Automatically detect your ESP32 COM port (or ask you to select one)
- Create a CSV file in the `Analyze Data` folder
- Start logging data in real-time
- Show progress updates every 5 seconds

### 4. Stop Logging
Press `Ctrl+C` to stop logging. Your data will be saved automatically.

## Output File
Data is saved to:
```
C:\Users\User\OneDrive - itum.mrt.ac.lk\Desktop\Lecture Modules\Semester-4\Project 2\Project-Dehumidifier\Analyze Data\dehumidifier_data.csv
```

## CSV File Format
The CSV file contains the following columns:
```
Timestamp, Time_ms, C1_Humidity_%, C1_Temperature_C, C2_Humidity_%, C2_Temperature_C, 
ExhaustFan_Status, SupplyFan_Status, C1_ExhaustValve, C1_SupplyValve, 
C2_ExhaustValve, C2_SupplyValve, Peltier_Status
```

### Example Data:
```
Timestamp,Time_ms,C1_Humidity_%,C1_Temperature_C,C2_Humidity_%,C2_Temperature_C,ExhaustFan_Status,SupplyFan_Status,C1_ExhaustValve,C1_SupplyValve,C2_ExhaustValve,C2_SupplyValve,Peltier_Status
2025-11-27 10:30:45.123,12345,85.50,28.30,82.10,27.50,1,0,1,0,1,0,1
2025-11-27 10:30:45.223,12445,85.45,28.30,82.05,27.50,1,0,1,0,1,0,1
```

## Analyzing Data in Excel
1. Open the CSV file in Excel
2. The data is already formatted with headers
3. You can create charts and graphs to analyze:
   - Humidity trends over time
   - Temperature variations
   - Fan operation cycles
   - Valve open/close patterns
   - Dehumidifying efficiency

## Troubleshooting

### "Could not open serial port"
- Make sure ESP32 is connected via USB
- Close PlatformIO Serial Monitor or any other program using the COM port
- Check Device Manager to confirm the COM port

### "Python is not installed"
- Download Python from https://www.python.org/
- During installation, check "Add Python to PATH"

### "No data appearing in CSV"
- Make sure you uploaded the latest `main.cpp` to ESP32
- Check Serial Monitor to verify ESP32 is sending "DATA," lines
- Verify baud rate is 115200

### Data appears as -1
- Value of -1 indicates sensor error or NaN (Not a Number)
- Check DHT11 sensor connections
- Verify sensors are working properly

## Notes
- Data is logged at approximately 100ms intervals (10 times per second)
- Each hour of logging generates approximately 36,000 data points
- Large CSV files (>1 million rows) may require Excel 2016+ or specialized tools
- The script automatically flushes data to disk to prevent data loss
