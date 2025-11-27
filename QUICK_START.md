# Quick Start Guide - Data Logging System

## What's New?
Your ESP32 now outputs structured data every 100ms that can be logged to CSV files for analysis in Excel.

## Files Created:
1. **data_logger.py** - Main Python script to capture data
2. **start_logger.bat** - Double-click to start logging (easiest method)
3. **check_setup.py** - Verify your system is ready
4. **find_com_port.py** - Find which COM port your ESP32 is on
5. **DATA_LOGGER_README.md** - Detailed documentation

## Step-by-Step Instructions:

### Step 1: Upload New Code to ESP32
1. Open PlatformIO in VS Code
2. Click the Upload button (or press `Ctrl+Alt+U`)
3. Wait for "SUCCESS" message
4. Your ESP32 now sends CSV data every 100ms

### Step 2: Verify Setup (Optional)
```
python check_setup.py
```
This checks if Python and libraries are installed.

### Step 3: Start Logging
**Method A - Easy (Double-click):**
- Simply double-click `start_logger.bat`

**Method B - Command line:**
```
python data_logger.py
```

The script will:
- Auto-detect your ESP32 COM port
- Create `dehumidifier_data.csv` in the `Analyze Data` folder
- Start logging immediately
- Show progress every 5 seconds

### Step 4: Stop Logging
- Press `Ctrl+C` in the console window
- Data is saved automatically

### Step 5: Analyze in Excel
1. Navigate to: `Analyze Data\dehumidifier_data.csv`
2. Open in Excel
3. Create charts and analyze trends

## What Data is Logged?

Every 100ms, the following is recorded:
- Timestamp (date and time)
- Container 1: Humidity % and Temperature °C
- Container 2: Humidity % and Temperature °C  
- Exhaust Fan: ON (1) or OFF (0)
- Supply Fan: ON (1) or OFF (0)
- Container 1 Exhaust Valve: OPEN (1) or CLOSED (0)
- Container 1 Supply Valve: OPEN (1) or CLOSED (0)
- Container 2 Exhaust Valve: OPEN (1) or CLOSED (0)
- Container 2 Supply Valve: OPEN (1) or CLOSED (0)
- Peltier Cooler: ON (1) or OFF (0)

## Troubleshooting:

**"Could not open serial port"**
- Close PlatformIO Serial Monitor
- Close any Arduino IDE windows
- Only one program can use the COM port at a time

**"Python not found"**
- Install Python from: https://www.python.org/
- Check "Add Python to PATH" during installation

**"No module named 'serial'"**
- Install pyserial: `pip install pyserial`

**Need to find COM port?**
- Run: `python find_com_port.py`
- Or check Device Manager → Ports (COM & LPT)

## Tips:

- 📊 **Data Rate**: 10 records/second = 36,000 records/hour
- 💾 **File Size**: ~1 hour = ~3-5 MB CSV file  
- ⏱️ **Excel Limit**: Can handle millions of rows (Excel 2016+)
- 🔄 **Multiple Sessions**: Each run overwrites previous data
- 💡 **Rename Files**: Rename CSV files to keep multiple sessions

## Example Analysis in Excel:

1. **Humidity Trend Chart:**
   - Select Time and both Humidity columns
   - Insert → Line Chart

2. **Fan Operation Timeline:**
   - Create chart showing ExhaustFan_Status and SupplyFan_Status
   - Visualize on/off cycles

3. **Temperature vs Humidity:**
   - Scatter plot to see correlation
   - Helps optimize Peltier operation

4. **Dehumidifying Efficiency:**
   - Compare humidity before/after fan cycles
   - Calculate time to reach target humidity

---

**Need Help?**
Read `DATA_LOGGER_README.md` for detailed information.
