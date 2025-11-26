"""
ESP32 Dehumidifier Data Logger
Reads CSV data from ESP32 serial port and saves to file in real-time
"""

import serial
import serial.tools.list_ports
import time
from datetime import datetime
import os

# Configuration
BAUD_RATE = 115200
CSV_OUTPUT_DIR = r"C:\Users\User\OneDrive - itum.mrt.ac.lk\Desktop\Lecture Modules\Semester-4\Project 2\Project-Dehumidifier\Analyze Data"
CSV_FILENAME = "dehumidifier_data.csv"

def find_esp32_port():
    """Automatically detect ESP32 COM port"""
    ports = serial.tools.list_ports.comports()
    for port in ports:
        # ESP32 typically shows as "CP210x" or "CH340" or "Silicon Labs"
        if "CP210" in port.description or "CH340" in port.description or "USB-SERIAL" in port.description:
            return port.device
    return None

def main():
    # Create output directory if it doesn't exist
    os.makedirs(CSV_OUTPUT_DIR, exist_ok=True)
    
    # Full path to CSV file
    csv_filepath = os.path.join(CSV_OUTPUT_DIR, CSV_FILENAME)
    
    # Try to find ESP32 port automatically
    port = find_esp32_port()
    
    if port is None:
        print("Available COM ports:")
        ports = serial.tools.list_ports.comports()
        for i, p in enumerate(ports):
            print(f"{i+1}. {p.device} - {p.description}")
        
        port_num = input("\nEnter COM port (e.g., COM3): ").strip()
        port = port_num
    else:
        print(f"Found ESP32 on port: {port}")
    
    print(f"\nConnecting to {port} at {BAUD_RATE} baud...")
    
    try:
        # Open serial connection
        ser = serial.Serial(port, BAUD_RATE, timeout=1)
        time.sleep(2)  # Wait for connection to stabilize
        
        print(f"Connected! Logging data to: {csv_filepath}")
        print("Press Ctrl+C to stop logging\n")
        
        # Open CSV file for writing
        with open(csv_filepath, 'w', encoding='utf-8') as csv_file:
            # Track if we've written the header
            header_written = False
            
            data_count = 0
            
            while True:
                try:
                    # Read line from serial port
                    if ser.in_waiting > 0:
                        line = ser.readline().decode('utf-8', errors='ignore').strip()
                        
                        if line:
                            # Check if this is the header line
                            if line.startswith("Time(ms)"):
                                if not header_written:
                                    # Add timestamp column to header
                                    csv_file.write(f"Timestamp,{line}\n")
                                    csv_file.flush()
                                    header_written = True
                                    print("✓ CSV Header written")
                                    print("="*60)
                            elif ',' in line and not '=' in line and header_written:
                                # This is CSV data (contains commas but not status messages)
                                # Check if it's a valid data line (starts with number)
                                if line[0].isdigit():
                                    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
                                    csv_file.write(f"{timestamp},{line}\n")
                                    csv_file.flush()  # Write immediately to disk
                                    
                                    data_count += 1
                                    # Show progress every 50 records (5 seconds at 100ms)
                                    if data_count % 50 == 0:
                                        print(f"✓ Logged {data_count} records... (latest: {timestamp})")
                            else:
                                # This is a monitoring message - display to console
                                if line and not line.startswith("Time(ms)"):
                                    print(line)
                            
                except UnicodeDecodeError:
                    # Skip lines with decode errors
                    continue
                
                time.sleep(0.001)  # Minimal delay for 100ms logging
                
    except serial.SerialException as e:
        print(f"Error: Could not open serial port {port}")
        print(f"Details: {e}")
        print("\nMake sure:")
        print("1. ESP32 is connected via USB")
        print("2. The correct COM port is selected")
        print("3. No other program (like Arduino IDE or PlatformIO Monitor) is using the port")
    
    except KeyboardInterrupt:
        print("\n\nLogging stopped by user")
        print(f"Data saved to: {csv_filepath}")
    
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Serial port closed")

if __name__ == "__main__":
    print("=" * 60)
    print("ESP32 Dehumidifier Data Logger")
    print("=" * 60)
    main()
