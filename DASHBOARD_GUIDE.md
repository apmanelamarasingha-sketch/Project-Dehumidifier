# Excel Dashboard - Quick Guide

## Step-by-Step Instructions

### 1. Collect Data First
```powershell
python data_logger.py
```
- Let it run for at least 5-10 minutes to collect meaningful data
- Press `Ctrl+C` to stop when done
- Data saved to: `Analyze Data\dehumidifier_data.csv`

### 2. Create Dashboard
```powershell
python create_dashboard.py
```
- Reads the CSV file
- Creates Excel file with graphs
- Output: `Analyze Data\dehumidifier_dashboard.xlsx`

### 3. Open Dashboard
Double-click: `Analyze Data\dehumidifier_dashboard.xlsx`

## What's Included

### 📊 Sheets in the Dashboard:

1. **Raw Data**
   - All your collected data imported from CSV
   - Timestamp, humidity, temperature, system states

2. **Calculations**
   - Dehumidification rate analysis
   - Duty cycle calculations
   - Temperature impact metrics
   - Automatic formulas

3. **Humidity Trends**
   - Line graph showing humidity over time
   - Both containers displayed
   - Last 500 data points (or all if less)

4. **Temperature Impact**
   - Temperature trends during operation
   - Temperature drop calculations
   - Summary metrics

5. **System Activity**
   - Fans and Peltier ON/OFF status
   - Duty cycle summary
   - Running time statistics

## 📈 Key Metrics Explained

### Dehumidification Rate
- **What**: How fast humidity drops when system is ON
- **Unit**: % RH (average humidity when fans running)
- **Location**: Calculations sheet, rows 5-8

### System Duty Cycle
- **What**: Percentage of time system is actively running
- **Formula**: (Time ON / Total Time) × 100%
- **Good Range**: 20-40% indicates efficient system
- **Location**: Calculations sheet, rows 13-16

### Temperature Impact
- **What**: Temperature change caused by dehumidification
- **Calculation**: Avg Temp (Fans OFF) - Avg Temp (Fans ON)
- **Expected**: 2-5°C drop during operation
- **Location**: Calculations sheet, rows 21-23

## 🔄 Updating Dashboard

To update with new data:

1. **Collect more data**: Run `python data_logger.py` again
   - This appends to the existing CSV file

2. **Regenerate dashboard**: Run `python create_dashboard.py`
   - This reads the updated CSV and creates fresh graphs

**Note**: Each time you run `create_dashboard.py`, it overwrites the Excel file with updated data.

## 💡 Tips

### For Best Results:
- Collect data for at least **1-2 hours** for meaningful analysis
- Run during actual dehumidification cycles (not idle time)
- Check "System Activity" sheet to verify system was actually running

### Interpreting Results:

**Good Performance:**
- Duty cycle: 20-40%
- Temperature drop: 2-5°C
- Humidity trends show clear downward slope when system ON

**Issues to Watch For:**
- Duty cycle > 60% = System struggling to maintain humidity
- Temperature drop > 10°C = Excessive cooling
- Flat humidity curves = System not effective

## 🐛 Troubleshooting

**"Error: CSV file not found!"**
→ Run `python data_logger.py` first to collect data

**"No data in CSV file!"**
→ Make sure ESP32 is connected and logger ran for some time

**"ModuleNotFoundError: No module named 'pandas'"**
→ Run: `pip install pandas openpyxl`

**Charts look strange**
→ Collect more data (at least 50-100 records minimum)

**Excel file won't open**
→ Make sure Microsoft Excel or compatible software is installed

## 📁 File Locations

All files saved in:
```
Analyze Data/
├── dehumidifier_data.csv        # Raw dataset
└── dehumidifier_dashboard.xlsx  # Dashboard with graphs
```

---

**Need more help?** See `LOGGING_UPDATE.md` for detailed documentation.
