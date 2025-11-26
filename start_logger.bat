@echo off
REM Dehumidifier Data Logger - Quick Launch Script
REM Double-click this file to start logging data

echo ============================================================
echo   ESP32 Dehumidifier Data Logger
echo ============================================================
echo.

REM Check if Python is installed
python --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Python is not installed or not in PATH
    echo Please install Python from https://www.python.org/
    echo.
    pause
    exit /b 1
)

echo Checking Python libraries...
echo.

REM Check if pyserial is installed
python -c "import serial" >nul 2>&1
if errorlevel 1 (
    echo Installing pyserial...
    pip install pyserial
)

REM Check if pandas is installed
python -c "import pandas" >nul 2>&1
if errorlevel 1 (
    echo Installing pandas...
    pip install pandas
)

REM Check if openpyxl is installed
python -c "import openpyxl" >nul 2>&1
if errorlevel 1 (
    echo Installing openpyxl...
    pip install openpyxl
)

echo.
echo Starting data logger...
echo Press Ctrl+C to stop logging
echo.

REM Run the data logger
python data_logger.py

pause
