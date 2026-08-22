/* Circuito de Comunicacion V3
   With 7 segment display counter */
#include <Wire.h>

// --- Config ---
const int S0 = 6; 
const int S1 = 7; 
const int S2 = 8; 
const int ADC_PIN = A0;

// 7-Segment Display Pins
const int DISPLAY_A = 2; 
const int DISPLAY_B = 4; 
const int DISPLAY_C = 3; 
const int DEBUG_LED = 5;

// --- Individual Sensor Calibration ---
// Index 0 through 4: {min, max}
const int sensorMin[5] = {600, 550, 555, 550, 565};
const int sensorMax[5] = {670, 700, 690, 740, 720};

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(DISPLAY_A, OUTPUT);
  pinMode(DISPLAY_B, OUTPUT);
  pinMode(DISPLAY_C, OUTPUT);
  pinMode(ADC_PIN, INPUT);
  pinMode(DEBUG_LED, OUTPUT);
  
  Serial.begin(115200);
  Serial.println("5-Sensor Loop Started");
  
  digitalWrite(DEBUG_LED, HIGH);
  delay(200);
  digitalWrite(DEBUG_LED, LOW);
}

void loop() {
  int scaledValues[5];
  int maxVal = -1;
  int maxIndex = 0;

  // 1. Loop through all 5 sensors
  for (int i = 0; i < 5; i++) {
    // Set MUX to channel i
    digitalWrite(S0, (i & 1) ? HIGH : LOW);
    digitalWrite(S1, (i & 2) ? HIGH : LOW);
    digitalWrite(S2, (i & 4) ? HIGH : LOW);
    
    delayMicroseconds(20); // Stand_by
    
    // Read raw value
    int raw = analogRead(ADC_PIN);
    
    // Map individually and constrain to 0-1023
    long mappedVal = map(raw, sensorMin[i], sensorMax[i], 0, 1023);
    mappedVal = constrain(mappedVal, 0, 1023);
    
    scaledValues[i] = (int)mappedVal;
    
    // Track Max 
    if (scaledValues[i] > maxVal) {
      maxVal = scaledValues[i];
      maxIndex = i;
    }
  }

  // 2. Display Index
  displayNumber(maxIndex);

  // 3. Send All 5 Scaled Values (10 Bytes) + Header
  // Header: 0xAA
  Serial.write(0xAA);
  for (int i = 0; i < 5; i++) {
    Serial.write(lowByte(scaledValues[i]));
    Serial.write(highByte(scaledValues[i]));
  }

  // Debug LED
  digitalWrite(DEBUG_LED, HIGH);
  delay(25);
  digitalWrite(DEBUG_LED, LOW);
  
  delay(50); 
}

void displayNumber(int num) {
  // Index 0 to 4
  if (num > 4) num = 0;
  digitalWrite(DISPLAY_A, (num & 1) ? HIGH : LOW); 
  digitalWrite(DISPLAY_B, (num & 2) ? HIGH : LOW); 
  digitalWrite(DISPLAY_C, (num & 4) ? HIGH : LOW); 
}