#include <OpenRB-150.h>
#include <Dynamixel2Arduino.h>

// --- Pin Definitions ---
// -1 means no external direction pin needed (uses diode/direct single-wire bridge or internal handling)
#define AX_DIR_PIN      -1          
#define XL_DIR_PIN      -1          

#define DXL_BAUD        1000000     // 1 Mbps baud rate

// --- Serial Objects ---
// Serial1: Onboard DXL connectors (AX-12)
// Serial2: Header Pins 0 & 1 (XL-320 bus)
Dynamixel2Arduino dxl_ax(Serial1, AX_DIR_PIN); 
Dynamixel2Arduino dxl_xl(Serial2, XL_DIR_PIN); 

// Serial3: Header Pins 8 & 9 (ATmega328P)
#define serial_atmega Serial3

// --- Buffers ---
uint8_t sensorBuffer[11]; 
int sensorPacketIndex = 0;
uint16_t currentSensors[5] = {0};
uint16_t targetPositions[5] = {0};

void setup() {
  // 1. USB Serial Link to ROS / Laptop
  Serial.begin(115200);

  // 2. Initialize AX-12 (Protocol 1.0 @ 1 Mbps)
  dxl_ax.begin(DXL_BAUD);
  dxl_ax.setPortProtocolVersion(1.0);

  // 3. Initialize XL-320 (Protocol 2.0 @ 1 Mbps)
  dxl_xl.begin(DXL_BAUD);
  dxl_xl.setPortProtocolVersion(2.0);

  // 4. Turn Torque ON
  dxl_ax.torqueOn(1);
  dxl_ax.torqueOn(2);
  dxl_ax.torqueOn(3);
  
  dxl_xl.torqueOn(10);
  dxl_xl.torqueOn(11);

  // 5. Sensor Board Serial (ATmega328P @ 115200 bps)
  serial_atmega.begin(115200);
}

void loop() {
  // --- 1. Read ATmega Sensors ---
  while (serial_atmega.available() > 0) {
    uint8_t b = serial_atmega.read();
    
    if (sensorPacketIndex == 0) {
      if (b == 0xAA) {
        sensorBuffer[0] = b;
        sensorPacketIndex = 1;
      }
    } else {
      sensorBuffer[sensorPacketIndex++] = b;
      
      if (sensorPacketIndex == 11) {
        for (int i = 0; i < 5; i++) {
          uint8_t low = sensorBuffer[1 + (i * 2)];
          uint8_t high = sensorBuffer[2 + (i * 2)];
          currentSensors[i] = ((uint16_t)high << 8) | low;
        }
        sensorPacketIndex = 0;
      }
    }
  }

  // --- 2. Read Commands from ROS (USB Serial) ---
  if (Serial.available() >= 11) {
    if (Serial.read() == 0xAA) {
      for (int i = 0; i < 5; i++) {
        uint8_t low = Serial.read();
        uint8_t high = Serial.read();
        targetPositions[i] = ((uint16_t)high << 8) | low;
      }
      
      // --- 3. Command Servos ---
      for (int i = 0; i < 5; i++) {
        uint16_t pos = targetPositions[i]; // Value range: 0-1023
        
        if (i < 3) {
          // Servos 1, 2, 3 -> AX-12 (IDs 1, 2, 3)
          dxl_ax.setGoalPosition(i + 1, pos);
        } else {
          // Servos 4, 5 -> XL-320 (IDs 10, 11)
          dxl_xl.setGoalPosition(i + 7, pos); // i=3 -> ID 10, i=4 -> ID 11
        }
      }
    }
  }
}