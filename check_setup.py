"""
Quick setup checker for ESP32 Data Logger
Run this to verify everything is ready
"""

import sys
import os

def check_python():
    """Check Python version"""
    version = sys.version_info
    print(f"✓ Python {version.major}.{version.minor}.{version.micro}")
    if version.major < 3 or (version.major == 3 and version.minor < 6):
        print("  ⚠ Warning: Python 3.6+ recommended")
        return False
    return True

def check_library(name, import_name=None):
    """Check if a library is installed"""
    if import_name is None:
        import_name = name
    
    try:
        __import__(import_name)
        print(f"✓ {name} installed")
        return True
    except ImportError:
        print(f"✗ {name} NOT installed - run: pip install {name}")
        return False

def check_folder(path):
    """Check if folder exists"""
    if os.path.exists(path):
        print(f"✓ Folder exists: {path}")
        return True
    else:
        print(f"✗ Folder does not exist: {path}")
        return False

def check_com_ports():
    """List available COM ports"""
    try:
        import serial.tools.list_ports
        ports = serial.tools.list_ports.comports()
        
        if ports:
            print(f"✓ Found {len(ports)} COM port(s):")
            for port in ports:
                print(f"  - {port.device}: {port.description}")
            return True
        else:
            print("✗ No COM ports found")
            return False
    except:
        print("✗ Cannot check COM ports (pyserial not installed)")
        return False

def main():
    print("=" * 60)
    print("ESP32 Dehumidifier Data Logger - Setup Check")
    print("=" * 60)
    print()
    
    all_good = True
    
    print("1. Checking Python...")
    all_good &= check_python()
    print()
    
    print("2. Checking Required Libraries...")
    all_good &= check_library("pyserial", "serial")
    print()
    
    print("3. Checking Optional Libraries...")
    check_library("pandas")
    check_library("openpyxl")
    print()
    
    print("4. Checking Output Folder...")
    data_folder = r"C:\Users\User\OneDrive - itum.mrt.ac.lk\Desktop\Lecture Modules\Semester-4\Project 2\Project-Dehumidifier\Analyze Data"
    all_good &= check_folder(data_folder)
    print()
    
    print("5. Checking COM Ports...")
    check_com_ports()
    print()
    
    print("=" * 60)
    if all_good:
        print("✓ Setup looks good! You're ready to start logging.")
        print("  Run: python data_logger.py")
    else:
        print("⚠ Some issues found. Please fix them before logging.")
    print("=" * 60)

if __name__ == "__main__":
    main()
    input("\nPress Enter to exit...")
