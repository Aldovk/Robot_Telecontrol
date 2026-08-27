#include <Dynamixel2Arduino.h>
// Code for openRB_comm
// --- UART Serial3 for Custom ATmega328P Haptic Sensors ---
uint8_t packetIndex = 0;
uint8_t rxBuffer[11]; 
uint16_t hapticSensorData[5]; // Haptic values from local ATmega328P board

// --- AX-12 Configuration on Integrated Ports (Serial1) ---
const uint8_t AX_IDS[] = {111, 112, 113, 121, 122, 123, 124, 125};
const int NUM_AX = sizeof(AX_IDS) / sizeof(AX_IDS[0]);
Dynamixel2Arduino dxl_ax(Serial1, -1);

// --- XL-320 Configuration on External Pins + 74LS241 Buffer (Serial2) ---
const int XL_DIR_PIN = 5;          // 74LS241 direction pin for Serial2
const uint8_t XL_IDS[] = {222, 223, 224, 225};
const int NUM_XL = sizeof(XL_IDS) / sizeof(XL_IDS[0]);
Dynamixel2Arduino dxl_xl(Serial2, XL_DIR_PIN);

using namespace DYNAMIXEL;

void setup() {
  // USB Serial for ROS 2 Python Node communication
  Serial.begin(115200);
  while(!Serial);

  // Serial3 for custom ATmega328P haptic sensor board
  Serial3.begin(115200);

  // --- Init AX-12 Bus Protocol 1.0 ---
  dxl_ax.begin(1000000);
  dxl_ax.setPortProtocolVersion(1.0);
  for (int i = 0; i < NUM_AX; i++) {
    dxl_ax.torqueOff(AX_IDS[i]);
    dxl_ax.setOperatingMode(AX_IDS[i], OP_POSITION);
    dxl_ax.torqueOn(AX_IDS[i]);
  }

  // --- Init XL-320 Bus Protocol 2.0 ---
  dxl_xl.begin(1000000);
  dxl_xl.setPortProtocolVersion(2.0);
  for (int i = 0; i < NUM_XL; i++) {
    dxl_xl.torqueOff(XL_IDS[i]);
    dxl_xl.setOperatingMode(XL_IDS[i], OP_POSITION);
    dxl_xl.torqueOn(XL_IDS[i]);
  }

  // Init Positions AX12:150, XL222-223:230, XL223-224:60
  // One second delay between so it does not pull a large startup current
  // --- Arm
  dxl_ax.setGoalPosition(111, 150, UNIT_DEGREE);
  delay(1000);
  dxl_ax.setGoalPosition(112, 150, UNIT_DEGREE);
  delay(1000);
  dxl_ax.setGoalPosition(113, 150, UNIT_DEGREE);
  delay(1000);
  // --- Hand AX
  dxl_ax.setGoalPosition(121, 60, UNIT_DEGREE);
  delay(1000);
  dxl_ax.setGoalPosition(122, 60, UNIT_DEGREE);
  delay(1000);
  dxl_ax.setGoalPosition(123, 60, UNIT_DEGREE);
  delay(1000);
  dxl_ax.setGoalPosition(124, 60, UNIT_DEGREE);
  delay(1000);
  dxl_ax.setGoalPosition(125, 60, UNIT_DEGREE);
  delay(1000);
  // --- Hand XL
  dxl_xl.setGoalPosition(222, 230, UNIT_DEGREE);
  delay(1000);
  dxl_xl.setGoalPosition(223, 230, UNIT_DEGREE);
  delay(1000);
  dxl_xl.setGoalPosition(224, 60, UNIT_DEGREE);
  delay(1000);
  dxl_xl.setGoalPosition(225, 60, UNIT_DEGREE);
  delay(1000);
}

void loop() {
  // 1. Read local Haptic Sensors from Custom ATmega328P on Serial3
  while (Serial3.available() > 0) {
    uint8_t b = Serial3.read();
    
    if (packetIndex == 0) {
      if (b == 0xAA) {
        rxBuffer[0] = b;
        packetIndex = 1;
      }
    } else {
      rxBuffer[packetIndex] = b;
      packetIndex++;
      
      if (packetIndex == 11) { // Full packet received[cite: 1]
        for (int i = 0; i < 5; i++) {
          uint8_t low = rxBuffer[1 + (i * 2)];
          uint8_t high = rxBuffer[2 + (i * 2)];
          hapticSensorData[i] = ((uint16_t)high << 8) | low;
        }
        
        // Format and print comma-separated haptic values to USB Serial 
        // so Python 'node_follower.py' can pick them up and publish to /haptic_feedback
        // printing all but ring finger, there is 4 dc motors
        Serial.print(hapticSensorData[0]);  //Thumb
        Serial.print(",");
        Serial.print(hapticSensorData[1]); //Index
        Serial.print(",");
        Serial.print(hapticSensorData[2]); //Middle
        Serial.print(",");
        Serial.print(hapticSensorData[3]); //Pinky
        Serial.println();

        packetIndex = 0; 
      }
    }
  }

  // 2. Read incoming commands from Python ROS Node (sent by Arduino Uno Master) via USB Serial
  // Expected format from Python: comma-separated string of 5 master values
  if (Serial.available() > 0) {
    String incomingLine = Serial.readStringUntil('\n');
    incomingLine.trim();

    if (incomingLine.length() > 0) {
      // Simple CSV parser for the 5 incoming master values
      int masterData[5];
      int index = 0;
      int lastIndex = 0;

      for (int i = 0; i < incomingLine.length(); i++) {
        if (incomingLine.charAt(i) == ',' && index < 4) {
          masterData[index++] = incomingLine.substring(lastIndex, i).toInt();
          lastIndex = i + 1;
        }
      }
      masterData[4] = incomingLine.substring(lastIndex).toInt();

      // Now map those Master values (2 pots, 3 encoders)
      // encoders need angle values:
      int armAngle1  = map(masterData[2], 0, 1023, 150, 250);
      int armAngle2  = map(masterData[3], 0, 1023, 150, 240);
      int armAngle3  = map(masterData[4], 0, 1023, 120, 240);
      
      dxl_ax.setGoalPosition(111, armAngle1, UNIT_DEGREE);
      dxl_ax.setGoalPosition(112, armAngle2, UNIT_DEGREE);
      dxl_ax.setGoalPosition(113, armAngle3, UNIT_DEGREE);

      int fingerDOF1_A = map(masterData[0], 0, 1023, 60, 220);
      dxl_ax.setGoalPosition(121, fingerDOF1_A, UNIT_DEGREE);
      dxl_ax.setGoalPosition(122, fingerDOF1_A, UNIT_DEGREE);

      int fingerDOF1_B = map(masterData[1], 0, 1023, 60, 220);
      dxl_ax.setGoalPosition(123, fingerDOF1_B, UNIT_DEGREE);
      dxl_ax.setGoalPosition(124, fingerDOF1_B, UNIT_DEGREE);
      dxl_ax.setGoalPosition(125, fingerDOF1_B, UNIT_DEGREE);

      // pots
      int xlPosIndex = map(masterData[0], 0, 1023, 230, 100);
      int xlPosRest  = map(masterData[1], 0, 1023, 60, 200);
      dxl_xl.setGoalPosition(222, xlPosIndex, UNIT_DEGREE);
      dxl_xl.setGoalPosition(223, xlPosRest, UNIT_DEGREE);
      dxl_xl.setGoalPosition(224, xlPosRest, UNIT_DEGREE);
      dxl_xl.setGoalPosition(225, xlPosRest, UNIT_DEGREE);
    }
  }
}