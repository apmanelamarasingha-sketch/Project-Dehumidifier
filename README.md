# ESP32 Dehumidifier Control System

Automated dual-container dehumidifier with high-resolution data logging for efficiency analysis.

## Features
- **Dual Container Control**: Independent humidity control for 2 containers
- **Automated Operation**: Solenoid valves, fans, and Peltier cooler control
- **High-Resolution Logging**: 100ms interval dataset creation (10 samples/second)
- **Real-Time Monitoring**: Human-readable status display
- **OLED Display**: Visual feedback with animations

## Hardware
- ESP32 Development Board
- 2x DHT11 Temperature/Humidity Sensors
- 2x 12V DC Fans (exhaust & supply)
- 4x Solenoid Valves (2 per container)
- TEC1-12706 Peltier Cooler
- 2x 4-Channel Relay Modules (Active-LOW)
- 0.96" OLED Display (SSD1306)

## Quick Start

### 1. Install Dependencies
```bash
pip install pyserial pandas openpyxl
```

### 2. Upload to ESP32
Open in VS Code with PlatformIO and press `Ctrl+Alt+U`

### 3. Collect Dataset
```bash
python data_logger.py
```

Dataset saved to: `Analyze Data\dehumidifier_data.csv`

### 4. Create Dashboard with Graphs
```bash
python create_dashboard.py
```

Opens: `Analyze Data\dehumidifier_dashboard.xlsx` with automatic graphs!

## Dataset Format
```csv
Timestamp,Time(ms),Humidity1(%),Temp1(C),Humidity2(%),Temp2(C),FansRunning,PeltierOn,C1_ExhaustValve,C1_SupplyValve,C2_ExhaustValve,C2_SupplyValve
2025-11-26 10:00:00.100,10000,85.50,28.10,82.30,27.90,1,1,1,0,1,0
```

- **Resolution**: 10 samples/second (100ms intervals)
- **Format**: CSV with timestamps
- **Purpose**: Efficiency and performance analysis

## Excel Dashboard

The `create_dashboard.py` script automatically generates an Excel file with:

### Graphs:
- **Dehumidification Rate**: Humidity reduction efficiency analysis
- **System Duty Cycle**: Percentage of time system runs (bar chart + metrics)
- **Temperature Impact**: Temperature changes during dehumidification

### Metrics Calculated:
- Average dehumidification rate (% RH when fans ON)
- Duty cycle percentage
- Total running time
- Temperature drop during operation
- Max/min humidity levels

## Control Logic
- **Upper Threshold**: 90% RH (dehumidifying starts)
- **Lower Threshold**: 80% RH (dehumidifying stops)
- **Delay**: 30 seconds between exhaust and supply fan activation
- **Peltier**: ON when any container is dehumidifying

## Documentation
See `LOGGING_UPDATE.md` for detailed logging system information.