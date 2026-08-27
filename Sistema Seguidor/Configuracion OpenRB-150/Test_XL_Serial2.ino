#include <Dynamixel2Arduino.h>

// --- Config AX12 on Integrated Ports (Serial1) ---
// Serial2 uses the built-in transceiver hardware, so no direction pin is needed (-1)
const uint8_t AX_ID = 123;          
Dynamixel2Arduino dxl_ax(Serial1, -1);

// --- Configuration for AX-12 on External Pins + 74LS241 Buffer (Serial1) ---
const int XL_DIR_PIN = 5;          // Your 74LS241 direction pin for Serial1
const uint8_t XL_ID = 21;           
Dynamixel2Arduino dxl_xl(Serial2, XL_DIR_PIN);

using namespace DYNAMIXEL;

int xlAngle = 150; // Start at midpoint
int axAngle = 150; // Start at midpoint

void setup() {
  // Initialize USB Serial for keyboard input from Serial Monitor
  Serial.begin(115200);
  while(!Serial);

  // --- Initialize XL-320 (Protocol 2.0) ---
  dxl_xl.begin(1000000); 
  dxl_xl.setPortProtocolVersion(2.0);
  dxl_xl.torqueOff(XL_ID);
  dxl_xl.setOperatingMode(XL_ID, OP_POSITION);
  dxl_xl.torqueOn(XL_ID);
  dxl_xl.setGoalPosition(XL_ID, xlAngle, UNIT_DEGREE);

  // --- Initialize AX-12 (Protocol 1.0) ---
  dxl_ax.begin(1000000); // Change to 57600 if your AX-12 baud is different
  dxl_ax.setPortProtocolVersion(1.0);
  dxl_ax.torqueOff(AX_ID);
  dxl_ax.setOperatingMode(AX_ID, OP_POSITION);
  dxl_ax.torqueOn(AX_ID);
  dxl_ax.setGoalPosition(AX_ID, axAngle, UNIT_DEGREE);

  Serial.println("--- Dual Servo Simultaneous Controller ---");
  Serial.println("XL-320 (ID 21): Press '+' to increase, '-' to decrease angle");
  Serial.println("AX-12  (ID 123) : Press 'c' to increase, 'v' to decrease angle");
}

void loop() {
  if (Serial.available() > 0) {
    char key = Serial.read();
    bool updated = false;

    // XL-320 Controls (+ / -)
    if (key == '+') {
      xlAngle += 10;
      updated = true;
    } else if (key == '-') {
      xlAngle -= 10;
      updated = true;
    }

    // AX-12 Controls (c / v)
    else if (key == 'c' || key == 'C') {
      axAngle += 10;
      updated = true;
    } else if (key == 'v' || key == 'V') {
      axAngle -= 10;
      updated = true;
    }

    if (updated) {
      // Constrain safely between 0 and 300 degrees
      xlAngle = constrain(xlAngle, 0, 300);
      axAngle = constrain(axAngle, 0, 300);

      dxl_xl.setGoalPosition(XL_ID, xlAngle, UNIT_DEGREE);
      dxl_ax.setGoalPosition(AX_ID, axAngle, UNIT_DEGREE);

      // Print status feedback
      Serial.print("XL-320 Target: "); Serial.print(xlAngle);
      Serial.print("° (Pos: "); Serial.print(dxl_xl.getPresentPosition(XL_ID, UNIT_DEGREE)); 
      Serial.print("°) | AX-12 Target: "); Serial.print(axAngle);
      Serial.print("° (Pos: "); Serial.print(dxl_ax.getPresentPosition(AX_ID, UNIT_DEGREE)); 
      Serial.println("°)");
    }
  }
}
