// --- Global Variables ---
uint8_t packetIndex = 0;
uint8_t buffer[11]; // 1 Header + 5 * 2 Bytes

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  Serial.println("--- OpenRB: 5-Sensor Parser Ready ---");
  Serial3.begin(115200);
}

void loop() {
  if (Serial3.available() > 0) {
    uint8_t b = Serial3.read();
    
    if (packetIndex == 0) {
      if (b == 0xAA) {
        buffer[0] = b;
        packetIndex = 1;
      }
    } else {
      buffer[packetIndex] = b;
      packetIndex++;
      
      if (packetIndex == 11) { // Full packet received
        // Parse 5 values
        for (int i = 0; i < 5; i++) {
          uint8_t low = buffer[1 + (i * 2)];
          uint8_t high = buffer[2 + (i * 2)];
          uint16_t val = ((uint16_t)high << 8) | low;
          
          Serial.print("Sensor ");
          Serial.print(i);
          Serial.print(": ");
          Serial.print(val);
          if (i < 4) Serial.print(" | ");
        }
        Serial.println();
        
        packetIndex = 0; // Reset
      }
    }
  }
  delay(10);
}