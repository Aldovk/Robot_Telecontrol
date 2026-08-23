// --- OpenRB-150: ATmega328P Sensor Reader ---
uint8_t packetIndex = 0;
uint8_t rxBuffer[11]; // 1 Header (0xAA) + 5 * 2 Bytes
uint16_t sensorValues[5];

void setup() {
  // USB Serial to talk to the laptop (ROS node)
  Serial.begin(115200);
  while(!Serial);

  // Serial3 to listen to the custom ATmega328P board
  Serial3.begin(115200);
  
  // Optional brief message (comment out if your Python parser expects ONLY raw numbers)
  // Serial.println("OpenRB Reader Ready");
}

void loop() {
  // Read incoming bytes from ATmega328P on Serial3
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
      
      if (packetIndex == 11) { // Full packet received
        // Reconstruct 16-bit values from low/high bytes[cite: 1]
        for (int i = 0; i < 5; i++) {
          uint8_t low = rxBuffer[1 + (i * 2)];
          uint8_t high = rxBuffer[2 + (i * 2)];
          sensorValues[i] = ((uint16_t)high << 8) | low;
        }
        
        // Print comma-separated values to USB Serial for easy Python parsing
        Serial.print(sensorValues[0]);
        for (int i = 1; i < 5; i++) {
          Serial.print(",");
          Serial.print(sensorValues[i]);
        }
        Serial.println();
        
        packetIndex = 0; // Reset packet tracking[cite: 1]
      }
    }
  }
}