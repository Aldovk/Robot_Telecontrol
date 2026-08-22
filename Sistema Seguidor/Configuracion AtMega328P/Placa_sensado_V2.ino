/*Circuito de Comunicacion V2
Con conteo entre los 5 sensores.*/
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

// --- Calibration ---
const int SENSOR_MIN = 595; 
const int SENSOR_MAX = 680; 

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
  int sensorValues[5];
  int maxVal = -1;
  int maxIndex = 0;

  // 1. Loop through all 5 sensors
  for (int i = 0; i < 5; i++) {
    // Set MUX to channel i
    digitalWrite(S0, (i & 1) ? HIGH : LOW);
    digitalWrite(S1, (i & 2) ? HIGH : LOW);
    digitalWrite(S2, (i & 4) ? HIGH : LOW);
    
    delayMicroseconds(20); // Stand_by
    
    // Read and Scale
    int raw = analogRead(ADC_PIN);
    sensorValues[i] = raw;
    
    // Track Max
    if (raw > maxVal) {
      maxVal = raw;
      maxIndex = i;
    }
  }

  // 2. Display the Index of the Max Sensor (0-4)
  displayNumber(maxIndex);

  // 3. Send All 5 Values (10 Bytes) + Header
  // Header: 0xAA
  // Data: [Val0_L, Val0_H, Val1_L, Val1_H, ... Val4_L, Val4_H]
  Serial.write(0xAA);
  for (int i = 0; i < 5; i++) {
    Serial.write(lowByte(sensorValues[i]));
    Serial.write(highByte(sensorValues[i]));
  }

  // Debug LED
  digitalWrite(DEBUG_LED, HIGH);
  delay(50);
  digitalWrite(DEBUG_LED, LOW);
  
  // Keep the 1s delay for now for debugging
  delay(1000); 
}

void displayNumber(int num) {
  // Only display 0-4
  if (num > 4) num = 0;
  digitalWrite(DISPLAY_A, (num & 1) ? HIGH : LOW); 
  digitalWrite(DISPLAY_B, (num & 2) ? HIGH : LOW); 
  digitalWrite(DISPLAY_C, (num & 4) ? HIGH : LOW); 
}