//Comunicacion Arduino UNO
// --- Arduino UNO Sensor Node ---
// Sends 5 16-bit values (e.g., 3 encoders, 2 pots) wrapped in a binary packet

void setup() {
  Serial.begin(115200); // Match OpenRB-150 Serial3 baud rate
}

void loop() {
  // 1. Read your sensors (replace with actual analogRead() or encoder logic)
  uint16_t sensor1 = analogRead(A0); // Pot 1 (0-1023)
  uint16_t sensor2 = analogRead(A1); // Pot 2 (0-1023)
  uint16_t sensor3 = 500;            // Placeholder for Encoder 1
  uint16_t sensor4 = 600;            // Placeholder for Encoder 2
  uint16_t sensor5 = 700;            // Placeholder for Encoder 3

  uint16_t sensorData[5] = {sensor1, sensor2, sensor3, sensor4, sensor5};

  // 2. Build and send the 11-byte packet (0xAA header + 5 * 2 bytes)
  Serial.write(0xAA); // Header byte

  for (int i = 0; i < 5; i++) {
    uint8_t lowByte = lowByte(sensorData[i]);
    uint8_t highByte = highByte(sensorData[i]);
    
    Serial.write(lowByte);
    Serial.write(highByte);
  }

  // 50 ms delay for a steady 20 Hz loop rate (safe and fast for ROS 2)
  delay(50);
}