"""
Simple utility to find ESP32 COM port
"""

import serial.tools.list_ports

def main():
    print("=" * 60)
    print("Available COM Ports")
    print("=" * 60)
    
    ports = serial.tools.list_ports.comports()
    
    if not ports:
        print("No COM ports found!")
        print("\nMake sure:")
        print("  1. ESP32 is connected via USB")
        print("  2. USB drivers are installed")
        print("  3. Device is recognized in Device Manager")
    else:
        for i, port in enumerate(ports, 1):
            print(f"\n{i}. {port.device}")
            print(f"   Description: {port.description}")
            print(f"   Hardware ID: {port.hwid}")
            
            # Highlight likely ESP32 ports
            desc_lower = port.description.lower()
            if any(keyword in desc_lower for keyword in ['cp210', 'ch340', 'usb-serial', 'silicon labs']):
                print(f"   ⭐ This looks like an ESP32!")
    
    print("\n" + "=" * 60)

if __name__ == "__main__":
    main()
    input("\nPress Enter to exit...")
