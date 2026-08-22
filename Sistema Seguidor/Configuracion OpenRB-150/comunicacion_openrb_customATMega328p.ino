// --- Global Variables ---
uint8_t packetIndex = 0;
uint8_t lowByteVal = 0;
uint8_t highByteVal = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  Serial.println("--- OpenRB: Sensor 0 Parser Ready ---");
  Serial3.begin(115200);
}

void loop() {
  if (Serial3.available() > 0) {
    uint8_t b = Serial3.read();
    
    // State Machine to parse 3 bytes: Header(0xAA), Low, High
    if (packetIndex == 0) {
      // Waiting for Header
      if (b == 0xAA) {
        packetIndex = 1; // Got Header, next is Low Byte
      }
    } 
    else if (packetIndex == 1) {
      // Got Header, now storing Low Byte
      lowByteVal = b;
      packetIndex = 2; // Next is High Byte
    } 
    else if (packetIndex == 2) {
      // Got Header + Low, now storing High Byte
      highByteVal = b;
      
      // Combine: (High << 8) | Low
      uint16_t val = ((uint16_t)highByteVal << 8) | lowByteVal;
      
      Serial.print("Parsed Value: ");
      Serial.println(val);
      
      // Reset state for next packet
      packetIndex = 0;
    }
  }
  delay(10);
}